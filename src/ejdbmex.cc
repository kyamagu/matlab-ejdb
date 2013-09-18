#include "bsonmex.h"
#include "ejdb.h"
#include "ejdbmex.h"

namespace ejdbmex {

Database::Database() : database_(NULL) {}

Database::~Database() {}

bool Database::open(const char* filename, int mode) {
  if (isOpen()) {
    WARNING("Closing existing connection.");
    if (!close())
      return false;
  }
  database_ = ejdbnew();
  if (!database_)
    return false;
  return ejdbopen(database_, filename, mode);
}

bool Database::isOpen() {
  return (database_) ? ejdbisopen(database_) : false;
}

bool Database::close() {
  if (isOpen()) {
    if (!ejdbclose(database_)) {
      return false;
    }
    ejdbdel(database_);
  }
  return true;
}

const char* Database::errorMessage() {
  return ejdberrmsg(ejdbecode(database_));
}

bool Database::save(const char* collection_name,
                    bson* value,
                    string* object_id) {
  bson_oid_t oid;
  char buffer[1024];
  EJCOLLOPTS collection_options = {false, false, 128 * 1024.0, 0};
  EJCOLL* collection = ejdbcreatecoll(database_,
                                      collection_name,
                                      &collection_options);
  if (!collection)
    return false;
  if (!ejdbsavebson(collection, value, &oid))
    return false;
  bson_oid_to_string(&oid, buffer);
  *object_id = buffer;
  return true;
}

bool Database::createCollection(const char* collection_name,
                                EJCOLLOPTS* collection_options) {
  EJCOLL* collection = ejdbcreatecoll(database_,
                                      collection_name,
                                      collection_options);
  return collection != NULL;
}

bool Database::removeCollection(const char* collection_name, 
                                bool unlinkfile) {
  return ejdbrmcoll(database_, collection_name, unlinkfile);
}

bool Database::find(const char* collection_name,
                    bson* query,
                    bson* hints,
                    mxArray** results) {
  EJCOLL* collection = ejdbgetcoll(database_, collection_name);
  if (!collection) {
    ERROR("%s: %s", ejdberrmsg(JBEINVALIDCOLNAME), collection_name);
    return false;
  }
  EJQ* ejdb_query = ejdbcreatequery(database_, query, NULL, 0, hints);
  if (!ejdb_query)
    return false;
  uint32_t num_results;
  EJQRESULT result_list = ejdbqryexecute(collection, ejdb_query, &num_results, 0, NULL);
  if (!result_list) {
    ejdbquerydel(ejdb_query);
    return false;
  }
  *results = mxCreateCellMatrix(1, num_results);
  for (int i = 0; i < num_results; ++i) {
    int size = 0;
    const void* result_data = ejdbqresultbsondata(result_list, i, &size);
    if (!result_data) {
      ejdbqresultdispose(result_list);
      ejdbquerydel(ejdb_query);
      return false;
    }
    bson_iterator it;
    bson_iterator_from_buffer(&it, (const char*)result_data);
    mxArray* value = ConvertBSONArrayToMxArray(&it);
    if (!value) {
      ejdbqresultdispose(result_list);
      ejdbquerydel(ejdb_query);
      return false;
    }
    mxSetCell(*results, i, value);
  }
  ejdbqresultdispose(result_list);
  ejdbquerydel(ejdb_query);
  TryMergeCellToNDArray(results);
  return true;
}

} // namespace ejdbmex

namespace mex {

template class Session<ejdbmex::Database>;

} // namespace mex