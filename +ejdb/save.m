function object_id = save(collection, value, varargin)
%SAVE Save a matlab object into a collection.
%
%    object_id = ejdb.save(collection, value, ...)
%    object_id = ejdb.save(database_id, collection, value, ...)
%    object_ids = ejdb.save(collection, value1, value2, ...)
%
% The function saves a matlab value to a collection.
%
% See also ejdb.open ejdb.close
  object_id = libejdbmex(mfilename, collection, value, varargin{:});
end
