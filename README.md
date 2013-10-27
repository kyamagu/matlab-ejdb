Embedded JSON database Matlab binding
=====================================

EJDB - Embedded JSON Database engine

http://ejdb.org

Build
-----

_Required tools/system libraries:_

 * g++/gcc
 * Matlab

Launch matlab, add a path to `matlab-ejdb`, and use `ejdb.make` command.

```Matlab
addpath /path/to/matlab-ejdb;
ejdb.make
```

By default, this will download the latest EJDB package from GitHub and create
a statically linked binary. If you prefer to link dynamically, specify
`--download-ejdb` to false and pass any additional flags. For example,

```Matlab
ejdb.make('--download-ejdb', false, ...
          '-I/opt/local/include', ...
          '-L/opt/local/lib');
```

Runtime requirement
-------------------

In most cases, you'll need to force Matlab to preload the system libstdc++ due
to the incompatibility between the system and Matlab's internal libstdc++
version. Use `LD_PRELOAD` variable to do so. For example, in Ubuntu 12.04 LTS
64-bit,

    LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libstdc++.so.6:/lib/x86_64-linux-gnu/libgcc_s.so.1 matlab

The required path depends on the platform. You can check which path to specify
by comparing the output of `ldd` tool in the UNIX shell and in Matlab.

From the UNIX shell,

    $ ldd +ejdb/private/libejdbmex.mex*

From Matlab shell,

    >> !ldd +ejdb/private/libejdbmex.mex*

And find a dependent library differing in Matlab. You must append that library
in the `LD_PRELOAD` path.

In OS X, `LD_PRELOAD` equivalent is `DYLD_INSERT_LIBRARIES`. Use `otool -L`
command instead of `ldd`.

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
  'birthdate', {bson.date(now)}, ...
  'likes', {{'green color', 'night', 'toys'}}, ...
  'extra1', []...
  );
parrot2 = struct(...
  'name', 'Bounty', ...
  'type', 'Cockatoo', ...
  'male', false, ...
  'age', 15, ...
  'birthdate', {bson.date(now)}, ...
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

 * Sparse array is not supported.
