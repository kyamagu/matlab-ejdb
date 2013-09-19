function begintx(collection, varargin)
%BEGINTX Begin collection transaction.
%
%    ejdb.begintx(collection)
%    ejdb.begintx(database, collection)
%
% See also ejdb
  libejdbmex(mfilename, collection, varargin{:});
end
