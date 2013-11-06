function testEJDB
%TESTEJDB
  addpath(fileparts(fileparts(mfilename('fullpath'))));
  test1;
end

function test1
%TEST1
  TESTDB_DIR = 'testdb';

  if ~exist(TESTDB_DIR, 'dir')
    mkdir(TESTDB_DIR);
  end

  cwd = cd(TESTDB_DIR);

  db_id = ejdb.open('zoo');

  ejdb.dropCollection(db_id, 'parrots');
  ejdb.dropCollection(db_id, 'cows');

  ejdb.ensureCollection('parrots');
  ejdb.ensureCollection('parrots', struct('LARGE', true, 'RECORDS', 200000));

  parrot1 = struct('name', 'Cacadoo', 'size', 12);
  ejdb.save('parrots', parrot1);

  parrot2 = struct('name', 'Mamadoo', ...
                   'size', 666, ...
                   'likes', {{ ...
                     'green color', ...
                     'night', ...
                     {'toys', 'joys'}, ...
                     parrot1 ...
                   }});
  ejdb.save('parrots', parrot2);

  ejdb.save('cows', struct('name', 'moo'));

  results = ejdb.find('parrots', {'name', 'Cacadoo'});
  for i = 1:numel(results)
    disp(results(i));
  end

  % Due to the grammatical ambiguity, $in operator must take an odd array.
  results = ejdb.find('parrots', {'name', {'$in', {'Mamadoo', 'Sauron', []}}});
  for i = 1:numel(results)
    disp(results(i));
  end

  disp(ejdb.find('cows', {}));

  results = ejdb.find('cows', {'name', 'moo'});
  if ~any(strcmp({results.name}, 'moo'))
    error('Error querying cows');
  end

  assert(ejdb.count('parrots', {}) == 2);
  assert(numel(ejdb.findOne('parrots', {})) == 1);

  ejdb.close(db_id);

  disp('CONGRATULATIONS!!! Test batch 1 has passed completely!');

  cd(cwd);
  rmdir(TESTDB_DIR, 's');
end