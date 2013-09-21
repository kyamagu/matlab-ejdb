/// Matlab bson API.
///
/// Kota Yamaguchi 2013

#include "bsonmex.h"
#include <mex.h>
#include "mex/arguments.h"
#include "mex/function.h"
#include <stdlib.h>
#include <string.h>

using mex::CheckInputArguments;
using mex::CheckOutputArguments;
using mex::VariableInputArguments;

namespace {

/** Encode mxArray* in BSON format and wrap in mxArray.
 * @param input mxArray to convert to bson.
 * @param flags options to change the behavior.
 *              BSON_FLAG_QUERY_MODE : Construct BSON as a query.
 * @param output mxArray object to be created.
 * @return true if success.
 */
void EncodeBSON(const mxArray* input, int flags, mxArray** output) {
  bson value;
  if (!ConvertMxArrayToBSON(input, flags, &value))
    mexErrMsgIdAndTxt("bsonmex:error", bson_first_errormsg(&value));
  int size = bson_size(&value);
  *output = mxCreateNumericMatrix(1, size, mxUINT8_CLASS, mxREAL);
  memcpy(mxGetData(*output), bson_data(&value), size);
  bson_destroy(&value);
}

/** Decode BSON-encoded mxArray.
 * @param input mxArray to decode.
 * @param output mxArray to be created.
 * @return true if success.
 */
void DecodeBSON(const mxArray* input, mxArray** output) {
  bson value;
  int size = mxGetNumberOfElements(input);
  char* data = (char*)bson_malloc(size * sizeof(char));
  memcpy(data, mxGetData(input), size * sizeof(char));
  if (bson_init_finished_data(&value, data) != BSON_OK) {
    bson_free(data);
    mexErrMsgIdAndTxt("bsonmex:error", bson_first_errormsg(&value));
  }
  if (!ConvertBSONToMxArray(&value, output))
    mexErrMsgIdAndTxt("bsonmex:error", bson_first_errormsg(&value));
  bson_destroy(&value);
}

MEX_FUNCTION(encode) (int nlhs,
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

MEX_FUNCTION(decode) (int nlhs,
                      mxArray *plhs[],
                      int nrhs,
                      const mxArray *prhs[]) {
  CheckInputArguments(1, 1, nrhs);
  CheckOutputArguments(0, 1, nlhs);
  DecodeBSON(prhs[0], &plhs[0]);
}

} // namespace