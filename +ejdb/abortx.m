function abortx(collection, varargin)
%ABORTX Abort collection transaction.
%
%    ejdb.abortx(collection)
%    ejdb.abortx(database, collection)
%
% See also ejdb
  libejdbmex(mfilename, collection, varargin{:});
end
