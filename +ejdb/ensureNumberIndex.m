function ensureNumberIndex(collection, index_path, varargin)
%ENSURENUMBERINDEX Ensure index presence of Number type for JSON field path.
%
%    ejdb.ensureNumberIndex(collection, index_path)
%    ejdb.ensureNumberIndex(database_id, collection, index_path)
%
% See also ejdb
  libejdbmex(mfilename, collection, index_path, varargin{:});
end
