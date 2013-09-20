Embedded JSON database Matlab binding
=====================================

EJDB - Embedded JSON Database engine

http://ejdb.org

Build
-----

_Required tools/system libraries:_

 * g++/gcc
 * Matlab
 * EJDB C library libtcejdb-dev

Launch matlab, add a path to `matlab-ejdb`, and use `ejdb.make` command.

```Matlab
addpath /path/to/matlab-ejdb;
ejdb.make
```

If you have a local ejdb package, you can link statically. Assuming the package
is located at `/path/to/ejdb`:

```Matlab
!make -C /path/to/ejdb
ejdb.make('--libtcejdb_path', '/path/to/ejdb/tcejdb/libtcejdb.a', ...
          '-I/path/to/ejdb/tcejdb');
```


One snippet intro
-----------------

```Matlab
addpath /path/to/matlab-ejdb;

db_id = ejdb.open('zoo');

parrot1 = struct(...
  'name', 'Grenny', ...
  'type', 'African Grey', ...
  'male', true, ...
  'age', 1, ...
  'birthdate', now, ...
  'likes', {{'green color', 'night', 'toys'}}, ...
  'extra1', []...
  );
parrot2 = struct(...
  'name', 'Bounty', ...
  'type', 'Cockatoo', ...
  'male', false, ...
  'age', 15, ...
  'birthdate', now, ...
  'likes', {{'sugar cane', 'toys'}}, ...
  'extra1', []...
  );

object_ids = ejdb.save('parrots', parrot1, parrot2);
disp(['Grenny OID: ', object_ids{1}]);
disp(['Boundy OID: ', object_ids{2}]);

results = ejdb.find('parrots', {'likes', 'toys'});

disp(['Found ', num2str(numel(results)), ' parrots']);

for i = 1:numel(results)
  disp([results(i).name, ' likes toys!']);
end

ejdb.close(db_id);
```

Note
----

 * Datetime is not properly handled yet.
 * Sparse array is not supported.
