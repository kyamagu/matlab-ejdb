function rebuildIStringIndex(collection, index_path, varargin)
%REBUILDISTRINGINDEX Rebuild case insensitive String index for JSON field path.
%
%    ejdb.rebuildIStringIndex(collection, index_path)
%    ejdb.rebuildIStringIndex(database, collection, index_path)
%
% See also ejdb
  libejdbmex(mfilename, collection, index_path, varargin{:});
end
