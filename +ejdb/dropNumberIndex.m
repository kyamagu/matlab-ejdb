function dropNumberIndex(collection, index_path, varargin)
%DROPNUMBERINDEX Drop index of Number type for JSON field path.
%
%    ejdb.dropNumberIndex(collection, index_path)
%    ejdb.dropNumberIndex(database, collection, index_path)
%
% See also ejdb
  libejdbmex(mfilename, collection, index_path, varargin{:});
end
