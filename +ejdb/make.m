function make(varargin)
%MAKE Build a driver mex file.
%
%    ejdb.make(['optionName', optionValue,] [compiler_flags])
%
% ejdb.make builds a mex file for the ejdb driver. The make script
% accepts db path option and compiler flags for the mex command.
% See below for the supported build options.
%
% Options:
%
%    Option name       Value
%    ----------------  -------------------------------------------------
%    --libtcejdb_path  path to libejdb.a
%
% Example:
%
% Specifying additional paths.
%
% >> ejdb.make('-I/opt/local/include', '-L/opt/local/lib');
%
% Specifying library files.
%
% >> ejdb.make('--libtcejdb_path', '/opt/local/lib/libtcejdb.a', ...
%             '-I/opt/local/include');
%
% See also mex
  package_dir = fileparts(mfilename('fullpath'));
  [config, compiler_flags] = parse_options(varargin{:});
  cmd = sprintf([...
      'mex -largeArrayDims%s -outdir %s -output libejdbmex %s%s -lz'...
      ],...
      find_source_files(fullfile(fileparts(package_dir), 'src')),...
      fullfile(package_dir, 'private'),...
      config.db_path,...
      compiler_flags...
      );
  disp(cmd);
  eval(cmd);
end

function [config, compiler_flags] = parse_options(varargin)
%PARSE_OPTIONS Parse build options.
  config.db_path = '-ltcejdb';
  mark_for_delete = false(size(varargin));
  for i = 1:2:numel(varargin)
    if strcmp(varargin{i}, '--libtcejdb_path')
      config.db_path = varargin{i+1};
      mark_for_delete(i:i+1) = true;
    end
  end
  compiler_flags = sprintf(' %s', varargin{~mark_for_delete});
  if isunix
    compiler_flags = sprintf(' CFLAGS="$CFLAGS -fPIC -std=c99"%s', compiler_flags);
  end
end

function files = find_source_files(root_dir)
%SOURCE_FILES List of source files in a string.
  files = dir(root_dir);
  srcs = files(cellfun(@(x)~isempty(x), ...
               regexp({files.name},'\S+\.(c)|(cc)|(cpp)|(C)')));
  srcs = cellfun(@(x)fullfile(root_dir, x), {srcs.name},...
                 'UniformOutput', false);
  subdirs = files([files.isdir] & cellfun(@(x)x(1)~='.',{files.name}));
  subdir_srcs = cellfun(@(x)find_source_files(fullfile(root_dir,x)),...
                        {subdirs.name}, 'UniformOutput', false);
  files = [sprintf(' %s', srcs{:}), subdir_srcs{:}];
end
