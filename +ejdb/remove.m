function remove(collection, id, varargin)
%REMOVE Removes from collection the document identified by id.
%
%    ejdb.remove(collection, id)
%    ejdb.remove(database_id, collection, id)
%
% See also ejdb
  libejdbmex(mfilename, collection, id, varargin{:});
end
