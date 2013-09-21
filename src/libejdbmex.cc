/// Matlab ejdb API.
///
/// Kota Yamaguchi 2013

#include "bsonmex.h"
#include "ejdb.h"
#include "ejdbmex.h"
#include "mex/arguments.h"
#include "mex/function.h"
#include "mex/mxarray.h"

using ejdbmex::Database;
using mex::CheckInputArguments;
using mex::CheckOutputArguments;
using mex::MxArray;
using mex::Session;
using mex::VariableInputArguments;

namespace {

/// Parse database input in the arguments.
int ParseDatabaseInput(int nrhs,
                       const mxArray *prhs[],
                       Database** database) {
  if (!database) {
    ERROR("Null pointer.");
  }
  int index = 0;
  int database_id = (nrhs > 0 && MxArray(prhs[index]).isNumeric()) ?
                    MxArray(prhs[index++]).toInt() : 0;
  *database = Session<Database>::get(database_id);
  if (!(*database)) {
    ERROR("No open database found.");
  }
  return index;
}

/// Parse collection input in the arguments.
int ParseCollectionInput(int nrhs,
                         const mxArray *prhs[],
                         EJCOLL** collection,
                         Database** database) {
  int index = ParseDatabaseInput(nrhs, prhs, database);
  string collection_name = MxArray(prhs[index++]).toString();
  *collection = (*database)->getMutableCollection(collection_name.c_str());
  return index;
}

/// Common query operation interface.
void QueryOperation(int nlhs,
                    mxArray *plhs[],
                    int nrhs,
                    const mxArray *prhs[],
                    int flags) {
  CheckInputArguments(2, 4, nrhs);
  CheckOutputArguments(0, 1, nlhs);
  Database* database;
  EJCOLL* collection;
  int index = ParseCollectionInput(nrhs, prhs, &collection, &database);
  bson query, hints;
  if (!ConvertMxArrayToBSON(prhs[index++], BSON_FLAG_QUERY_MODE, &query))
    ERROR(bson_first_errormsg(&query));
  bool with_hints = false;
  if (index < nrhs) {
    if (ConvertMxArrayToBSON(prhs[index++], BSON_FLAG_QUERY_MODE, &hints))
      with_hints = true;
    else
      ERROR(bson_first_errormsg(&hints));
  }
  if (!database->find(collection,
                      &query,
                      (with_hints) ? &hints : NULL,
                      &plhs[0],
                      flags))
    ERROR("Failed to query: %s", database->errorMessage());
  bson_destroy(&query);
  if (with_hints)
    bson_destroy(&hints);
}

/// Common setIndex operation interface.
void SetIndexOperation(int nlhs,
                       mxArray *plhs[],
                       int nrhs,
                       const mxArray *prhs[],
                       int flags) {
  CheckInputArguments(2, 3, nrhs);
  CheckOutputArguments(0, 0, nlhs);
  Database* database;
  EJCOLL* collection;
  int index = ParseCollectionInput(nrhs, prhs, &collection, &database);
  string path = MxArray(prhs[index++]).toString();
  if (!ejdbsetindex(collection, path.c_str(), flags)) {
    ERROR("Failed to set index: %s", database->errorMessage());
  }
}

MEX_FUNCTION(open) (int nlhs,
                    mxArray *plhs[],
                    int nrhs,
                    const mxArray *prhs[]) {
  CheckInputArguments(1, 1024, nrhs);
  CheckOutputArguments(0, 1, nlhs);
  VariableInputArguments options;
  options.set("READER", false);
  options.set("WRITER", nrhs < 2);
  options.set("CREAT", nrhs < 2);
  options.set("TRUNC", false);
  options.set("NOLCK", false);
  options.set("LCKNB", false);
  options.set("TSYNC", false);
  options.update(prhs + 1, prhs + nrhs);
  string filename = MxArray(prhs[0]).toString();
  int mode = ((options["READER"].toBool()) ? JBOREADER : 0) |
             ((options["WRITER"].toBool()) ? JBOWRITER : 0) |
             ((options["CREAT"].toBool()) ? JBOCREAT : 0) |
             ((options["TRUNC"].toBool()) ? JBOTRUNC : 0) |
             ((options["NOLCK"].toBool()) ? JBONOLCK : 0) |
             ((options["LCKNB"].toBool()) ? JBOLCKNB : 0) |
             ((options["TSYNC"].toBool()) ? JBOTSYNC : 0);
  Database* database = NULL;
  int database_id = Session<Database>::create(&database);
  if (!database->open(filename.c_str(), mode)) {
    Session<Database>::destroy(database_id);
    ERROR("Failed to create a database at %s: %s", 
          filename.c_str(), 
          database->errorMessage());
  }
  plhs[0] = MxArray(database_id).getMutable();
}

MEX_FUNCTION(isopen) (int nlhs,
                      mxArray *plhs[],
                      int nrhs,
                      const mxArray *prhs[]) {
  CheckInputArguments(0, 1, nrhs);
  CheckOutputArguments(0, 1, nlhs);
  int database_id = (nrhs > 0) ? MxArray(prhs[0]).toInt() : 0;
  plhs[0] = mxCreateLogicalScalar(Session<Database>::is_valid(database_id));
}

MEX_FUNCTION(close) (int nlhs,
                     mxArray *plhs[],
                     int nrhs,
                     const mxArray *prhs[]) {
  CheckInputArguments(0, 1, nrhs);
  CheckOutputArguments(0, 0, nlhs);
  int database_id = (nrhs > 0 && MxArray(prhs[0]).isNumeric()) ?
                    MxArray(prhs[0]).toInt() : 0;
  Database* database = Session<Database>::get(database_id);
  if (!database)
    ERROR("No open database found.");
  database->close();
  Session<Database>::destroy(database_id);
}

MEX_FUNCTION(ensureCollection) (int nlhs,
                                mxArray *plhs[],
                                int nrhs,
                                const mxArray *prhs[]) {
  CheckInputArguments(1, 1024, nrhs);
  CheckOutputArguments(0, 0, nlhs);
  Database* database;
  int index = ParseDatabaseInput(nrhs, prhs, &database);
  string collection_name = MxArray(prhs[index++]).toString();
  VariableInputArguments options;
  options.set("LARGE",         false);
  options.set("COMPRESSED",    false);
  options.set("RECORDS",       65535);
  options.set("CACHEDRECORDS", 0);
  options.update(prhs + index, prhs + nrhs);
  EJCOLLOPTS collection_options = {
    options["LARGE"].toBool(),
    options["COMPRESSED"].toBool(),
    options["RECORDS"].toDouble(),
    options["CACHEDRECORDS"].toInt()
  };
  EJCOLL* collection = ejdbcreatecoll(database->getMutable(),
                                      collection_name.c_str(),
                                      &collection_options);
  if (!collection) {
    ERROR(database->errorMessage());
  }
}

MEX_FUNCTION(dropCollection) (int nlhs,
                              mxArray *plhs[],
                              int nrhs,
                              const mxArray *prhs[]) {
  CheckInputArguments(1, 1024, nrhs);
  CheckOutputArguments(0, 0, nlhs);
  Database* database;
  int index = ParseDatabaseInput(nrhs, prhs, &database);
  string collection_name = MxArray(prhs[index++]).toString();
  VariableInputArguments options;
  options.set("UNLINK", true);
  options.update(prhs + index, prhs + nrhs);
  if (!ejdbrmcoll(database->getMutable(),
                  collection_name.c_str(),
                  options["UNLINK"].toBool())) {
    ERROR(database->errorMessage());
  }
}

MEX_FUNCTION(save) (int nlhs,
                    mxArray *plhs[],
                    int nrhs,
                    const mxArray *prhs[]) {
  CheckInputArguments(2, 1024, nrhs);
  CheckOutputArguments(0, 1, nlhs);
  Database* database;
  int index = ParseDatabaseInput(nrhs, prhs, &database);
  string collection_name = MxArray(prhs[index++]).toString();
  int num_objects = nrhs - index;
  plhs[0] = mxCreateCellMatrix(1, num_objects);
  for (int i = 0; i < num_objects; ++i) {
    bson value;
    if (!ConvertMxArrayToBSON(prhs[index++], 0, &value))
      ERROR(bson_first_errormsg(&value));
    string object_id;
    if (!database->save(collection_name.c_str(), &value, &object_id))
      ERROR(database->errorMessage());
    bson_destroy(&value);
    mxSetCell(plhs[0], i, MxArray(object_id).getMutable());
  }
  if (num_objects == 1) {
    mxArray* value = mxDuplicateArray(mxGetCell(plhs[0], 0));
    mxDestroyArray(plhs[0]);
    plhs[0] = value;
  }
}

MEX_FUNCTION(load) (int nlhs,
                    mxArray *plhs[],
                    int nrhs,
                    const mxArray *prhs[]) {
  CheckInputArguments(1, 2, nrhs);
  CheckOutputArguments(0, 1, nlhs);
  Database* database;
  EJCOLL* collection;
  int index = ParseCollectionInput(nrhs, prhs, &collection, &database);
  string object_id = MxArray(prhs[index++]).toString();
  bson* value;
  if (database->load(collection, object_id.c_str(), &value)) {
    if (!ConvertBSONToMxArray(value, &plhs[0])) {
      ERROR(bson_first_errormsg(value));
    }
    bson_del(value);
  }
  else
    plhs[0] = mxCreateCellMatrix(0, 0);
}

MEX_FUNCTION(remove) (int nlhs,
                      mxArray *plhs[],
                      int nrhs,
                      const mxArray *prhs[]) {
  CheckInputArguments(1, 2, nrhs);
  CheckOutputArguments(0, 0, nlhs);
  Database* database;
  EJCOLL* collection;
  int index = ParseCollectionInput(nrhs, prhs, &collection, &database);
  string object_id = MxArray(prhs[index++]).toString();
  if (!database->remove(collection, object_id.c_str())) {
    ERROR(database->errorMessage());
  }
}

MEX_FUNCTION(find) (int nlhs,
                    mxArray *plhs[],
                    int nrhs,
                    const mxArray *prhs[]) {
  QueryOperation(nlhs, plhs, nrhs, prhs, 0);
}

MEX_FUNCTION(findOne) (int nlhs,
                       mxArray *plhs[],
                       int nrhs,
                       const mxArray *prhs[]) {
  QueryOperation(nlhs, plhs, nrhs, prhs, JBQRYFINDONE);
}

MEX_FUNCTION(count) (int nlhs,
                     mxArray *plhs[],
                     int nrhs,
                     const mxArray *prhs[]) {
  QueryOperation(nlhs, plhs, nrhs, prhs, JBQRYCOUNT);
}

MEX_FUNCTION(update) (int nlhs,
                      mxArray *plhs[],
                      int nrhs,
                      const mxArray *prhs[]) {
  CheckInputArguments(2, 4, nrhs);
  CheckOutputArguments(0, 1, nlhs);
  Database* database;
  EJCOLL* collection;
  int index = ParseCollectionInput(nrhs, prhs, &collection, &database);
  bson query, hints;
  if (!ConvertMxArrayToBSON(prhs[index++], BSON_FLAG_QUERY_MODE, &query))
    ERROR(bson_first_errormsg(&query));
  bool with_hints = false;
  if (index < nrhs) {
    if (ConvertMxArrayToBSON(prhs[index++], BSON_FLAG_QUERY_MODE, &hints))
      with_hints = true;
    else
      ERROR(bson_first_errormsg(&hints));
  }
  uint32_t num_updates = ejdbupdate(collection, &query, NULL, 0, &hints, NULL);
  bson_destroy(&query);
  if (with_hints)
    bson_destroy(&hints);
  plhs[0] = MxArray(static_cast<double>(num_updates)).getMutable();
}

MEX_FUNCTION(dropIndexes) (int nlhs,
                           mxArray *plhs[],
                           int nrhs,
                           const mxArray *prhs[]) {
  SetIndexOperation(nlhs, plhs, nrhs, prhs, JBIDXDROPALL);
}

MEX_FUNCTION(optimizeIndexes) (int nlhs,
                               mxArray *plhs[],
                               int nrhs,
                               const mxArray *prhs[]) {
  SetIndexOperation(nlhs, plhs, nrhs, prhs, JBIDXOP);
}

MEX_FUNCTION(ensureStringIndex) (int nlhs,
                                 mxArray *plhs[],
                                 int nrhs,
                                 const mxArray *prhs[]) {
  SetIndexOperation(nlhs, plhs, nrhs, prhs, JBIDXSTR);
}

MEX_FUNCTION(rebuildStringIndex) (int nlhs,
                                  mxArray *plhs[],
                                  int nrhs,
                                  const mxArray *prhs[]) {
  SetIndexOperation(nlhs, plhs, nrhs, prhs, JBIDXSTR | JBIDXREBLD);
}

MEX_FUNCTION(dropStringIndex) (int nlhs,
                               mxArray *plhs[],
                               int nrhs,
                               const mxArray *prhs[]) {
  SetIndexOperation(nlhs, plhs, nrhs, prhs, JBIDXSTR | JBIDXDROP);
}

MEX_FUNCTION(ensureIStringIndex) (int nlhs,
                                  mxArray *plhs[],
                                  int nrhs,
                                  const mxArray *prhs[]) {
  SetIndexOperation(nlhs, plhs, nrhs, prhs, JBIDXISTR);
}

MEX_FUNCTION(rebuildIStringIndex) (int nlhs,
                                   mxArray *plhs[],
                                   int nrhs,
                                   const mxArray *prhs[]) {
  SetIndexOperation(nlhs, plhs, nrhs, prhs, JBIDXISTR | JBIDXREBLD);
}

MEX_FUNCTION(dropIStringIndex) (int nlhs,
                                mxArray *plhs[],
                                int nrhs,
                                const mxArray *prhs[]) {
  SetIndexOperation(nlhs, plhs, nrhs, prhs, JBIDXISTR | JBIDXDROP);
}

MEX_FUNCTION(ensureNumberIndex) (int nlhs,
                                 mxArray *plhs[],
                                 int nrhs,
                                 const mxArray *prhs[]) {
  SetIndexOperation(nlhs, plhs, nrhs, prhs, JBIDXNUM);
}

MEX_FUNCTION(rebuildNumberIndex) (int nlhs,
                                  mxArray *plhs[],
                                  int nrhs,
                                  const mxArray *prhs[]) {
  SetIndexOperation(nlhs, plhs, nrhs, prhs, JBIDXNUM | JBIDXREBLD);
}

MEX_FUNCTION(dropNumberIndex) (int nlhs,
                               mxArray *plhs[],
                               int nrhs,
                               const mxArray *prhs[]) {
  SetIndexOperation(nlhs, plhs, nrhs, prhs, JBIDXNUM | JBIDXDROP);
}

MEX_FUNCTION(ensureArrayIndex) (int nlhs,
                                mxArray *plhs[],
                                int nrhs,
                                const mxArray *prhs[]) {
  SetIndexOperation(nlhs, plhs, nrhs, prhs, JBIDXARR);
}

MEX_FUNCTION(rebuildArrayIndex) (int nlhs,
                                 mxArray *plhs[],
                                 int nrhs,
                                 const mxArray *prhs[]) {
  SetIndexOperation(nlhs, plhs, nrhs, prhs, JBIDXARR | JBIDXREBLD);
}

MEX_FUNCTION(dropArrayIndex) (int nlhs,
                              mxArray *plhs[],
                              int nrhs,
                              const mxArray *prhs[]) {
  SetIndexOperation(nlhs, plhs, nrhs, prhs, JBIDXARR | JBIDXDROP);
}

MEX_FUNCTION(sync) (int nlhs,
                    mxArray *plhs[],
                    int nrhs,
                    const mxArray *prhs[]) {
  CheckInputArguments(0, 1, nrhs);
  CheckOutputArguments(0, 0, nlhs);
  Database* database;
  ParseDatabaseInput(nrhs, prhs, &database);
  if (!ejdbsyncdb(database->getMutable())) {
    ERROR(database->errorMessage());
  }
}

MEX_FUNCTION(begintx) (int nlhs,
                      mxArray *plhs[],
                      int nrhs,
                      const mxArray *prhs[]) {
  CheckInputArguments(0, 1, nrhs);
  CheckOutputArguments(0, 0, nlhs);
  Database* database;
  EJCOLL* collection;
  ParseCollectionInput(nrhs, prhs, &collection, &database);
  if (!ejdbtranbegin(collection)) {
    ERROR(database->errorMessage());
  }
}

MEX_FUNCTION(commitx) (int nlhs,
                      mxArray *plhs[],
                      int nrhs,
                      const mxArray *prhs[]) {
  CheckInputArguments(0, 1, nrhs);
  CheckOutputArguments(0, 0, nlhs);
  Database* database;
  EJCOLL* collection;
  ParseCollectionInput(nrhs, prhs, &collection, &database);
  if (!ejdbtrancommit(collection)) {
    ERROR(database->errorMessage());
  }
}

MEX_FUNCTION(abortx) (int nlhs,
                      mxArray *plhs[],
                      int nrhs,
                      const mxArray *prhs[]) {
  CheckInputArguments(0, 1, nrhs);
  CheckOutputArguments(0, 0, nlhs);
  Database* database;
  EJCOLL* collection;
  ParseCollectionInput(nrhs, prhs, &collection, &database);
  if (!ejdbtranabort(collection)) {
    ERROR(database->errorMessage());
  }
}

MEX_FUNCTION(isactivetx) (int nlhs,
                          mxArray *plhs[],
                          int nrhs,
                          const mxArray *prhs[]) {
  CheckInputArguments(0, 1, nrhs);
  CheckOutputArguments(0, 1, nlhs);
  Database* database;
  EJCOLL* collection;
  ParseCollectionInput(nrhs, prhs, &collection, &database);
  bool status = false;
  if (!ejdbtranstatus(collection, &status)) {
    ERROR(database->errorMessage());
  }
  plhs[0] = MxArray(status).getMutable();
}

MEX_FUNCTION(dbmeta) (int nlhs,
                      mxArray *plhs[],
                      int nrhs,
                      const mxArray *prhs[]) {
  CheckInputArguments(0, 1, nrhs);
  CheckOutputArguments(0, 1, nlhs);
  Database* database;
  ParseDatabaseInput(nrhs, prhs, &database);
  bson* value = ejdbmeta(database->getMutable());
  if (!value) {
    ERROR(database->errorMessage());
  }
  if (!ConvertBSONToMxArray(value, &plhs[0])) {
    ERROR(bson_first_errormsg(value));
  }
  bson_del(value);
}

} // namespace