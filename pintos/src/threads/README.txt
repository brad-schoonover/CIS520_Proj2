In order to add the alarm-mega test the following files needed to be changed:

tests/threads/tests.c
tests/threads/tests.h
tests/threads/Make.tests
tests/threads/Rubric.alarm
tests/threads/alarm_wait.c

tests.c - Includes the functions used to run the tests, needed to add test_alarm_mega to the list of possible tests

tests.h - Header file for tests.c

Make.tests - Makefile responsible for compiling all of the tests 

Rubric.alarm - specifies the functionality of the given tests

alarm_wait.c - Performs the delays for each alarm
