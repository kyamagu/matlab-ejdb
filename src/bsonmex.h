/** mxArray BSON encoder interface.
 *
 * Kota Yamaguchi 2013
 */

#ifndef __BSONMEX_H__
#define __BSONMEX_H__

#include <bson.h>
#include <matrix.h>

/** Convert mxArray* to bson.
 * @param input mxArray to convert to bson.
 * @param flags options to change the behavior.
 *              BSON_FLAG_QUERY_MODE : Construct BSON as a query.
 * @param output bson object to be created. Caller is responsible for calling
 *               bson_destroy() after use. 
 * @return true if success.
 */
EXTERN_C bool ConvertMxArrayToBSON(const mxArray* input, int flags, bson* output);
/** Convert bson to mxArray*.
 * @param input bson object to convert to mxArray.
 * @param output mxArray to be created.
 * @return true if success.
 */
EXTERN_C bool ConvertBSONToMxArray(const bson* input, mxArray** output);
/** Convert bson iterator to mxArray*.
 * @param input bson object to convert to mxArray.
 * @param output mxArray to be created.
 * @return true if success.
 */
EXTERN_C mxArray* ConvertBSONArrayToMxArray(bson_iterator* it);
/** Try to merge cell array to N-D array.
 * @param array mxArray to be merged into N-D.
 */
EXTERN_C void TryMergeCellToNDArray(mxArray** array);
/** Encode mxArray* in BSON format and wrap in mxArray.
 * @param input mxArray to convert to bson.
 * @param flags options to change the behavior.
 *              BSON_FLAG_QUERY_MODE : Construct BSON as a query.
 * @param output mxArray object to be created.
 * @return true if success.
 */
EXTERN_C void EncodeBSON(const mxArray* input, int flags, mxArray** output);
/** Decode BSON-encoded mxArray.
 * @param input mxArray to decode.
 * @param output mxArray to be created.
 * @return true if success.
 */
EXTERN_C void DecodeBSON(const mxArray* input, mxArray** output);

#endif /* __BSONMEX_H__ */