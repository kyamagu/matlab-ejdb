function num_updates = update(collection, query, varargin)
%UPDATE Convenient method to execute update queries.
%
%    num_updates = ejdb.update(collection, query)
%    num_updates = ejdb.update(collection, query, hints)
%    num_updates = ejdb.update(database_id, collection, query)
%    num_updates = ejdb.update(database_id, collection, query, hints)
%
% Returns:
%
%    Count of updated objects
%
%
%    `$set` and `$inc` operations are supported:
%   
%    `$set` Field set operation:
%         - {some fields for selection, '$set' : {'fpath1' : {obj}, ...,  'fpathN' : {obj}}}
%    `$inc` Increment operation. Only number types are supported.
%         - {some fields for selection, '$inc' : {'fpath1' : number, ...,  'fpathN' : {number}}
%
% See also ejdb
  num_updates = libejdbmex(mfilename, collection, query, varargin{:});
end
