function status = isopen(varargin)
%ISOPEN Check if a database handle is valid.
%
%    status = ejdb.isopen
%    status = ejdb.isopen(database)
%
% See also ejdb
  status = libejdbmex(mfilename, varargin{:});
end
