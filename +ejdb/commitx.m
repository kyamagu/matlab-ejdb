function commitx(collection, varargin)
%COMMITX Commit collection transaction.
%
%    ejdb.commitx(collection)
%    ejdb.commitx(database, collection)
%
% See also ejdb
  libejdbmex(mfilename, collection, varargin{:});
end
