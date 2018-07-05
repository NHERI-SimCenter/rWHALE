# written: jbl

import json
import sys

divider = '#' * 80
log_output = []

from WorkflowUtils import *

def main(tests_to_run, list_of_tests_filename):
    # the whole test process is wrapped within a 'try' block.
    # a number of exceptions (files missing, explicit application failures, etc.) are
    # handled explicitly to aid the user.
    # But unhandled exceptions case the workflow to stop with an error, handled in the
    # exception block way at the bottom of this main() function

    try:

        workflow_log(divider)
        workflow_log('Start of test')
        workflow_log(divider)
        workflow_log('running test(s): %s' % ' '.join(tests_to_run))

        #
        # first we parse the applications registry to load all possible applications
        #  - for each application type we place in a dictionary key being name, value containing path to executable
        #

        #print tests_to_run
        #print list_of_tests

        missing_test = False
        for test in tests_to_run:
            if not test in list_of_tests:
                print "test '%s' not in list of available tests." % test
                missing_test = True
            else:
                # check a couple other things to make sure this will run
                if not 'command' in list_of_tests[test]:
                    print "no 'command' configured for '%s' so there would be nothing to do. check %s" % (test, list_of_tests_filename)
                    exit(1)

        if missing_test:
            print('needs to be one of the following:')
            print '   ',
            for test in list_of_tests:
                print test,
            exit(1)


        for test in tests_to_run:
            workflow_log('running %s' % test)
            log_output.append([('starting %s ...' % test), '... at %s' % strftime('%Y-%m-%d-%H-%M-%S-utc', gmtime())])
            #workflow_log('working directory %s' % list_of_tests[test]['directory'])
            #workflow_log('%s' % list_of_tests[test]['command'])
            command, result, returncode = runApplication([list_of_tests[test]['command']])
            log_output.append([command, result, returncode])


    except WorkFlowInputError as e:
        workflow_log('workflow error: %s' % e.value)
        workflow_log(divider)
        exit(1)

    # unhandled exceptions are handled here
    except:
        raise
        workflow_log('unhandled exception... exiting')
        exit(1)


if __name__ == '__main__':

    # read list of possible tests (from file list_of_tests.json)
    try:
        list_of_tests_filename = 'list_of_tests.json'
        with open(list_of_tests_filename, 'r') as data_file:
            list_of_tests = json.load(data_file)
            # convert all relative paths to full paths
            relative2fullpath(list_of_tests)
    except IOError:
        raise WorkFlowInputError('Could not open list of tests configuration file: %s' % list_of_tests_filename)

    if len(sys.argv) == 1:
        print
        print('Syntax')
        print('    python %s <list of tests to run>' % sys.argv[0])
        print
        print('where <list of tests to run> is one or more of the following:')
        print
        print '   ',
        for test in list_of_tests:
            print test,
        print
        print
        exit(1)

    # here is where the heavy lifting is done
    main(sys.argv[1:], 'list_of_tests.json')

    # write results to log file
    workflow_log_file = 'workflow-log-%s.txt' % (strftime('%Y-%m-%d-%H-%M-%S-utc', gmtime()))
    log_filehandle = open(workflow_log_file, 'wb')

    print >>log_filehandle, divider
    print >>log_filehandle, 'Start of Log'
    print >>log_filehandle, divider
    print >>log_filehandle, workflow_log_file
    # nb: log_output is a global variable, defined at the top of this script.
    for result in log_output:
        print >>log_filehandle, divider
        print >>log_filehandle, '%s' % result[0]
        print >>log_filehandle, divider
        print >>log_filehandle, '%s' % result[1]

    print >>log_filehandle, divider
    print >>log_filehandle, 'End of Log'
    print >>log_filehandle, divider

    workflow_log('test output may be found in: %s' % workflow_log_file)
    workflow_log('End of run.')
