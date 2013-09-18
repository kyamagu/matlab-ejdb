function id = open(filename, varargin)
%OPEN Open a database.
%
%    id = ejdb.open(filename, ...)
%
% The function opens a database session for the given db file.
%
% ## Options
%
% _READER_ [false]
% Open as a reader.
%
% _WRITER_ [true]
% Open as a writer.
%
% _CREAT_ [true]
% Create db if it not exists.
%
% _TRUNC_ [false]
% Truncate db.
%
% _NOLCK_ [false]
% Open without locking.
%
% _LCKNB_ [false]
% Lock without blocking.
%
% _TSYNC_ [false]
% Synchronize every transaction.
%
% See also ejdb.close
  id = libejdbmex(mfilename, filename, varargin{:});
end
