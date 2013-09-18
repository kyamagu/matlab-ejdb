function dropCollection(collection_name, varargin)
%DROPCOLLECTION Open or creating a new collection.
%
%    ejdb.dropCollection(collection_name, ...)
%    ejdb.dropCollection(id, collection_name, ...)
%
% The function destroys an existing collection.
%
% See also ejdb.open ejdb.close ejdb.ensureCollection
  libejdbmex(mfilename, collection_name, varargin{:});
end
