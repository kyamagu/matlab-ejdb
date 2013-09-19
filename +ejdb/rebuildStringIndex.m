function rebuildStringIndex(collection, index_path, varargin)
%REBUILDSTRINGINDEX Rebuild index of String type for JSON field path.
%
%    ejdb.rebuildStringIndex(collection, index_path)
%    ejdb.rebuildStringIndex(database, collection, index_path)
%
% See also ejdb
  libejdbmex(mfilename, collection, index_path, varargin{:});
end
