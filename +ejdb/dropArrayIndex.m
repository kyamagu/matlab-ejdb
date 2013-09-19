function dropArrayIndex(collection, index_path, varargin)
%DROPARRAYINDEX Drop index of Array type for JSON field path.
%
%    ejdb.dropArrayIndex(collection, index_path)
%    ejdb.dropArrayIndex(database_id, collection, index_path)
%
% See also ejdb
  libejdbmex(mfilename, collection, index_path, varargin{:});
end
