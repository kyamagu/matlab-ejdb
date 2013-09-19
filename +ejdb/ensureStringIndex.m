function ensureStringIndex(collection, index_path, varargin)
%ENSURESTRINGINDEX Ensure index presence of String type for JSON field path.
%
%    ejdb.ensureStringIndex(collection, index_path)
%    ejdb.ensureStringIndex(database, collection, index_path)
%
% See also ejdb
  libejdbmex(mfilename, collection, index_path, varargin{:});
end
