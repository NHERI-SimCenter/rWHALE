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
if "--getRV" in inputArgs:
    getUncertaintyCommand = '"{}/OpenSeesPreprocessor" {} {} {} {}'.format(scriptDir, bimName, samName, evtName, simName)
    subprocess.Popen(getUncertaintyCommand, shell=True).wait()
else:
    #Run preprocessor
    preprocessorCommand = '"{}/OpenSeesPreprocessor" {} {} {} {} {} example.tcl'.format(scriptDir, bimName, samName, evtName, edpName, simName)
    subprocess.Popen(preprocessorCommand, shell=True).wait()
    
    #Run OpenSees
    subprocess.Popen("OpenSees example.tcl", shell=True).wait()

    #Run postprocessor
    postprocessorCommand = '"{}/OpenSeesPostprocessor" {} {} {} {}'.format(scriptDir, bimName, samName, evtName, edpName)
    subprocess.Popen(postprocessorCommand, shell=True).wait()
