import sys

count = int(sys.argv[1])

with open('WorkflowTasks', 'w+') as tasksFile:
    subfolder = 0    
    for i in range(0, count):
        if (i%500) == 0:
            subfolder = subfolder + 1
        min = i * 10 + 1
        max = (i + 1) * 10
        tasksFile.write('cd /tmp/WorkflowRegionalEarthquake/Workflow && ')  
        tasksFile.write('python RegionalEarthquakeSimulation.py run Workflow5.json WorkflowApplications.json -Min {} -Max {} && '.format(min, max))  
        tasksFile.write('mkdir -p $SCRATCH/results/{}/ && '.format(subfolder))
        tasksFile.write('cp -f Workflow5/DLs{}-{}.csv $SCRATCH/results/{}/\n'.format(min, max, subfolder))

  
