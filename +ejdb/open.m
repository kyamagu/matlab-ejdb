function database = open(filename, varargin)
%OPEN Open a database.
%
%    database = ejdb.open(filename, option1, option2,...)
%
% Samples:
%
%    database = ejdb.open('foo')
%    database = ejdb.open('foo', 'READER')
%    database = ejdb.open('foo', 'WRITER', 'CREAT', 'TRUNC')
%
% Parameters:
%    - `filename` Database file path.
%    - `optionN` List of options. By default, 'WRITER' and 'CREAT' is given.
%      If any option is specified, default values are all `false`.
%        READER - Open as a reader.
%        WRITER - Open as a writer.
%        CREAT - Create db if it not exists.
%        TRUNC - Truncate db.
%        NOLCK - Open without locking.
%        LCKNB - Lock without blocking.
%        TSYNC - Synchronize every transaction.
%
% Returns:
%
%    Database handle.
%
% See also ejdb
  database = libejdbmex(mfilename, filename, varargin{:});
end
