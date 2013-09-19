/// Helper library.

#ifndef __EJDBMEX_H__
#define __EJDBMEX_H__

#include <mex.h>
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
  bool load(const char* collection_name, const char* object_id, bson** value);
  /// Query objects.
  /// @param collection_name name of the collection in which to query.
  /// @param query bson query object.
  /// @param hints bson query hint object.
  /// @param results query results.
  /// @return true if success.
  bool find(const char* collection_name,
            bson* query,
            bson* hints,
            mxArray** results);
  /// Create a column handle.
  bool createCollection(const char* collection_name,
                        EJCOLLOPTS* collection_options);
  /// Remove a column handle.
  bool removeCollection(const char* collection_name,
                        bool unlinkfile);

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