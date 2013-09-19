function dropIStringIndex(collection, index_path, varargin)
%DROPISTRINGINDEX Drop case insensitive String index for JSON field path.
%
%    ejdb.dropIStringIndex(collection, index_path)
%    ejdb.dropIStringIndex(database_id, collection, index_path)
%
% See also ejdb
  libejdbmex(mfilename, collection, index_path, varargin{:});
end
