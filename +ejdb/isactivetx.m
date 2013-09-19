function status = isactivetx(collection, varargin)
%ISACTIVETX Is collection transaction active.
%
%    status = ejdb.isactivetx(collection)
%    status = ejdb.isactivetx(database, collection)
%
% See also ejdb
  status = libejdbmex(mfilename, collection, varargin{:});
end
