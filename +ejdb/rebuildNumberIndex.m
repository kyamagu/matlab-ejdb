function rebuildNumberIndex(collection, index_path, varargin)
%REBUILDNUMBERINDEX Rebuild index of Number type for JSON field path.
%
%    ejdb.rebuildNumberIndex(collection, index_path)
%    ejdb.rebuildNumberIndex(database_id, collection, index_path)
%
% See also ejdb
  libejdbmex(mfilename, collection, index_path, varargin{:});
end
