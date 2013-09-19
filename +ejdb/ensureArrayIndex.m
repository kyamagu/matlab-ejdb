function ensureArrayIndex(collection, index_path, varargin)
%ENSUREARRAYINDEX Ensure index presence of Array type for JSON field path.
%
%    ejdb.ensureArrayIndex(collection, index_path)
%    ejdb.ensureArrayIndex(database, collection, index_path)
%
% See also ejdb
  libejdbmex(mfilename, collection, index_path, varargin{:});
end
