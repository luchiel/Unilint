#!/usr/bin/env python
from glob import iglob
from filecmp import cmp
import os

test_sets = [
    (os.path.join('tests', 'c_tests'), '*.c', 'C'),
    (os.path.join('tests', 'pascal_tests'), '*.pas', 'Pascal'),
]

def change_ext(file, ext):
    return os.path.splitext(file)[0] + ext

for path, mask, name in test_sets:
    passed = 0
    total = 0
    for file in iglob(os.path.join(path, mask)):
        os.system('./unilint {0} -s={1} -r={2}'.format(
            file, change_ext(file, '.ini'), change_ext(file, '.result')))
        total += 1
        if(cmp(change_ext(file, '.result'), change_ext(file, '.expected'))):
            passed += 1
        else:
            print file + ' failed'
    print '{0}: {1}/{2}\n'.format(name, passed, total)
