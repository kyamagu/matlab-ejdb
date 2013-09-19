function ensureIStringIndex(collection, index_path, varargin)
%ENSUREISTRINGINDEX Ensure case insensitive String index for JSON field path.
%
%    ejdb.ensureIStringIndex(collection, index_path)
%    ejdb.ensureIStringIndex(database, collection, index_path)
%
% See also ejdb
  libejdbmex(mfilename, collection, index_path, varargin{:});
end
