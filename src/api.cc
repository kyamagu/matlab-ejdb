/// Matlab ejdb API.
///
/// Kota Yamaguchi 2013

#include "bsonmex.h"
#include <cstring>
#include "ejdb.h"
#include "ejdbmex.h"
#include <memory>
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
  int index = 0;
  int database_id = (nrhs > 0 && MxArray(prhs[index]).isNumeric()) ?
                    MxArray(prhs[index++]).toInt() : 0;
  Database* database = Session<Database>::get(database_id);
  if (!database)
    ERROR("No open database found.");
  string collection_name = MxArray(prhs[index++]).toString();
  VariableInputArguments options;
  options.set("LARGE",         false);
  options.set("COMPRESSED",    false);
  options.set("RECORDS",       128 * 1024.0);
  options.set("CACHEDRECORDS", 0);
  options.update(prhs + index, prhs + nrhs);
  EJCOLLOPTS collection_options = {
    options["LARGE"].toBool(),
    options["COMPRESSED"].toBool(),
    options["RECORDS"].toDouble(),
    options["CACHEDRECORDS"].toInt()
  };
  if (!database->createCollection(collection_name.c_str(),
                                  &collection_options)) {
    ERROR("Failed to create a collection: %s", database->errorMessage());
  }
}

MEX_FUNCTION(dropCollection) (int nlhs,
                              mxArray *plhs[],
                              int nrhs,
                              const mxArray *prhs[]) {
  CheckInputArguments(1, 1024, nrhs);
  CheckOutputArguments(0, 0, nlhs);
  int index = 0;
  int database_id = (nrhs > 0 && MxArray(prhs[index]).isNumeric()) ?
                    MxArray(prhs[index++]).toInt() : 0;
  Database* database = Session<Database>::get(database_id);
  if (!database)
    ERROR("No open database found.");
  string collection_name = MxArray(prhs[index++]).toString();
  VariableInputArguments options;
  options.set("UNLINK", true);
  options.update(prhs + index, prhs + nrhs);
  if (!database->removeCollection(collection_name.c_str(), 
                                  options["UNLINK"].toBool())) {
    ERROR("Failed to remove a collection: %s", database->errorMessage());
  }
}

MEX_FUNCTION(save) (int nlhs,
                    mxArray *plhs[],
                    int nrhs,
                    const mxArray *prhs[]) {
  CheckInputArguments(2, 1024, nrhs);
  CheckOutputArguments(0, 1, nlhs);
  int index = 0;
  int database_id = (nrhs > 0 && MxArray(prhs[index]).isNumeric()) ?
                    MxArray(prhs[index++]).toInt() : 0;
  Database* database = Session<Database>::get(database_id);
  if (!database)
    ERROR("No open database found.");
  string collection_name = MxArray(prhs[index++]).toString();
  int num_objects = nrhs - index;
  plhs[0] = mxCreateCellMatrix(1, num_objects);
  for (int i = 0; i < num_objects; ++i) {
    bson value;
    if (!ConvertMxArrayToBSON(prhs[index++], 0, &value))
      ERROR(bson_first_errormsg(&value));
    string object_id;
    if (!database->save(collection_name.c_str(), &value, &object_id))
      ERROR("Failed to save: %s", database->errorMessage());
    bson_destroy(&value);
    mxSetCell(plhs[0], i, MxArray(object_id).getMutable());
  }
  if (num_objects == 1) {
    mxArray* value = mxDuplicateArray(mxGetCell(plhs[0], 0));
    mxDestroyArray(plhs[0]);
    plhs[0] = value;
  }
}

MEX_FUNCTION(find) (int nlhs,
                    mxArray *plhs[],
                    int nrhs,
                    const mxArray *prhs[]) {
  CheckInputArguments(2, 4, nrhs);
  CheckOutputArguments(0, 1, nlhs);
  int index = 0;
  int database_id = (nrhs > 0 && MxArray(prhs[index]).isNumeric()) ?
                    MxArray(prhs[index++]).toInt() : 0;
  Database* database = Session<Database>::get(database_id);
  if (!database)
    ERROR("No open database found.");
  string collection_name = MxArray(prhs[index++]).toString();
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
  if (!database->find(collection_name.c_str(),
                      &query,
                      (with_hints) ? &hints : NULL,
                      &plhs[0]))
    ERROR("Failed to find: %s", database->errorMessage());
  bson_destroy(&query);
  if (with_hints)
    bson_destroy(&hints);
}

MEX_FUNCTION(encodeBSON) (int nlhs,
                          mxArray *plhs[],
                          int nrhs,
                          const mxArray *prhs[]) {
  CheckInputArguments(1, 2, nrhs);
  CheckOutputArguments(0, 1, nlhs);
  VariableInputArguments options;
  options.set("QUERY", false);
  options.update(prhs + 1, prhs + nrhs);
  int flags = (options["QUERY"].toBool()) ? BSON_FLAG_QUERY_MODE : 0;
  EncodeBSON(prhs[0], flags, &plhs[0]);
}

MEX_FUNCTION(decodeBSON) (int nlhs,
                          mxArray *plhs[],
                          int nrhs,
                          const mxArray *prhs[]) {
  CheckInputArguments(1, 1, nrhs);
  CheckOutputArguments(0, 1, nlhs);
  DecodeBSON(prhs[0], &plhs[0]);
}

} // namespace
