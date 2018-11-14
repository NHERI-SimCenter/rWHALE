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
        tasksFile.write('python RegionalEarthquakeSimulation.py run {} WorkflowApplications.json -Min {} -Max {} && '.format(configFile, min, max))  
        tasksFile.write('mkdir -p {}/results/{}/ && '.format(outDir, subfolder))
        tasksFile.write('cp -f {}/DLs{}-{}.csv {}/results/{}/\n'.format(configFile[:-5], min, max, outDir, subfolder))

  
