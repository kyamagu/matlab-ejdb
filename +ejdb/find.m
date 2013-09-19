function results = find(collection, query, varargin)
%FIND Execute query on collection.
%
%    results = ejdb.find(collection, query)
%    results = ejdb.find(collection, query, hints)
%    results = ejdb.find(database, collection, query)
%    results = ejdb.find(database, collection, query, hints)
%
% Sample:
%
%    % Fetch all elements from collection
%    ejdb.find('mycoll', {});
%    % Query document with 'foo==bar' condition, include in resulting doc 'foo' and '_id' fields only
%    ejdb.find('mycoll', {'foo', 'bar'}, {'$fields', {'foo', 1, '_id', 1}});
%
% Parameters:
%
%    - `database` Database handle. The last opened database is used when
%      skpped.
%    - `collection` Collection name.
%    - `query` Main query object.
%    - `hints` Query hints. See explanations below.
%
% Returns:
%
%    An array of records.
%
% EJDB queries inspired by MongoDB (mongodb.org) and follows same philosophy.
%
% - Supported queries:
%    - Simple matching of String OR Number OR Array value:
%        -   {'fpath', 'val', ...}
%    - $not Negate operation.
%        -   {'fpath', {'$not', val}} //Field not equal to val
%        -   {'fpath', {'$not', {'$begin', prefix}}} //Field not begins with val
%    - $begin String starts with prefix
%        -   {'fpath', {'$begin', prefix}}
%    - $gt, $gte (>, >=) and $lt, $lte for number types:
%        -   {'fpath', {'$gt', number}, ...}
%    - $bt Between for number types:
%        -   {'fpath', {'$bt', [num1, num2]}}
%    - $in String OR Number OR Array val matches to value in specified array:
%        -   {'fpath', {'$in', {val1, val2, val3}}}
%    - $nin - Not IN
%    - $strand String tokens OR String array val matches all tokens in specified array:
%        -   {'fpath', {'$strand', {val1, val2, val3}}}
%    - $stror String tokens OR String array val matches any token in specified array:
%        -   {'fpath', {'$stror', {val1, val2, val3}}}
%    - $exists Field existence matching:
%        -   {'fpath', {'$exists', true|false}}
%    - $icase Case insensitive string matching:
%        -    {'fpath', {'$icase', 'val1'}} //icase matching
%        icase matching with '$in' operation:
%        -    {'name', {'$icase', {'$in', ['tHéâtre - театр', 'heLLo WorlD']}}}
%        For case insensitive matching you can create special type of string index.
%    - $elemMatch The $elemMatch operator matches more than one component within an array element.
%        -    { 'array', { '$elemMatch', { value1, 1, value2, { $gt: 1 } } } }
%        Restriction: only one $elemMatch allowed in context of one array field.
%    - $and, $or joining:
%        -   {..., '$and', {subq1, subq2, ...} }
%        -   {..., '$or' , {subq1, subq2, ...} }
%       Example: {'z', 33, '$and', { {'$or', {{'a', 1}, {'b', 2}}}, {'$or', {{'c', 5}, {'d', 7}}} } }
% 
% - Queries can be used to update records:
% 
%    - $set Field set operation.
%        - {.., '$set', {'field1', val1, 'fieldN', valN}}
%    - $upsert Atomic upsert. If matching records are found it will be '$set' operation,
%              otherwise new record will be inserted with fields specified by argment object.
%        - {.., '$upsert', {'field1', val1, 'fieldN', valN}}
%    - $inc Increment operation. Only number types are supported.
%        - {.., '$inc', {'field1', number, ...,  'field1', number}
%    - $dropall In-place record removal operation.
%        - {.., '$dropall', true}
%    - $addToSet Atomically adds value to the array only if its not in the array already.
%                If containing array is missing it will be created.
%        - {.., '$addToSet', {'fpath', val1, 'fpathN', valN, ...}}
%    - $addToSetAll Batch version if $addToSet
%        - {.., '$addToSetAll', {'fpath', {array of values to add}, ...}}
%    - $pull Atomically removes all occurrences of value from field, if field is an array.
%        - {.., '$pull', {'fpath', val1, 'fpathN', valN, ...}}
%    - $pullAll Batch version of $pull
%        - {.., '$pullAll', {'fpath', {array of values to remove}, ...}}
% 
% - Collection joins supported in the following form:
%    - {..., '$do', {'fpath', {'$join', 'collectionname'}} }
%     Where 'fpath' value points to object's OIDs from 'collectionname'. Its value
%     can be OID, string representation of OID or array of this pointers.
% 
% .. NOTE:: Due to the limitation in Matlab syntax, an array {val1, val2, val3, ...} cannot
%        be distinguished from key-value pairs {key1, value1, key2, value2, ...}. To force
%        an array type, make an odd-sized cell array or just prepend [] in the beginning of
%        an array. i.e., {[], val1, val2, ...}.
%
% .. NOTE:: Object id is represented as `_id` property in the database, while in Matlab
%        the field is renamed to `id`.
%
% .. NOTE:: It is better to execute update queries with `$onlycount=true` hint flag
%        or use the special `update()` method to avoid unnecessarily data fetching.
% 
% .. NOTE:: Negate operations: $not and $nin not using indexes
%          so they can be slow in comparison to other matching operations.
% 
% .. NOTE:: Only one index can be used in search query operation.
% 
% QUERY HINTS (specified by `hints` argument):
%    - $max Maximum number in the result set
%    - $skip Number of skipped results in the result set
%    - $orderby Sorting order of query fields.
%    - $onlycount true|false If `true` only count of matching records will be returned
%                            without placing records in result set.
%    - $fields Set subset of fetched fields.
%        If field presented in $orderby clause it will be forced to include in resulting records.
%        Example:
%        hints:    {
%            '$orderby', [ //ORDER BY field1 ASC, field2 DESC
%                ('field1', 1),
%                ('field2', -1)
%            ],
%            '$fields', { //SELECT ONLY {_id, field1, field2}
%                'field1', 1,
%                'field2', 1
%            }
%        }
%
% See also ejdb
  results = libejdbmex(mfilename, collection, query, varargin{:});
end
