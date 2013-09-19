function value = load(collection, id, varargin)
%LOAD Loads documents identified by id from a collection.
%
%    value = ejdb.load(collection, id)
%    value = ejdb.load(database_id, collection, id)
%
% Sample:
%
%    ejdb.load('mycoll', '511c72ae7922641d00000000');
%
% Parameters:
%
%    - `database_id` Database handle. The last opened database is used when
%      skpped.
%    - `collection` Collection name.
%    - `id` Document object id (`_id` property).
%
% Returns:
%
%    A document identified by `id`.
%
% See also ejdb
  value = libejdbmex(mfilename, collection, id, varargin{:});
end
