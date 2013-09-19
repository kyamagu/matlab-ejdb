function ensureCollection(collection, varargin)
%ENSURECOLLECTION Automatically creates new collection if it does not exists.
%
%    ejdb.ensureCollection(collection, ...)
%    ejdb.ensureCollection(id, collection, ...)
%    ejdb.ensureCollection(..., collection, optionName, optionValue, ...)
%
% Parameters:
%
%    - `collection` Name of the collection.
%    - `optionN` Following options can be specified. Collection options are
%      applied only for newly created collection. For existing collections,
%      options take no effect.
%
%        CACHEDRECORDS - Max number of cached records in shared memory segment.
%                        Default: 0
%        RECORDS - Estimated number of records in this collection.
%                  Default: 65535.
%        LARGE - Specifies that the size of the database can be larger than
%                2GB. Default: false
%        COMPRESSED - If true collection records will be compressed with
%                     DEFLATE compression. Default: false.
% 
% See also ejdb
  libejdbmex(mfilename, collection, varargin{:});
end
