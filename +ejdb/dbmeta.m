function metadata = dbmeta(varargin)
%DBMETA Retrieve metainfo object describing database structure.
%
%    metadata = ejdb.dbmeta
%    metadata = ejdb.dbmeta(database)
%
% See also ejdb
  metadata = libejdbmex(mfilename, varargin{:});
end
