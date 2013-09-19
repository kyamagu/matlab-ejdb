function optimizeIndexes(collection, index_path, varargin)
%OPTIMIZEINDEXES Optimize indexes of all types for JSON field path.
%
%    ejdb.optimizeIndexes(collection, index_path)
%    ejdb.optimizeIndexes(database, collection, index_path)
%
% See also ejdb
  libejdbmex(mfilename, collection, index_path, varargin{:});
end
