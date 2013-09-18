function ensureCollection(collection_name, varargin)
%ENSURECOLLECTION Open or creating a new collection.
%
%    ejdb.ensureCollection(collection_name, ...)
%    ejdb.ensureCollection(id, collection_name, ...)
%
% The function opens an existing collection or creates one if not existing.
%
% See also ejdb.open ejdb.close
  libejdbmex(mfilename, collection_name, varargin{:});
end
