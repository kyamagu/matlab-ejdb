/// Matlab ejdb API.
///
/// Kota Yamaguchi 2013

#include "bsonmex.h"
#include "mex/arguments.h"
#include "mex/function.h"
#include "mex/mxarray.h"

using mex::CheckInputArguments;
using mex::CheckOutputArguments;
using mex::VariableInputArguments;

namespace {

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