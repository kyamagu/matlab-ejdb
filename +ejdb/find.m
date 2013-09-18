function results = find(collection, query, varargin)
%FIND Find documents in a collection.
%
%    results = ejdb.find(collection, query)
%    results = ejdb.find(collection, query, hints)
%    results = ejdb.find(database_id, collection, query)
%    results = ejdb.find(database_id, collection, query, hints)
%
% The function finds records in a collection. Optionally, the function can
% take a query hint.
%
% See also ejdb.open ejdb.close ejdb.save
  results = libejdbmex(mfilename, collection, query, varargin{:});
end
