#!/bin/sh

echo "Current Job Directory"
pwd

# Copying workflow to the job directory
echo ""
echo "Copying Workflow Applications"
cp -rf /home1/05361/elhaddad/NHERI/SimCenter/rWHALE/v110/WorkflowRegionalEarthquake .

# Extracting regional simulation data
echo ""
echo "Current Job Directory"
ls -lh
for dataFile in ${dataFiles}; do
	$HOME/p7zip_16.02/bin/7za x $dataFile -oWorkflowRegionalEarthquake/build/data
done
ls WorkflowRegionalEarthquake/build/data

# Copying config file
cp ${configFile} WorkflowRegionalEarthquake/Workflow
echo ""
echo "Workflow directory"
ls -lh WorkflowRegionalEarthquake/Workflow


#Creating launcher tasks
echo $PWD
python WorkflowRegionalEarthquake/utils/HPC/CreateLauncherTasks.py ${buildingsCount} ${configFile} $PWD #${buildingsCount}


# Copying data to local node /tmp
echo "Started copying data to compute nodes"
date
for node in $(scontrol show hostnames $SLURM_NODELIST) ; do
  srun -N 1-1 -n 1 -w $node cp -rf $PWD/WorkflowRegionalEarthquake /tmp &
done
wait
echo "Finished copying data to compute nodes"
date

# Setting prerequisites (OpenSees, Dakota...etc.)
module load dakota
module load launcher/3.1
export PATH=/home1/00477/tg457427/bin:$PATH
export LAUNCHER_JOB_FILE=$PWD/WorkflowTasks
$TACC_LAUNCHER_DIR/paramrun

echo ""
echo "Aggregating Results"
python ./WorkflowRegionalEarthquake/Workflow/AggregateDLs.py ./results RegionalDamageLoss.csv

echo ""
echo "Archiving Log Files"
date
$HOME/p7zip_16.02/bin/7za a -tzip logs.zip ./logs/* 
date

echo ""
echo "Cleaning up before archiving"
rm -rf WorkflowRegionalEarthquake
rm -rf results
rm -rf logs


for dataFile in ${dataFiles}; do
	rm $dataFile
done
rm ${configFile}