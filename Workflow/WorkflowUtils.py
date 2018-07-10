import os
import subprocess
from time import gmtime, strftime

class WorkFlowInputError(Exception):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return repr(self.value)


def workflow_log(msg):
    # ISO-8601 format, e.g. 2018-06-16T20:24:04Z
    print '%s %s' % (strftime('%Y-%m-%dT%H:%M:%SZ', gmtime()), msg)


# function to return result of invoking an application
def runApplication(application_plus_args):
    try:
        result = subprocess.check_output(' '.join(application_plus_args), stderr=subprocess.STDOUT, shell=True)
        # for line in result.split('\n'):
        # pass
        # print(line)
        returncode = 0
    except subprocess.CalledProcessError as e:
        result = e.output
        returncode = e.returncode

    if returncode != 0:
        workflow_log('NON-ZERO RETURN CODE: %s' % returncode)
    return ' '.join(application_plus_args), result, returncode


def add_full_path(possible_filename):
    if not isinstance(possible_filename, basestring):
        return possible_filename
    if (os.path.exists(possible_filename)):
        if os.path.isdir(possible_filename):
            return os.path.abspath(possible_filename) + '/'
        else:
            return os.path.abspath(possible_filename)
    else:
        return possible_filename


def recursive_iter(obj):
    if isinstance(obj, dict):
        for k, v in obj.items():
            if isinstance(v, basestring):
                obj[k] = add_full_path(v)
            else:
                recursive_iter(v)
    elif any(isinstance(obj, t) for t in (list, tuple)):
        for idx, item in enumerate(obj):
            if isinstance(item, basestring):
                obj[idx] = add_full_path(item)
            else:
                recursive_iter(item)


def relative2fullpath(json_object):
    recursive_iter(json_object)
