# written: fmk
from __future__ import division, print_function
import json
import os
import re
import sys
import subprocess
from time import gmtime, strftime
import shutil
import argparse

divider = '#' * 80
log_output = []

from WorkflowUtils import *

def main(workflowArguments):
    # The whole workflow is wrapped within a 'try' block.
    # A number of exceptions (files missing, explicit application failures, 
    # etc.) are handled explicitly to aid the user. But unhandled exceptions 
    # cause the workflow to stop with an error, handled in the exception block 
    # way at the bottom of this main() function
    try:

        workflow_log(divider)
        workflow_log('Start of run')
        workflow_log(divider)
        workflow_log('Workflow Configuration:       %s' % workflowArguments.configuration)
        workflow_log('Applications Registry: %s' % workflowArguments.registry)
        if workflowArguments.check:
            workflow_log('Run Type: check only')
        if workflowArguments.forceCleanup:
            workflow_log('Forced Cleanup Flag: True')
        workflow_log(divider)

        # First, we parse the applications registry to load all possible 
        # applications - for each application type we place in a dictionary key 
        # being name, value containing path to executable
        with open(workflowArguments.registry, 'r') as data_file:
            registryData = json.load(data_file)
            # convert all relative paths to full paths
            relative2fullpath(registryData)

        Applications = dict()
        appList = [a + 'Applications' for a in ['Building','Event','Modeling',
                                                'EDP','Simulation','UQ',
                                                'DamageAndLoss']]

        for app_type in appList:

            if app_type in registryData:
                xApplicationData = registryData[app_type]
                applicationsData = xApplicationData['Applications']

                for app in applicationsData:
                    appName = app['Name']
                    appExe = app['ExecutablePath']
                    if not app_type in Applications:
                        Applications[app_type] = dict()
                    Applications[app_type][appName] = appExe

        # open input file, and parse json into data
        with open(workflowArguments.configuration, 'r') as data_file:
            data = json.load(data_file)
            # convert all relative paths to full paths
            relative2fullpath(data)

        # get all application data, quit if error
        if 'WorkflowType' in data:
            typeWorkflow = data['WorkflowType']
        else:
            raise WorkFlowInputError('Need a Workflow Type')

        #TODO: check correct workflow type

        # Use workflow name to set cwd
        workDir = data['Name']
        if (not os.path.exists(workDir)):
            os.mkdir(workDir)

        # Now we parse for the applications & app specific data in workflow
        if 'Applications' in data:
            available_apps = data['Applications']
        else:
            raise WorkFlowInputError('Need an Applications Entry')

        # get building application and its data
        if 'Buildings' in available_apps:
            buildingApp = available_apps['Buildings']

            if 'BuildingApplication' in buildingApp:
                buildingApplication = buildingApp['BuildingApplication']

                # check building app in registry, if so get full executable path
                buildingAppData = buildingApp['ApplicationData']
                if workflowArguments.Min:
                    print('Overriding min: ' + str(workflowArguments.Min))
                    buildingAppData['Min'] = str(workflowArguments.Min)

                if workflowArguments.Max:
                    print('Overriding max: ' + str(workflowArguments.Max))
                    buildingAppData['Max'] = str(workflowArguments.Max)

                if (buildingApplication in 
                    Applications['BuildingApplications'].keys()):
                    buildingAppExe = (u'' +
                        #u'' + Applications['BuildingApplications'].get(buildingApplication))                         
                        Applications['BuildingApplications'][buildingApplication])
                else:
                    raise WorkFlowInputError(
                        'Building application {} not in registry'.format(
                            buildingApplication))
            else:
                raise WorkFlowInputError(
                    'Need a Building Generator Application in Buildings')
        else:
            raise WorkFlowInputError('Need a Buildings Entry in Applications')


        # get events, for each application and its data .. FOR NOW 1 EVENT
        if 'Events' in available_apps:
            events = available_apps['Events']

            for event in events:
                if 'EventClassification' in event:
                    eventClassification = event['EventClassification']
                    if eventClassification == 'Earthquake':
                        if 'EventApplication' in event:
                            eventApplication = event['EventApplication']
                            eventAppData = event['ApplicationData']
                            eventData = event['ApplicationData']
                            if (eventApplication in 
                                Applications['EventApplications'].keys()):
                                eventAppExe = (u'' + 
                                    Applications['EventApplications'][eventApplication])
                            else:
                                raise WorkFlowInputError(
                                    'Event application {} not in registry'.format(
                                        eventApplication))
                        else:
                            raise WorkFlowInputError(
                                'Need an EventApplication section')
                    else:
                        raise WorkFlowInputError(
                            'Event classification must be Earthquake, '
                            'not {}'.format(eventClassification))
                else:
                    raise WorkFlowInputError('Need Event Classification')
        else:
            raise WorkFlowInputError('Need an Events Entry in Applications')


        # get modeling application and its data
        if 'Modeling' in available_apps:
            modelingApp = available_apps['Modeling']

            if 'ModelingApplication' in modelingApp:
                modelingApplication = modelingApp['ModelingApplication']

                # check modeling app in registry, if so get full executable path
                modelingAppData = modelingApp['ApplicationData']
                if (modelingApplication in 
                    Applications['ModelingApplications'].keys()):
                    modelingAppExe = (u'' + 
                        Applications['ModelingApplications'][modelingApplication])
                else:
                    raise WorkFlowInputError(
                        'Modeling application {} not in registry'.format(
                            modelingApplication))
            else:
                raise WorkFlowInputError(
                    'Need a ModelingApplication in Modeling data')

        else:
            raise WorkFlowInputError('Need a Modeling Entry in Applications')


        # get EDP application and its data
        if 'EDP' in available_apps:
            edpApp = available_apps['EDP']

            if 'EDPApplication' in edpApp:
                edpApplication = edpApp['EDPApplication']

                # check modeling app in registry, if so get full executable path
                edpAppData = edpApp['ApplicationData']
                if edpApplication in Applications['EDPApplications'].keys():
                    edpAppExe = (u'' + 
                        Applications['EDPApplications'][edpApplication])
                else:
                    raise WorkFlowInputError(
                        'EDP application {} not in registry'.format(
                            edpApplication))
            else:
                raise WorkFlowInputError('Need an EDPApplication in EDP data')
        else:
            raise WorkFlowInputError('Need an EDP Entry in Applications')

        # get simulation application and its data
        if 'Simulation' in available_apps:
            simulationApp = available_apps['Simulation']

            if 'SimulationApplication' in simulationApp:
                simulationApplication = simulationApp['SimulationApplication']

                # check modeling app in registry, if so get full executable path
                simAppData = simulationApp['ApplicationData']
                if (simulationApplication in 
                    Applications['SimulationApplications'].keys()):
                    simAppExe = (u'' + 
                        Applications['SimulationApplications'][simulationApplication])
                else:
                    raise WorkFlowInputError(
                        'Simulation application {} not in registry'.format(
                            simulationApplication))
            else:
                raise WorkFlowInputError(
                    'Need an SimulationApplication in Simulation data')
        else:
            raise WorkFlowInputError('Need a Simulation Entry in Applications')


        # get UQ application and its data
        if 'UQ-Simulation' in available_apps:
            uqApp = available_apps['UQ-Simulation']

            if 'UQApplication' in uqApp:
                uqApplication = uqApp['UQApplication']

                # check modeling app in registry, if so get full executable path
                uqAppData = uqApp['ApplicationData']
                if uqApplication in Applications['UQApplications'].keys():
                    uqAppExe = (u'' + 
                                Applications['UQApplications'][uqApplication])
                else:
                    raise WorkFlowInputError(
                        'UQ application {} not in registry'.format(
                            uqApplication))
            else:
                raise WorkFlowInputError('Need a UQApplication in UQ data')
        else:
            raise WorkFlowInputError('Need a Simulation Entry in Applications')

        if 'Damage&Loss' in available_apps:
            DLApp = available_apps['Damage&Loss']

            if 'Damage&LossApplication' in DLApp:
                dlApplication = DLApp['Damage&LossApplication']

                # check modeling app in registry, if so get full executable path
                dlAppData = DLApp['ApplicationData']
                if (dlApplication in 
                    Applications['DamageAndLossApplications'].keys()):
                    dlAppExe = (u'' + 
                        Applications['DamageAndLossApplications'][dlApplication])
                else:
                    raise WorkFlowInputError(
                        'Damage & Loss application {} not in registry'.format(
                            dlApplication))
            else:
                raise WorkFlowInputError(
                    'Need a Damage&LossApplicationApplication in Damage & Loss '
                    'data')
        else:
            raise WorkFlowInputError('Need a Simulation Entry in Applications')

        workflow_log('SUCCESS: Parsed workflow input')
        workflow_log(divider)


        # Now invoke the applications

        # put building generator application data into list and exe
        buildingsFile = os.path.abspath('{}/buildings.json'.format(workDir))
        if 'buildingFile' in data:
            buildingsFile = os.path.abspath(
                os.path.join(workDir, data['buildingFile']))

        buildingsFile = (u'' + 
                         buildingsFile.replace(
                             '.json', '{}-{}.json'.format(buildingAppData['Min'],
                                                          buildingAppData['Max'])))
        buildingAppDataList = [buildingAppExe, buildingsFile]

        for key in buildingAppData.keys():
            buildingAppDataList.append(u'-' + key)
            buildingAppDataList.append(u'' + buildingAppData.get(key))

            # sanity check - added by rynge 8/27/18
            if key == 'Min' or key == 'Max':
                value = buildingAppData.get(key)
                # make sure we have an int
                if not re.search('^[0-9]+$', value):
                    print('Expected value for '
                          '{} is not an integer: {}'.format(key, value))
                    print(buildingAppData)
                    sys.exit(1)

        buildingAppDataList.append('-getRV')
        command, result, returncode = runApplication(buildingAppDataList,
                                                     workDir)
        log_output.append([command, result, returncode])

        del buildingAppDataList[-1]

        # Now we need to open buildingsfile and for each building
        #  - get RV for EVENT file for building
        #  - get RV for SAM file for building
        #  - get EDP for buildings and event
        #  - get SAM for buildings, event and EDP
        #  - perform Simulation
        #  - getDL

        with open(buildingsFile, 'r') as data_file:
            data = json.load(data_file)

        for building in data:
            id = u'' + building['id']
            bimFILE = u'' + building['file']
            eventFILE = id + '-EVENT.json'
            samFILE = id + '-SAM.json'
            edpFILE = id + '-EDP.json'
            dlFILE = id + '-DL.json'
            simFILE = id + '-SIM.json'
            driverFile = id + '-driver'

            # open driver file & write building app (minus the -getRV) to it
            driverFILE = open(os.path.join(workDir, driverFile), 'w')
            for item in buildingAppDataList:
                driverFILE.write('"{}" '.format(item))
            driverFILE.write('\n')


            # get RV for event
            eventAppDataList = [eventAppExe, 
                                '-filenameBIM', bimFILE,
                                '-filenameEVENT', eventFILE]
            if (eventAppExe.endswith('.py')):
                eventAppDataList.insert(0, 'python')

            for key in eventAppData.keys():
                eventAppDataList.append('-' + key)
                value = u'' + eventAppData.get(key)
                if (os.path.exists(value) and not os.path.isabs(value)):
                    value = os.path.abspath(value)
                eventAppDataList.append(u'' + value)

            for item in eventAppDataList:
                driverFILE.write('"{}" '.format(item))
            driverFILE.write('\n')

            eventAppDataList.append('-getRV')
            command, result, returncode = runApplication(eventAppDataList,
                                                         workDir)
            log_output.append([command, result, returncode])


            # get RV for building model
            modelAppDataList = [modelingAppExe, 
                                '-filenameBIM', bimFILE,
                                '-filenameEVENT', eventFILE, 
                                '-filenameSAM', samFILE]

            for key in modelingAppData.keys():
                modelAppDataList.append('-' + key)
                modelAppDataList.append(u'' + modelingAppData.get(key))

            for item in modelAppDataList:
                driverFILE.write('"{}" '.format(item))
            driverFILE.write('\n')

            modelAppDataList.append('-getRV')
            command, result, returncode = runApplication(modelAppDataList,
                                                         workDir)
            log_output.append([command, result, returncode])


            # get RV for EDP
            edpAppDataList = [edpAppExe, 
                              '-filenameBIM', bimFILE,
                              '-filenameEVENT', eventFILE, 
                              '-filenameSAM', samFILE,
                              '-filenameEDP', edpFILE]

            for key in edpAppData.keys():
                edpAppDataList.append('-' + key)
                edpAppDataList.append(u'' + edpAppData.get(key))

            for item in edpAppDataList:
                driverFILE.write('"{}" '.format(item))
            driverFILE.write('\n')

            edpAppDataList.append('-getRV')
            command, result, returncode = runApplication(edpAppDataList,
                                                         workDir)
            log_output.append([command, result, returncode])


            # get RV for Simulation
            simAppDataList = [simAppExe, 
                              '-filenameBIM', bimFILE,
                              '-filenameSAM', samFILE, 
                              '-filenameEVENT', eventFILE,
                              '-filenameEDP', edpFILE, 
                              '-filenameSIM', simFILE]

            if (simAppExe.endswith('.py')):
                simAppDataList.insert(0, 'python')

            for key in simAppData.keys():
                simAppDataList.append('-' + key)
                simAppDataList.append(u'' + simAppData.get(key))

            for item in simAppDataList:
                driverFILE.write('"{}" '.format(item))
            driverFILE.write('\n')

            simAppDataList.append('-getRV')
            command, result, returncode = runApplication(simAppDataList,
                                                         workDir)
            log_output.append([command, result, returncode])


            # If FemaP58-LU is used, then we add CreateLoss to Dakota Driver
            # (Otherwise, loss assessment is performed after all structural 
            # response simulations are finished.)
            if dlApplication == 'FemaP58-LU':
                dlAppDataList = [dlAppExe, 
                                 '-filenameBIM', bimFILE, 
                                 '-filenameEDP', edpFILE, 
                                 '-filenameLOSS', dlFILE]
    
                for key in dlAppData.keys():
                    dlAppDataList.append('-' + key)
                    dlAppDataList.append(u'' + dlAppData.get(key))
    
                for item in dlAppDataList:
                    driverFILE.write('"{}" '.format(item))

            # perform the structural response simulation
            driverFILE.close()

            uqAppDataList = [uqAppExe, 
                             '-filenameBIM', bimFILE, '-filenameSAM', samFILE, 
                             '-filenameEVENT', eventFILE, '-filenameEDP', edpFILE, 
                             '-filenameLOSS', dlFILE, '-filenameSIM', simFILE, 
                             '-driverFile', driverFile]
            if (uqAppExe.endswith('.py')):
                uqAppDataList.insert(0, 'python')

            for key in uqAppData.keys():
                uqAppDataList.append('-' + key)
                if uqAppData.get(key) is not None:
                    uqAppDataList.append(u'' + str(uqAppData.get(key)))

            if workflowArguments.check:
                workflow_log('Check run only. No simulation performed.')
            else:
                workflow_log('Running simulation for building {} ...'.format(id))
                command, result, returncode = runApplication(uqAppDataList,
                                                             workDir)
                log_output.append([command, result, returncode])

            # Perform the loss assessment (unless FemaP58-LU is used)
            
            #Cleanup if necessary (cleanup is forced for HPC)
            if workflowArguments.forceCleanup:
                cleanupFile(os.path.join(workDir, eventFILE))
                cleanupFile(os.path.join(workDir, samFILE))
                cleanupFile(os.path.join(workDir, edpFILE))
                cleanupFile(os.path.join(workDir, simFILE))
                cleanupFile(os.path.join(workDir, driverFile))

                cleanupFolder(os.path.join(workDir, id))

        # Collect the Damage and Loss data into a single file
        minBldg = buildingAppData['Min']
        maxBldg = buildingAppData['Max']
        readDLs = [os.path.abspath("../build/bin/ReadDLs"), minBldg, maxBldg,
                   "DLs{}-{}.csv".format(minBldg, maxBldg)]
        command, result, returncode = runApplication(readDLs, workDir)
        log_output.append([command, result, returncode])

        if workflowArguments.forceCleanup:
            for building in data:
                id = u'' + building['id']
                dlFILE = id + '-DL.json'
                cleanupFile(os.path.join(workDir, dlFILE))
                cleanupFile(os.path.join(workDir, building['file']))

        return [minBldg, maxBldg]

    except WorkFlowInputError as e:
        workflow_log('workflow error: {}'.format(e.value))
        workflow_log(divider)
        exit(1)

    # unhandled exceptions are handled here
    except:
        workflow_log('unhandled exception... exiting')
        raise
        exit(1) # azs - this seems unnecessary 


if __name__ == '__main__':

    #Defining the command line arguments
    workflowArgParser = argparse.ArgumentParser("Run the NHERI SimCenter workflow for a set of buildings")
    workflowArgParser.add_argument("configuration", help="Configuration file specifying the applications and data to be used")
    workflowArgParser.add_argument("-Min", type=int, default=None, help="Override the index of the first building")
    workflowArgParser.add_argument("-Max", type=int, default=None, help="Override the index of the last building")
    workflowArgParser.add_argument("-c", "--check", help="Check the configuration file")
    workflowArgParser.add_argument("-r", "--registry", default="WorkflowApplications.json", help="Path to file containing registered workflow applications")
    workflowArgParser.add_argument("-f", "--forceCleanup",  action="store_true", help="Path to file containing registered workflow applications")

    #Parsing the command line arguments
    workflowArguments = workflowArgParser.parse_args()    

    #Calling the main workflow method and passing the parsed arguments
    [min, max] = main(workflowArguments)

    workflow_log_file = 'workflow-log-{}-{}.txt'.format(min, max)
    log_filehandle = open(workflow_log_file, 'w')

    print(divider, file=log_filehandle)
    print('Start of Log', file=log_filehandle)
    print(divider, file=log_filehandle)
    print(workflow_log_file, file=log_filehandle)
    # nb: log_output is a global variable, defined at the top of this script.
    for result in log_output:
        print(divider, file=log_filehandle)
        print('command line:\n{}\n'.format(result[0]), 
              file=log_filehandle)
        print(divider, file=log_filehandle)
        print('output from process:\n{}\n'.format(result[1]), 
              file=log_filehandle)

    print(divider, file=log_filehandle)
    print('End of Log', file=log_filehandle)
    print(divider, file=log_filehandle)

    workflow_log('Log file: {}'.format(workflow_log_file))
    workflow_log('End of run.')

