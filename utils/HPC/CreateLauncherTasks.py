import sys
import math

count = int(sys.argv[1])
configFile = sys.argv[2]
outDir = sys.argv[3]
tasksCount = int(math.ceil(count/10.0))

with open('WorkflowTasks', 'w+') as tasksFile:
    subfolder = 0    
    for i in range(0, tasksCount):
        if (i%500) == 0:
            subfolder = subfolder + 1
        min = i * 10 + 1
        max = (i + 1) * 10
        tasksFile.write('cd /tmp/WorkflowRegionalEarthquake/Workflow && ')  
        tasksFile.write('python RegionalEarthquakeSimulation.py {} -Min {} -Max {} -f && '.format(configFile, min, max))  
        tasksFile.write('mkdir -p {}/results/{}/ && '.format(outDir, subfolder))
        tasksFile.write('cp -f {}/DLs{}-{}.csv {}/results/{}/ && '.format(configFile[:-5], min, max, outDir, subfolder))
        tasksFile.write('mkdir -p {}/logs/{}/ && cp -f workflow-log-{}-{}.txt {}/logs/{}/\n'.format(outDir, subfolder, min, max, outDir, subfolder))

  
