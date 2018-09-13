import sys
import os
import platform
import shutil
import subprocess
import stat
from preprocessJSON import preProcessDakota

#First we need to set the path and environment
home = os.path.expanduser('~')
env = os.environ
if os.getenv("PEGASUS_WF_UUID") is not None:
    print "Pegasus job detected - Pegasus will set up the env"
elif platform.system() == 'Darwin':
    env["PATH"] = env["PATH"] + ':{}/bin'.format(home)
    env["PATH"] = env["PATH"] + ':{}/dakota/bin'.format(home)
elif platform.system() == 'Linux':
    env["PATH"] = env["PATH"] + ':{}/bin'.format(home)
    env["PATH"] = env["PATH"] + ':{}/dakota/dakota-6.5/bin'.format(home)
else:
    print "PLATFORM {} NOT RECOGNIZED".format(platform.system)

#Reading input arguments
bimName = sys.argv[2]
samName = sys.argv[4]
evtName = sys.argv[6]
edpName = sys.argv[8]
lossName = sys.argv[10]
simName = sys.argv[12]
driverFile = sys.argv[14]

numSamples = 5
bldgName = bimName[:-9]


#Removing working directory for the current building, if it exists
if os.path.exists(bldgName):
    shutil.rmtree(bldgName)

os.mkdir(bldgName)

#Run Preprocess for Dakota
scriptDir = os.path.dirname(os.path.realpath(__file__))
# preProcessArgs = ["python", "{}/preprocessJSON.py".format(scriptDir), bimName, evtName,\
# samName, edpName, lossName, simName, driverFile, scriptDir, bldgName]
# subprocess.call(preProcessArgs)
numRVs = preProcessDakota(bimName, evtName, samName, edpName, lossName, simName, driverFile, bldgName)


#Create Template Directory and copy files
templateDir = "{}/templatedir".format(bldgName)
os.mkdir(templateDir)
bldgWorkflowDriver = "{}/workflow_driver".format(bldgName)
st = os.stat(bldgWorkflowDriver)
os.chmod(bldgWorkflowDriver, st.st_mode | stat.S_IEXEC)
shutil.copy("{}/dpreproSimCenter".format(scriptDir), bldgName)
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
#shutil.rmtree(bldgName)