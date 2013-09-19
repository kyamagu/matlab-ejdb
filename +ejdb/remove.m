function remove(collection, id, varargin)
%REMOVE Remove an object from a collection.
%
%    ejdb.remove(collection, id, ...)
%    ejdb.remove(database_id, collection, id, ...)
%
% The function removes an object from a collection with a given id.
%
% See also ejdb.open ejdb.close
  libejdbmex(mfilename, collection, id, varargin{:});
end
