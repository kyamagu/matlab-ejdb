function bson = encodeBSON(value, varargin)
%ENCODEBSON Serialize value in BSON format.
%
%    bson = ejdb.encodeBSON(value, ...)
%
% The function converts a matlab variable to BSON format.
%
% See also ejdb.decodeBSON
  bson = libejdbmex(mfilename, value, varargin{:});
end
