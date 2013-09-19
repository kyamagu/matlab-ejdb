function rebuildArrayIndex(collection, index_path, varargin)
%REBUILDARRAYINDEX Rebuild index of Array type for JSON field path.
%
%    ejdb.rebuildArrayIndex(collection, index_path)
%    ejdb.rebuildArrayIndex(database, collection, index_path)
%
% See also ejdb
  libejdbmex(mfilename, collection, index_path, varargin{:});
end
