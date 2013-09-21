classdef date
%DATE Date type in BSON format.

  properties (SetAccess = private)
    number % Matlab date number.
  end

  methods
    function this = date(varargin)
      %DATE Create a new date object.
      %
      %    bson.date
      %    bson.date('2007-01-01')
      %
      % See also datenum
      if nargin == 1 && isstruct(varargin{1})
        this = repmat(this, size(varargin{1}));
        [this.number] = deal(varargin{1}.number);
      elseif nargin > 0
        this.number = datenum(varargin{:});
      else
        this.number = now;
      end
    end
    
    function value = string(this, varargin)
      %STRING Convert to a string representation.
      %
      %    value.string('yyyy-mm-dd')
      %
      % See also datestr
      value = datestr([this.number], varargin{:});
    end
    
    function disp(this)
      %DISP Display function.
      disp(char(this));
    end
    
    function value = double(this)
      %DOUBLE Convert to double.
      value = double(reshape([this.number], size(this)));
    end
    
    function value = char(this)
      %CHAR Convert to string.
      value = datestr([this.number]);
    end
  end

end