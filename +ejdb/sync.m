function sync(varargin)
%SYNC Synchronize entire EJDB database and all of its collections with storage.
%
%    ejdb.sync
%    ejdb.sync(database)
%
% See also ejdb
  libejdbmex(mfilename, varargin{:});
end
