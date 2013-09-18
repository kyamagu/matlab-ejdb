function close(varargin)
%CLOSE Close the database.
%
%    ejdb.close
%    ejdb.close(id)
%
% The function closes the database session of the specified id. When the id is
% omitted, the default session is closed.
%
% See also ejdb.open
  libejdbmex(mfilename, varargin{:});
end
