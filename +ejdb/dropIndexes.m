function dropIndexes(collection, index_path, varargin)
%DROPINDEXES Drops indexes of all types for JSON field path.
%
%    ejdb.dropIndexes(collection, index_path)
%    ejdb.dropIndexes(database, collection, index_path)
%
% See also ejdb
  libejdbmex(mfilename, collection, index_path, varargin{:});
end
