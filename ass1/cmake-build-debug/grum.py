#!/usr/bin/env python
from __future__ import print_function
import sys
import os

# Get correct path to files, based on platform
import platform
host = platform.node().split('.')[0]

if host=="moss":
   sys.path[0:0]=['/local/courses/csse2310/lib']
   TEST_LOCATION = '/home/users/uqjfenw1/public/2020_1/a1ptest'
else:
	sys.path[0:0] = [ 
	    '/home/joel/marks',
	]
import marks

COMPILE = "make"
 
 
class Assignment1(marks.TestCase):
  timeout = 12 
  @classmethod
  def setup_class(cls):
        # Store original location
        options = getattr(cls, '__marks_options__', {})
        cls.prog = os.path.join(options['working_dir'], './push2310')
        cls._compile_warnings = 0
        cls._compile_errors = 0
 
        # Create symlink to tests in working dir
        os.chdir(options['working_dir'])
        try:
            os.symlink(TEST_LOCATION, 'tests')
        except OSError:
            pass
        os.chdir(options['temp_dir'])

        # Modify test environment when running tests (excl. explain mode).
        if not options.get('explain', False):
            # Compile program
            os.chdir(options['working_dir'])
            p = marks.Process(COMPILE.split())
            os.chdir(options['temp_dir'])
 
            # Count warnings and errors
            warnings = 0
            errors = 0
            while True:
                line = p.readline_stderr()
                if line == '':
                    break
                if 'warning:' in line:
                    warnings += 1
                if 'error:' in line:
                    errors += 1
                print(line, end='')

            # Do not run tests if compilation failed.
            assert p.assert_exit_status(0)
 
            # Create symlink to tests within temp folder
            try:
                os.symlink(TEST_LOCATION, 'tests')
            except OSError:
                pass 

  @marks.marks('nlbadargs', category_marks=3)
  def test_usage1(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog])
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/usage1.out')
    self.assert_stderr_matches_file(p, 'tests/usage1.err')
    self.assert_exit_status(p, 1)

  @marks.marks('nlbadargs', category_marks=1)
  def test_usage2(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["",""])
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/usage2.out')
    self.assert_stderr_matches_file(p, 'tests/usage2.err')
    self.assert_exit_status(p, 1)

  @marks.marks('nlbadargs', category_marks=1)
  def test_usage3(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["","","",""])
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/usage3.out')
    self.assert_stderr_matches_file(p, 'tests/usage3.err')
    self.assert_exit_status(p, 1)

  @marks.marks('nlbadargs', category_marks=3)
  def test_bad_type1(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["xx", "", ""])
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/type.err')
    self.assert_exit_status(p, 2)

  @marks.marks('nlbadargs', category_marks=3)
  def test_bad_type2(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["0", "2", ""])
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/type.err')
    self.assert_exit_status(p, 2)

  @marks.marks('nlbadargs', category_marks=3)
  def test_bad_type3(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["2", "1", "ha"])
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/type.err')
    self.assert_exit_status(p, 2)

  @marks.marks('lbadargs', category_marks=3)
  def test_bad_load1(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["H", "1", "board0"])
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/nofile.err')
    self.assert_exit_status(p, 3)

  @marks.marks('lbadargs', category_marks=3)
  def test_bad_file1(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["0", "0", "tests/bmap1"])
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/board.err')
    self.assert_exit_status(p, 4)

  @marks.marks('lbadargs', category_marks=3)
  def test_bad_file2(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["0", "0", "tests/bmap2"])
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/board.err')
    self.assert_exit_status(p, 4)

  @marks.marks('lbadargs', category_marks=3)
  def test_bad_file3(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["0", "0", "tests/full1"])
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/empty')
    self.assert_stderr_matches_file(p, 'tests/full.err')
    self.assert_exit_status(p, 6)

  @marks.marks('loaddisplay', category_marks=4)
  def test_load1(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["H", "H", "tests/board1"])
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/load1.out')
    self.assert_stderr_matches_file(p, 'tests/eof.err')
    self.assert_exit_status(p, 5)

  @marks.marks('loaddisplay', category_marks=4)
  def test_load2(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["H", "H", "tests/board2"])
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/load2.out')
    self.assert_stderr_matches_file(p, 'tests/eof.err')
    self.assert_exit_status(p, 5)

  @marks.marks('t0move1', category_marks=2)
  def test_t0_m1_1(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["H", "0", "tests/board2"], "tests/empty")
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/t0m11.out')
    self.assert_stderr_matches_file(p, 'tests/eof.err')
    self.assert_exit_status(p, 5)

  @marks.marks('t0move1', category_marks=2)
  def test_t0_m1_2(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["H", "0", "tests/board1"], "tests/empty")
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/t0m13.out')
    self.assert_stderr_matches_file(p, 'tests/eof.err')
    self.assert_exit_status(p, 5)

  @marks.marks('t1move1', category_marks=2)
  def test_t1_m1_1(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["H", "0", "tests/board2"], "tests/empty")
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/t1m11.out')
    self.assert_stderr_matches_file(p, 'tests/eof.err')
    self.assert_exit_status(p, 5)

  @marks.marks('t1move1', category_marks=2)
  def test_t1_m1_2(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["H", "0", "tests/board1"], "tests/empty")
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/t1m13.out')
    self.assert_stderr_matches_file(p, 'tests/eof.err')
    self.assert_exit_status(p, 5)

# need to do file contents check and possibly ignore output ?
  @marks.marks('save', category_marks=3)
  def test_save1(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["H", "H", "tests/board7"], "tests/s1.in")
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/s1.out')
    self.assert_stderr_matches_file(p, 'tests/eof.err')
    self.assert_exit_status(p, 5)

  @marks.marks('save', category_marks=3)
  def test_save2(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["H", "H", "tests/board4"], "tests/s2.in")
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/s2.out')
    self.assert_stderr_matches_file(p, 'tests/sfeof.err')
    self.assert_exit_status(p, 5)
    
  @marks.marks('save', category_marks=3)
  def test_save3(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["H", "H", "tests/board7"], "tests/s5.in")
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/s5.out')
    self.assert_stderr_matches_file(p, 'tests/eof.err')
    self.assert_files_equal('tsave.5', 'tests/tsave.5')
    self.assert_exit_status(p, 5)
        
  @marks.marks('lastgo', category_marks=4)
  def test_lastgo1(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["0", "0", "tests/board7.ne"])
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/lg1.out')
    self.assert_stderr_matches_file(p, 'tests/empty')
    self.assert_exit_status(p, 0)
    
  @marks.marks('lastgo', category_marks=4)
  def test_lastgo2(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["H", "H", "tests/board2"], "tests/lg2.in")
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/lg2.out')
    self.assert_stderr_matches_file(p, 'tests/empty')
    self.assert_exit_status(p, 0)
    
  @marks.marks('partial0H', category_marks=4)
  def test_partial0H1(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["0", "H", "tests/board1"], "tests/0H1.in")
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/0H1.out')
    self.assert_stderr_matches_file(p, 'tests/eof.err')
    self.assert_exit_status(p, 5)
    
  @marks.marks('partial0H', category_marks=4)
  def test_partial0H2(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["0", "H", "tests/board1"], "tests/0H2.in")
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/0H2.out')
    self.assert_stderr_matches_file(p, 'tests/eof.err')
    self.assert_exit_status(p, 5)
    
  @marks.marks('partial1H', category_marks=4)
  def test_partial1H1(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["1", "H", "tests/board4"], "tests/1H1.in")
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/1H1.out')
    self.assert_stderr_matches_file(p, 'tests/eof.err')
    self.assert_exit_status(p, 5)
    
  @marks.marks('partial1H', category_marks=4)
  def test_partial1H2(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["1", "H", "tests/board4"], "tests/1H2.in")
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/1H2.out')
    self.assert_stderr_matches_file(p, 'tests/eof.err')
    self.assert_exit_status(p, 5)
        
  @marks.marks('complete', category_marks=5)
  def test_complete1(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["1", "1", "tests/board4"])
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/complete1.out')
    self.assert_stderr_matches_file(p, 'tests/empty')
    self.assert_exit_status(p, 0)
    
  @marks.marks('complete', category_marks=5)
  def test_complete2(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["1", "1", "tests/board1"])
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/complete2.out')
    self.assert_stderr_matches_file(p, 'tests/empty')
    self.assert_exit_status(p, 0)
    
  @marks.marks('complete', category_marks=5)
  def test_complete3(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["0", "0", "tests/board4"])
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/complete3.out')
    self.assert_stderr_matches_file(p, 'tests/empty')
    self.assert_exit_status(p, 0)
    
  @marks.marks('complete', category_marks=5)
  def test_complete4(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["0", "0", "tests/board6"])
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/complete4.out')
    self.assert_stderr_matches_file(p, 'tests/empty')
    self.assert_exit_status(p, 0)
        
  @marks.marks('complete', category_marks=5)
  def test_complete5(self):
    """ """
    #1 empty empty usage.err - - 
    p = self.process([self.prog]+["0", "0", "tests/board1"])
    p.finish_input()
    self.assert_stdout_matches_file(p, 'tests/complete10.out')
    self.assert_stderr_matches_file(p, 'tests/empty')
    self.assert_exit_status(p, 0)

if __name__ == '__main__':
    marks.main()
