#!/usr/bin/env python
from glob import iglob
from filecmp import cmp
import os

class TestSet:
    path = ''
    mask = ''
    name = ''
    def __init__(self, path, mask, name):
        self.path = path
        self.mask = mask
        self.name = name

test_sets = [
    TestSet(os.path.join('tests', 'c_tests'), '*.c', 'C'),
    TestSet(os.path.join('tests', 'pascal_tests'), '*.pas', 'Pascal'),
]

def change_ext(file, ext):
    return os.path.splitext(file)[0] + ext

for test_set in test_sets:
    passed = 0
    total = 0
    for file in iglob(os.path.join(test_set.path, test_set.mask)):
        os.system('./unilint {0} -s={1} -r={2}'.format(
            file, change_ext(file, '.ini'), change_ext(file, '.result')))
        total += 1
        if(cmp(change_ext(file, '.result'), change_ext(file, '.expected'))):
            passed += 1
        else:
            print file + ' failed'
    print '{0}: {1}/{2}\n'.format(test_set.name, passed, total)
