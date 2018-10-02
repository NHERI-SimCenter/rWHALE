import sys
import os
import platform
import shutil
import subprocess
from preprocessJSON import preProcessDakota
import platform

#Reading input arguments
bimName = sys.argv[2]
samName = sys.argv[4]
evtName = sys.argv[6]
edpName = sys.argv[8]
lossName = sys.argv[10]
simName = sys.argv[12]
driverFile = sys.argv[14]

numSamples = 5 #TODO: needs to be configured

bldgName = bimName[:-9]

#setting workflow driver name based on platform
workflowDriver = 'workflow_driver'
if platform.system == 'Windows':
    workflowDriver = 'workflow_driver.bat'

#Removing working directory for the current building, if it exists
if os.path.exists(bldgName) and os.path.isdir(bldgName):
    shutil.rmtree(bldgName)

os.mkdir(bldgName)

#Run Preprocess for Dakota
scriptDir = os.path.dirname(os.path.realpath(__file__))
numRVs = preProcessDakota(bimName, evtName, samName, edpName, lossName, simName, driverFile, bldgName, numSamples)


#Create Template Directory and copy files
templateDir = "{}/templatedir".format(bldgName)
os.mkdir(templateDir)
bldgWorkflowDriver = "{}/{}".format(bldgName, workflowDriver)
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
postprocessCommand = '{}/postprocessDAKOTA {} {} {} {} {}'.format(scriptDir, numRVs, numSamples, bimName, edpName, lossName) \
+ ' ./{}/dakotaTab.out '.format(bldgName) + './{}/'.format(bldgName)
subprocess.Popen(postprocessCommand, shell=True).wait()

#Clean up building folder
shutil.rmtree(bldgName)