function value = flags(varargin)
%FLAGS Return various flags for other functions.
%
% Supported flags:
%
%    - 'JSONEXPORT'      Export data as JSON files instead exporting into BSONs.
%    - 'IMPORTUPDATE'    Update existing collection entries with imported ones.
%                        Existing collections will not be recreated.
%                        For existing collections options will not be imported.
%    - 'IMPORTREPLACE'   Recreate existing collections and replace
%                        all their data with imported entries.
%                        Collections options will be imported.
%
% See also ejdb
  value = uint32(0);
  index = 1;
  while index <= nargin
    switch lower(varargin{index})
      case 'jsonexport'
        value = bitor(value, uint32(1));
      case 'importupdate'
        value = bitor(value, uint32(2));
      case 'importreplace'
        value = bitor(value, uint32(4));
    end
    index = index + 1;
  end
end