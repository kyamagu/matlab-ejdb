function bson = encodeBSON(value, varargin)
%ENCODEBSON Serialize value in BSON format.
%
%    bson = ejdb.encodeBSON(value, ...)
%
% Parameters:
%
%    - `value` A value to be encoded as a BSON binary.
%
% Returns:
%
%    A BSON binary.
%
% See also ejdb
  bson = libejdbmex(mfilename, value, varargin{:});
end
