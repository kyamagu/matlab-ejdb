function dropCollection(collection, varargin)
%DROPCOLLECTION Removes database collection.
%
%    ejdb.dropCollection(collection, ...)
%    ejdb.dropCollection(id, collection, ...)
%
% See also ejdb
  libejdbmex(mfilename, collection, varargin{:});
end
