function make(varargin)
%MAKE Build a driver mex file.
%
%    ejdb.make(['optionName', optionValue,] [compiler_flags])
%
% ejdb.make builds a mex file for the ejdb driver. The make script
% accepts a path option and compiler flags for the mex command.
% See below for the supported build options.
%
% Options:
%
%    Option name       Value
%    ----------------  -------------------------------------------------
%    --libtcejdb-path  path to libejdb.a
%
% Example:
%
% Specifying additional paths.
%
% >> ejdb.make('-I/opt/local/include', '-L/opt/local/lib');
%
% Specifying library files.
%
% >> ejdb.make('--libtcejdb-path', '/opt/local/lib/libtcejdb.a', ...
%              '-I/opt/local/include');
%
% See also mex
  root_dir = fileparts(fileparts(mfilename('fullpath')));
  api_files = { ...
    fullfile(root_dir, 'src', 'libejdbmex.cc'), ...
    fullfile(root_dir, 'src', 'libbsonmex.cc') ...
    };
  targets = { ...
    fullfile(root_dir, '+ejdb', 'private', 'libejdbmex'), ...
    fullfile(root_dir, '+bson', 'private', 'libbsonmex') ...
    };
  source_files = setdiff(find_source_files(fullfile(root_dir, 'src')), ...
                         api_files);
  [config, compiler_flags] = parse_options(varargin{:});
  for i = 1:numel(targets)
    [outdir, output] = fileparts(targets{i});
    cmd = sprintf('mex -largeArrayDims%s %s -outdir %s -output %s %s%s -lz', ...
                  sprintf(' %s', source_files{:}), ...
                  api_files{i}, ...
                  outdir, ...
                  output, ...
                  config.libtcejdb_path, ...
                  compiler_flags ...
                  );
    disp(cmd);
    eval(cmd);
  end
end

function [config, compiler_flags] = parse_options(varargin)
%PARSE_OPTIONS Parse build options.
  config.libtcejdb_path = '-ltcejdb';
  mark_for_delete = false(size(varargin));
  for i = 1:2:numel(varargin)
    if strcmp(varargin{i}, '--libtcejdb-path')
      config.libtcejdb_path = varargin{i+1};
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
  if nargin < 2, excludes = {}; end
  files = dir(root_dir);
  srcs = files(cellfun(@(x)~isempty(x), ...
               regexp({files.name},'\S+\.(c)|(cc)|(cpp)|(C)')));
  srcs = cellfun(@(x)fullfile(root_dir, x), {srcs.name},...
                 'UniformOutput', false);
  subdirs = files([files.isdir] & cellfun(@(x)x(1)~='.',{files.name}));
  subdir_srcs = cellfun(@(x)find_source_files(fullfile(root_dir,x)),...
                        {subdirs.name}, 'UniformOutput', false);
  files = [srcs, subdir_srcs{:}];
end
