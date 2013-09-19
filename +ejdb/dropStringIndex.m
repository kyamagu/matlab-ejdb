function dropStringIndex(collection, index_path, varargin)
%DROPSTRINGINDEX Drop index of String type for JSON field path.
%
%    ejdb.dropStringIndex(collection, index_path)
%    ejdb.dropStringIndex(database_id, collection, index_path)
%
% See also ejdb
  libejdbmex(mfilename, collection, index_path, varargin{:});
end
