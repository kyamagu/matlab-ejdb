function dropCollection(collection, varargin)
%DROPCOLLECTION Removes database collection.
%
%    ejdb.dropCollection(collection, ...)
%    ejdb.dropCollection(database, collection, ...)
%    ejdb.dropCollection(..., collection, option1, ...)
%
% Samples:
%
%    ejdb.dropCollection('mycoll')
%    ejdb.dropCollection('mycoll', 'UNLINK')
%
% Parameters:
%
%    - `database` Database handle.
%    - `collection` Collection name.
%    - `optionN` Following options can be specified.
%
%         UNLINK - unlink the underlying file.
%
% See also ejdb
  libejdbmex(mfilename, collection, varargin{:});
end
