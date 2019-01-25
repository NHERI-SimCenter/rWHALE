# import functions for Python 2.X support
from __future__ import division, print_function
import sys
if sys.version.startswith('2'): 
    range=xrange

import sys
import os
import subprocess

inputArgs = sys.argv

# set filenames
bimName = sys.argv[2]
samName = sys.argv[4]
evtName = sys.argv[6]
edpName = sys.argv[8]
simName = sys.argv[10]

scriptDir = os.path.dirname(os.path.realpath(__file__))

#If requesting random variables run getUncertainty
#Otherwise, Run Opensees 
if "-getRV" in inputArgs:
    getUncertaintyCommand = '"{}/getUncertainty" {} {} {} {}'.format(scriptDir, bimName, samName, evtName, simName)
    subprocess.Popen(args=getUncertaintyCommand, shell=True).wait()
else:
    #Run preprocessor
    preprocessorCommand = u'"{}/mainPreprocessor" {} {} {} {} example.tcl'.format(scriptDir, bimName, samName, evtName, edpName)
    subprocess.Popen(preprocessorCommand, shell=True).wait()
    
    #Run OpenSees
    subprocess.Popen("OpenSees example.tcl", shell=True).wait()

    #Run postprocessor
    postprocessorCommand = u'"{}/mainPostprocessor" {} {} {} {}'.format(scriptDir, bimName, samName, evtName, edpName)
    subprocess.Popen(postprocessorCommand, shell=True).wait()