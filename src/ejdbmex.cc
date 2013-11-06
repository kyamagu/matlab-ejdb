/// Matlab EJDB helper implementation.
/// 
/// Kota Yamaguchi 2013

#include "bsonmex.h"
#include "ejdb.h"
#include "ejdbmex.h"
#include <mex.h>

namespace ejdbmex {

Database::Database() : database_(NULL) {}

Database::~Database() {}

EJCOLL* Database::getMutableCollection(const char* collection_name) {
  EJCOLL* collection = ejdbgetcoll(database_, collection_name);
  if (!collection) {
    ERROR("%s: %s", ejdberrmsg(JBEINVALIDCOLNAME), collection_name);
  }
  return collection;
}

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
  EJCOLLOPTS collection_options = {false, false, 65535, 0};
  EJCOLL* collection = ejdbcreatecoll(database_,
                                      collection_name,
                                      &collection_options);
  if (!collection)
    return false;
  // TODO: merge option.
  if (!ejdbsavebson(collection, value, &oid))
    return false;
  bson_oid_to_string(&oid, buffer);
  *object_id = buffer;
  return true;
}

bool Database::load(EJCOLL* collection, const char* object_id, bson** value) {
  bson_oid_t oid;
  bson_oid_from_string(&oid, object_id);
  *value = ejdbloadbson(collection, &oid);
  return *value != NULL;
}

bool Database::remove(EJCOLL* collection, const char* object_id) {
  bson_oid_t oid;
  bson_oid_from_string(&oid, object_id);
  return ejdbrmbson(collection, &oid);
}

bool Database::find(EJCOLL* collection,
                    bson* query,
                    bson* hints,
                    mxArray** results,
                    int flags) {
  EJQ* ejdb_query = ejdbcreatequery(database_, query, NULL, 0, hints);
  if (!ejdb_query)
    return false;
  uint32_t num_results;
  EJQRESULT result_list = ejdbqryexecute(collection,
                                         ejdb_query,
                                         &num_results,
                                         flags,
                                         NULL);
  if (flags == JBQRYCOUNT) {
    ejdbquerydel(ejdb_query);
    *results = mxCreateDoubleScalar(num_results);
  }
  else {
    if (!result_list) {
      ejdbquerydel(ejdb_query);
      return false;
    }
    num_results = (flags == JBQRYFINDONE && num_results > 1) ? 1 : num_results;
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
      mxArray* value = ConvertBSONIteratorToMxArray(&it);
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
  }
  return true;
}

} // namespace ejdbmex

namespace mex {

template class Session<ejdbmex::Database>;

} // namespace mex