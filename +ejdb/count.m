function num_objects = count(collection, query, varargin)
%COUNT Counts matched documents.
%
%    num_objects = ejdb.count(collection, query)
%    num_objects = ejdb.count(collection, query, hints)
%    num_objects = ejdb.count(database_id, collection, query)
%    num_objects = ejdb.count(database_id, collection, query, hints)
%
% Returns:
%
%    Count of matched objects.
%
% See also ejdb
  num_objects = libejdbmex(mfilename, collection, query, varargin{:});
end
