# import functions for Python 2.X support
from __future__ import division, print_function
import sys
if sys.version.startswith('2'): 
    range=xrange

import sys
import os
import platform
import shutil
import subprocess
from preprocessJSON import preProcessDakota
import platform
import stat
import argparse

#Input Arguments Specifications
uqArgParser = argparse.ArgumentParser()
uqArgParser.add_argument("-filenameBIM", required=True, help="Path to BIM file")
uqArgParser.add_argument("-filenameSAM", required=True, help="Path to SAM file")
uqArgParser.add_argument("-filenameEVENT", required=True, help="Path to EVENT file")
uqArgParser.add_argument("-filenameEDP", required=True, help="Path to EDP file")
uqArgParser.add_argument("-filenameSIM", required=True, help="Path to SIM file")
uqArgParser.add_argument("-filenameLOSS", required=True, help="Path to Damage & Loss file")
uqArgParser.add_argument("-driverFile", required=True, help="Path to UQ driver")
uqArgParser.add_argument("-samples", type=int, required=True, help="Number of samples")
uqArgParser.add_argument("-seed", type=int, default=1, help="The seed used for random number generation")
uqArgParser.add_argument("-keepSamples", help="A flag to specify whether to clean working directory for samples")
uqArgParser.add_argument("-concurrency", type=int, default=1, help="Number of concurrent evaluations")

#Parse Arguments
uqArgs = uqArgParser.parse_args()

#Reading input arguments
bimName = uqArgs.filenameBIM
samName = uqArgs.filenameSAM
evtName = uqArgs.filenameEVENT
edpName = uqArgs.filenameEDP
lossName = uqArgs.filenameLOSS
simName = uqArgs.filenameSIM
driverFile = uqArgs.driverFile

concurrency = 1
if uqArgs.concurrency is not None:
    concurrency = uqArgs.concurrency

doCleanup = True
if uqArgs.keepSamples.lower() == "true":
    doCleanup = False
elif  uqArgs.keepSamples.lower() == "false" or uqArgs.keepSamples is None:
    doCleanup = True
else:
    doCleanup = True
    print("Warning: {} is not a valid value for keepSamples".format(uqArgs.keepSamples))

numSamples = uqArgs.samples
rngSeed = uqArgs.seed

bldgName = bimName[:-9]

#setting workflow driver name based on platform
workflowDriver = 'workflow_driver'
if platform.system() == 'Windows':
    workflowDriver = 'workflow_driver.bat'

#Removing working directory for the current building, if it exists
if os.path.exists(bldgName) and os.path.isdir(bldgName):
    shutil.rmtree(bldgName)

os.mkdir(bldgName)

#Run Preprocess for Dakota
scriptDir = os.path.dirname(os.path.realpath(__file__))
numRVs = preProcessDakota(bimName, evtName, samName, edpName, lossName, simName, driverFile, bldgName, numSamples, rngSeed, concurrency)


#Create Template Directory and copy files
templateDir = "{}/templatedir".format(bldgName)
os.mkdir(templateDir)
bldgWorkflowDriver = "{}/{}".format(bldgName, workflowDriver)
st = os.stat(bldgWorkflowDriver)
os.chmod(bldgWorkflowDriver, st.st_mode | stat.S_IEXEC)
shutil.copy(bldgWorkflowDriver, "{}/{}".format(templateDir, workflowDriver))
shutil.copy("{}/dpreproSimCenter".format(scriptDir), templateDir)
shutil.copy(bimName, "{}/bim.j".format(templateDir))
shutil.copy(evtName, "{}/evt.j".format(templateDir))
shutil.copy(samName, "{}/sam.j".format(templateDir))
shutil.copy(edpName, "{}/edp.j".format(templateDir))
shutil.copy(simName, "{}/sim.j".format(templateDir))

#Run Dakota
dakotaCommand = "dakota -input dakota.in -output dakota.out -error dakota.err"
subprocess.Popen(dakotaCommand, cwd=bldgName, shell=True).wait()

#Postprocess Dakota results
if(numRVs > 0):
    postprocessCommand = '"{}/postprocessDAKOTA" {} {} {} {} {}'.format(scriptDir, numRVs, numSamples, bimName, edpName, lossName) \
    + ' ./{}/dakotaTab.out '.format(bldgName) + './{}/'.format(bldgName)
else:
    postprocessCommand = '"{}/postprocessDAKOTA" {} {} {} {} {}'.format(scriptDir, 1, 1, bimName, edpName, lossName) \
    + ' ./{}/dakotaTab.out '.format(bldgName) + './{}/'.format(bldgName)                      

subprocess.Popen(postprocessCommand, shell=True).wait()

#Clean up building folder
if doCleanup:
    shutil.rmtree(bldgName)