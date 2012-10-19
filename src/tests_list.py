#!/usr/bin/env python
from glob import iglob
from filecmp import cmp
import os

test_directory = os.path.join('tests', 'c_tests')

def change_ext(file, ext):
    return os.path.splitext(file)[0] + ext

for file in iglob(os.path.join(test_directory, '*.c')):
    os.system('./unilint {0} -s={1} -r={2}'.format(
        file, change_ext(file, '.ini'), change_ext(file, '.result')))
    if(cmp(change_ext(file, '.result'), change_ext(file, '.expected'))):
        print 'ok'
    else:
        print file + ' failed'