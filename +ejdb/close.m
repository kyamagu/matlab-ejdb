function close(varargin)
%CLOSE Close the database.
%
%    ejdb.close
%    ejdb.close(database)
%
% See also ejdb
  libejdbmex(mfilename, varargin{:});
end
