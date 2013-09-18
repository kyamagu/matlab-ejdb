function value = decodeBSON(bson, varargin)
%DECODEBSON Deserialize value from BSON format.
%
%    value = ejdb.decodeBSON(bson, ...)
%
% The function converts a bson format to a matlab value.
%
% See also ejdb.encodeBSON
  value = libejdbmex(mfilename, bson, varargin{:});
end
