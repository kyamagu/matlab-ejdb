function value = decodeBSON(bson, varargin)
%DECODEBSON Deserialize value from BSON format.
%
%    value = ejdb.decodeBSON(bson, ...)
%
% Parameters:
%
%    - `bson` BSON encoded binary.
%
% Returns:
%
%    Decoded Matlab value.
%
% See also ejdb
  value = libejdbmex(mfilename, bson, varargin{:});
end
