/// Matlab EJDB helper.
///
/// Kota Yamaguchi 2013

#ifndef __EJDBMEX_H__
#define __EJDBMEX_H__

#include "mex/session.h"
#include <string>

using namespace std;

/// Alias for the mex error function.
#define ERROR(...) mexErrMsgIdAndTxt("ejdb:error", __VA_ARGS__)

/// Alias for the mex warning function.
#define WARNING(...) mexWarnMsgIdAndTxt("ejdb:warning", __VA_ARGS__)

/// Forward declarations.
struct EJDB;

namespace ejdbmex {

/// Database handle.
class Database {
public:
  /// Empty constructor.
  Database();
  /// Destructor.
  virtual ~Database();
  /// Get a raw pointer.
  EJDB* getMutable() { return database_; }
  /// Get a collection. Throws an error when invalid name is given.
  EJCOLL* getMutableCollection(const char* collection_name);
  /// Open a new connection.
  bool open(const char* filename, int mode);
  /// Check if open.
  bool isOpen();
  /// Close a connection.
  bool close();
  /// Last error message.
  const char* errorMessage();
  /// Save a BSON object.
  /// @param collection_name name of the collection to save.
  /// @param value bson value to be stored.
  /// @param object_id assigned object id.
  /// @return true if success.
  bool save(const char* collection_name, bson* value, string* object_id);
  /// Load a BSON object.
  /// @param collection_name name of the collection to save.
  /// @param object_id object id.
  /// @param value bson value to be loaded. Must be freed with bson_del().
  /// @return true if success.
  bool load(EJCOLL* collection, const char* object_id, bson** value);
  /// Remove a BSON object.
  /// @param collection_name name of the collection to save.
  /// @param object_id object id to be removed.
  /// @return true if success.
  bool remove(EJCOLL* collection, const char* object_id);
  /// Query objects.
  /// @param collection Collection in which to query.
  /// @param query bson query object.
  /// @param hints bson query hint object.
  /// @param results query results.
  /// @param flags query search mode: JBQRYCOUNT or JBQRYFINDONE.
  /// @return true if success.
  bool find(EJCOLL* collection,
            bson* query,
            bson* hints,
            mxArray** results,
            int flags);

private:
  /// Database pointer.
  EJDB* database_;
};

} // namespace ejdbmex

namespace mex {

// Template instanciations.
extern template class Session<ejdbmex::Database>;

} // namespace mex

#endif // __EJDBMEX_H__