#!/usr/bin/env vpython3
'''Wrapper script to appease the PRESUBMIT.py vpython dark spirits.'''

import os
import subprocess
import unittest

class WrapperUnittest(unittest.TestCase):
  def testCallRealTestsWithPy3(self):
    tests = os.path.join(os.path.dirname(__file__), 'generate_gn_unittest.py')
    try:
      test_out = subprocess.check_output(['python3', tests], text=True)
    except subprocess.CalledProcessError as e:
      self.fail('Unit tests failed, see above for expectation errors')
    self.assertEqual(test_out, '')

if __name__ == '__main__':
  unittest.main()