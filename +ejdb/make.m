function make(varargin)
%MAKE Build a driver mex file.
%
%    ejdb.make(['optionName', optionValue,] [compiler_flags])
%
% ejdb.make builds a mex file for the ejdb driver. The make script
% accepts a few options as well as compiler flags for the mex command.
% See below for the supported build options.
%
% Options:
%
%    Option name       Description
%    ----------------  -------------------------------------------------
%    --download-ejdb   logical flag of whether to download ejdb source.
%    --libtcejdb-path  path to libejdb.a.
%
% Example:
%
% Default to automatically download the EJDB package from GitHub and
% statically link to the EJDB binary.
%
% >> ejdb.make();
%
% Dynamic linking to the system EJDB library.
%
% >> ejdb.make('--download-ejdb', false);
%
% Static linking to the user-built EJDB binary.
%
% >> ejdb.make('--download-ejdb', false, ...
%              '--libtcejdb-path', '/opt/local/lib/libtcejdb.a', ...
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
  config = parse_options(varargin{:});
  if config.download_ejdb
    config = download_ejdb(root_dir, config);
  end

  for i = 1:numel(targets)
    [outdir, output] = fileparts(targets{i});
    cmd = sprintf('mex -largeArrayDims%s %s -outdir %s -output %s %s%s -lz', ...
                  sprintf(' %s', source_files{:}), ...
                  api_files{i}, ...
                  outdir, ...
                  output, ...
                  config.libtcejdb_path, ...
                  config.compiler_flags ...
                  );
    disp(cmd);
    eval(cmd);
  end
end

function config = parse_options(varargin)
%PARSE_OPTIONS Parse build options.
  config.download_ejdb = true;
  config.libtcejdb_path = '-ltcejdb';
  mark_for_delete = false(size(varargin));
  for i = 1:2:numel(varargin)
    if strcmp(varargin{i}, '--libtcejdb-path')
      config.libtcejdb_path = varargin{i+1};
      mark_for_delete(i:i+1) = true;
    elseif strcmp(varargin{i}, '--download-ejdb')
      config.download_ejdb = logical(varargin{i+1});
      mark_for_delete(i:i+1) = true;
    end
  end
  compiler_flags = sprintf(' %s', varargin{~mark_for_delete});
  if isunix
    compiler_flags = sprintf(' CFLAGS="$CFLAGS -fPIC -std=c99"%s', ...
                             compiler_flags);
  end
  config.compiler_flags = compiler_flags;
end

function config = download_ejdb(root_dir, config)
%DOWNLOAD_EJDB Download the latest EJDB package from GitHub.
  github_url = 'https://github.com/Softmotions/ejdb/archive/master.zip';
  zip_file = fullfile(root_dir, 'ejdb-master.zip');
  disp(['Downloading ', github_url]);
  urlwrite(github_url, zip_file);
  disp(['Extracting ' zip_file]);
  unzip(zip_file);
  delete(zip_file);
  cmd = sprintf('make -C %s', fullfile(root_dir, 'ejdb-master'));
  disp(cmd);
  system(cmd);
  config.libtcejdb_path = fullfile(root_dir, ...
                                   'ejdb-master', ...
                                   'tcejdb', ...
                                   'libtcejdb.a');
  config.compiler_flags = sprintf(' -I%s %s', ...
                                  fullfile(root_dir, ...  
                                           'ejdb-master', ...
                                           'tcejdb'), ...
                                  config.compiler_flags);
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
