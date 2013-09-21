function response = command(command_value, varargin)
%COMMAND Executes ejdb database command.
%
%    response = ejdb.command(command_value)
%    response = ejdb.command(database, command_value)
%
% Samples:
%
%    response = ejdb.command({'export', {'path', '/path/to/exported_zoo', ...
%                                        'cnames', 'parrots', ...
%                                        'mode', ejdb.flags('JSONEXPORT')} ...
%                            });
%
% Parameters:
%
%    - `cmd`  Command object dictionary
%
% Returns:
%
%    Command response object dictionary
%
% Supported commands:
%
% 1) Exports database collections data. See ejdbexport() method.
%
%  "export" : {
%        "path" : string,                    //Exports database collections data
%        "cnames" : [string array]|null,     //List of collection names to export
%        "mode" : int|null                   //Values: null|ejdb.flags('JSONEXPORT')
%  }
%
%  Command response:
%     {
%        "log" : string,        //Diagnostic log about executing this command
%        "error" : string|null, //ejdb error message
%        "errorCode" : int|0,   //ejdb error code
%     }
%
% 2) Imports previously exported collections data into ejdb.
%
%  "import" : {
%        "path" : string                     //The directory path in which data resides
%        "cnames" : [string array]|null,     //List of collection names to import
%        "mode" : int|null                //Values: null|ejdb.flags('IMPORTUPDATE')|ejdb.flags('IMPORTREPLACE')
%   }
%
%   Command response:
%     {
%        "log" : string,        //Diagnostic log about executing this command
%        "error" : string|null, //ejdb error message
%        "errorCode" : int|0,   //ejdb error code
%     }
%
% See also ejdb
  response = libejdbmex(mfilename, command_value, varargin{:});
end
