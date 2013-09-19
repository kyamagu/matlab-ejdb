function value = load(collection, id, varargin)
%LOAD Load an object from a collection.
%
%    value = ejdb.load(collection, id, ...)
%    value = ejdb.load(database_id, collection, id, ...)
%
% The function loads an object from a collection with a given id.
%
% See also ejdb.open ejdb.close
  value = libejdbmex(mfilename, collection, id, varargin{:});
end
