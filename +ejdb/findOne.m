function result = findOne(collection, query, varargin)
%FINDONE Same as find() but retrieves only one matching JSON object.
%
%    result = ejdb.findOne(collection, query)
%    result = ejdb.findOne(collection, query, hints)
%    result = ejdb.findOne(database_id, collection, query)
%    result = ejdb.findOne(database_id, collection, query, hints)
%
% See also ejdb
  result = libejdbmex(mfilename, collection, query, varargin{:});
end
