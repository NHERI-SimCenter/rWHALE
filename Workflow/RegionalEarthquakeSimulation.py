# written: fmk

import json
import os
import sys
import subprocess
from time import gmtime, strftime

divider = '#' * 80
log_output = []

from WorkflowUtils import *

def main(run_type, inputFile, applicationsRegistry):
    # the whole workflow is wrapped within a 'try' block.
    # a number of exceptions (files missing, explicit application failures, etc.) are
    # handled explicitly to aid the user.
    # But unhandled exceptions case the workflow to stop with an error, handled in the
    # exception block way at the bottom of this main() function
    try:

        workflow_log(divider)
        workflow_log('Start of run')
        workflow_log(divider)
        workflow_log('workflow input file:       %s' % inputFile)
        workflow_log('application registry file: %s' % applicationsRegistry)
        workflow_log('runtype:                   %s' % run_type)
        workflow_log(divider)


        #
        # first we parse the applications registry to load all possible applications
        #  - for each application type we place in a dictionary key being name, value containing path to executable
        #
        with open(applicationsRegistry, 'r') as data_file:
            registryData = json.load(data_file)
            # convert all relative paths to full paths
            relative2fullpath(registryData)

        A = 'Applications'
        Applications = dict()
        appList = 'Building Event Modeling EDP Simulation UQ DamageAndLoss'.split(' ')
        appList = [a + A for a in appList]

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

        #
        # open input file, and parse json into data
        #

        with open(inputFile, 'r') as data_file:
            data = json.load(data_file)
            # convert all relative paths to full paths
            relative2fullpath(data)

        #
        # get all application data, quit if error
        #

        if 'WorkflowType' in data:
            typeWorkflow = data['WorkflowType']
        else:
            raise WorkFlowInputError('Need a Workflow Type')

        # check correct workflow type

        #
        # now we parse for the applications & app specific data in workflow
        #

        if 'Applications' in data:
            available_apps = data['Applications']
        else:
            raise WorkFlowInputError('Need an Applications Entry')

        #
        # get building application and its data
        #

        if 'Buildings' in available_apps:
            buildingApp = available_apps['Buildings']

            if 'BuildingApplication' in buildingApp:
                buildingApplication = buildingApp['BuildingApplication']

                # check building app in registry, if so get full executable path
                buildingAppData = buildingApp['ApplicationData']
                if buildingApplication in Applications['BuildingApplications'].keys():
                    buildingAppExe = Applications['BuildingApplications'].get(buildingApplication)
                else:
                    raise WorkFlowInputError('Building application %s not in registry' % buildingApplication)

            else:
                raise WorkFlowInputError('Need a Building Generator Application in Buildings')

        else:
            raise WorkFlowInputError('Need a Buildings Entry in Applications')

        #
        # get events, for each the  application and its data .. FOR NOW 1 EVENT
        #

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
                            if eventApplication in Applications['EventApplications'].keys():
                                eventAppExe = Applications['EventApplications'].get(eventApplication)
                            else:
                                raise WorkFlowInputError('Event application %s not in registry' % eventApplication)

                        else:
                            raise WorkFlowInputError('Need an EventApplication section')


                    else:
                        raise WorkFlowInputError('Event classification must be Earthquake, not %s' % eventClassification)

                else:
                    raise WorkFlowInputError('Need Event Classification')

        else:
            raise WorkFlowInputError('Need an Events Entry in Applications')

        #
        # get modeling application and its data
        #

        if 'Modeling' in available_apps:
            modelingApp = available_apps['Modeling']

            if 'ModelingApplication' in modelingApp:
                modelingApplication = modelingApp['ModelingApplication']

                # check modeling app in registry, if so get full executable path
                modelingAppData = modelingApp['ApplicationData']
                if modelingApplication in Applications['ModelingApplications'].keys():
                    modelingAppExe = Applications['ModelingApplications'].get(modelingApplication)
                else:
                    raise WorkFlowInputError('Modeling application %s not in registry' % modelingApplication)

            else:
                raise WorkFlowInputError('Need a ModelingApplication in Modeling data')


        else:
            raise WorkFlowInputError('Need a Modeling Entry in Applications')

        #
        # get edp application and its data
        #

        if 'EDP' in available_apps:
            edpApp = available_apps['EDP']

            if 'EDPApplication' in edpApp:
                edpApplication = edpApp['EDPApplication']

                # check modeling app in registry, if so get full executable path
                edpAppData = edpApp['ApplicationData']
                if edpApplication in Applications['EDPApplications'].keys():
                    edpAppExe = Applications['EDPApplications'].get(edpApplication)
                else:
                    raise WorkFlowInputError('EDP application %s not in registry', edpApplication)

            else:
                raise WorkFlowInputError('Need an EDPApplication in EDP data')


        else:
            raise WorkFlowInputError('Need an EDP Entry in Applications')

        if 'Simulation' in available_apps:
            simulationApp = available_apps['Simulation']

            if 'SimulationApplication' in simulationApp:
                simulationApplication = simulationApp['SimulationApplication']

                # check modeling app in registry, if so get full executable path
                simAppData = simulationApp['ApplicationData']
                if simulationApplication in Applications['SimulationApplications'].keys():
                    simAppExe = Applications['SimulationApplications'].get(simulationApplication)
                else:
                    raise WorkFlowInputError('Simulation application %s not in registry', simulationApplication)

            else:
                raise WorkFlowInputError('Need an SimulationApplication in Simulation data')


        else:
            raise WorkFlowInputError('Need a Simulation Entry in Applications')

        if 'UQ-Simulation' in available_apps:
            uqApp = available_apps['UQ-Simulation']

            if 'UQApplication' in uqApp:
                uqApplication = uqApp['UQApplication']

                # check modeling app in registry, if so get full executable path
                uqAppData = uqApp['ApplicationData']
                if uqApplication in Applications['UQApplications'].keys():
                    uqAppExe = Applications['UQApplications'].get(uqApplication)
                else:
                    raise WorkFlowInputError('UQ application %s not in registry', uqApplication)

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
                if dlApplication in Applications['DamageAndLossApplications'].keys():
                    dlAppExe = Applications['DamageAndLossApplications'].get(dlApplication)
                else:
                    raise WorkFlowInputError('Dmage & Loss application %s not in registry' % dlApplication)

            else:
                raise WorkFlowInputError('Need a Damage&LossApplicationApplication in Damage & Loss data')

        else:
            raise WorkFlowInputError('Need a Simulation Entry in Applications')

        workflow_log('SUCCESS: Parsed workflow input')
        workflow_log(divider)

        #
        # now invoke the applications
        #

        #
        # put building generator application data into list and exe
        #

        buildingsFile = 'buildings.json'
        buildingAppDataList = [buildingAppExe, buildingsFile]

        for key in buildingAppData.keys():
            buildingAppDataList.append('-' + key.encode('ascii', 'ignore'))
            buildingAppDataList.append(buildingAppData.get(key).encode('ascii', 'ignore'))

        buildingAppDataList.append('-getRV')
        command, result, returncode = runApplication(buildingAppDataList)
        log_output.append([command, result, returncode])

        del buildingAppDataList[-1]

        #
        # now we need to open buildingsfile and for each building
        #  - get RV for EVENT file for building
        #  - get RV for SAM file for building
        #  - get EDP for buildings and event
        #  - get SAM for buildings, event and EDP
        #  - perform Simulation
        #  - getDL

        with open(buildingsFile, 'r') as data_file:
            data = json.load(data_file)

        for building in data:
            id = building['id']
            bimFILE = building['file']
            eventFILE = id + '-EVENT.json'
            samFILE = id + '-SAM.json'
            edpFILE = id + '-EDP.json'
            dlFILE = id + '-DL.json'
            simFILE = id + '-SIM.json'
            driverFile = id + '-driver'

            # open driver file & write building app (minus the -getRV) to it
            driverFILE = open(driverFile, 'w')
            for item in buildingAppDataList:
                driverFILE.write('%s ' % item)
            driverFILE.write('\n')

            # get RV for event
            eventAppDataList = [eventAppExe, '-filenameBIM', bimFILE, '-filenameEVENT', eventFILE]
            if (eventAppExe.endswith('.py')):
                eventAppDataList.insert(0, 'python')

            for key in eventAppData.keys():
                eventAppDataList.append('-' + key.encode('ascii', 'ignore'))
                value = eventAppData.get(key)
                if (os.path.exists(value) and not os.path.isabs(value)):
                    value = os.path.abspath(value)
                eventAppDataList.append(value.encode('ascii', 'ignore'))

            for item in eventAppDataList:
                driverFILE.write('%s ' % item)
            driverFILE.write('\n')

            eventAppDataList.append('-getRV')
            command, result, returncode = runApplication(eventAppDataList)
            log_output.append([command, result, returncode])

            # get RV for building model
            modelAppDataList = [modelingAppExe, '-filenameBIM', bimFILE, '-filenameEVENT', eventFILE, '-filenameSAM',
                                samFILE]

            for key in modelingAppData.keys():
                modelAppDataList.append('-' + key.encode('ascii', 'ignore'))
                modelAppDataList.append(modelingAppData.get(key).encode('ascii', 'ignore'))

            for item in modelAppDataList:
                driverFILE.write('%s ' % item)
            driverFILE.write('\n')

            modelAppDataList.append('-getRV')
            command, result, returncode = runApplication(modelAppDataList)
            log_output.append([command, result, returncode])


            # get RV for EDP!
            edpAppDataList = [edpAppExe, '-filenameBIM', bimFILE, '-filenameEVENT', eventFILE, '-filenameSAM', samFILE,
                              '-filenameEDP', edpFILE]

            for key in edpAppData.keys():
                edpAppDataList.append('-' + key.encode('ascii', 'ignore'))
                edpAppDataList.append(edpAppData.get(key).encode('ascii', 'ignore'))

            for item in edpAppDataList:
                driverFILE.write('%s ' % item)
            driverFILE.write('\n')

            edpAppDataList.append('-getRV')
            command, result, returncode = runApplication(edpAppDataList)
            log_output.append([command, result, returncode])

            # get RV for Simulation
            simAppDataList = [simAppExe, '-filenameBIM', bimFILE, '-filenameSAM', samFILE, '-filenameEVENT', eventFILE,
                              '-filenameEDP', edpFILE, '-filenameSIM', simFILE]

            for key in simAppData.keys():
                simAppDataList.append('-' + key.encode('ascii', 'ignore'))
                simAppDataList.append(simAppData.get(key).encode('ascii', 'ignore'))

            for item in simAppDataList:
                driverFILE.write('%s ' % item)
            driverFILE.write('\n')

            simAppDataList.append('-getRV')
            command, result, returncode = runApplication(simAppDataList)
            log_output.append([command, result, returncode])

            # Adding CreateLoss to Dakota Driver
            dlAppDataList = [dlAppExe, '-filenameBIM', bimFILE, '-filenameEDP', edpFILE, '-filenameLOSS', dlFILE]

            for key in dlAppData.keys():
                dlAppDataList.append('-' + key.encode('ascii', 'ignore'))
                dlAppDataList.append(dlAppData.get(key).encode('ascii', 'ignore'))

            for item in dlAppDataList:
                driverFILE.write('%s ' % item)

            # perform the simulation
            driverFILE.close()

            uqAppDataList = [uqAppExe, '-filenameBIM', bimFILE, '-filenameSAM', samFILE, '-filenameEVENT', eventFILE,
                             '-filenameEDP', edpFILE, '-filenameLOSS', dlFILE, '-filenameSIM', simFILE, 'driverFile',
                             driverFile]

            for key in uqAppData.keys():
                uqAppDataList.append('-' + key.encode('ascii', 'ignore'))
                uqAppDataList.append(simAppData.get(key).encode('ascii', 'ignore'))

            if run_type == 'run':
                workflow_log('Running Simulation...')
                workflow_log(' '.join(uqAppDataList))
                command, result, returncode = runApplication(uqAppDataList)
                log_output.append([command, result, returncode])
                workflow_log('Simulation ended...')
            else:
                workflow_log('Check run only. No simulation performed.')

    except WorkFlowInputError as e:
        workflow_log('workflow error: %s' % e.value)
        workflow_log(divider)
        exit(1)

    # unhandled exceptions are handled here
    except:
        raise
        workflow_log('unhandled exception... exiting')
        exit(1)


if __name__ == '__main__':

    if len(sys.argv) != 4:
        print('\nNeed three arguments, e.g.:\n')
        print('    python %s action workflowinputfile.json workflowapplications.json' % sys.argv[0])
        print('\nwhere: action is either check or run\n')
        exit(1)

    run_type = sys.argv[1]
    inputFile = sys.argv[2]
    applicationsRegistry = sys.argv[3]

    main(run_type, inputFile, applicationsRegistry)

    workflow_log_file = 'workflow-log-%s.txt' % (strftime('%Y-%m-%d-%H-%M-%S-utc', gmtime()))
    log_filehandle = open(workflow_log_file, 'wb')

    print >>log_filehandle, divider
    print >>log_filehandle, 'Start of Log'
    print >>log_filehandle, divider
    print >>log_filehandle, workflow_log_file
    # nb: log_output is a global variable, defined at the top of this script.
    for result in log_output:
        print >>log_filehandle, divider
        print >>log_filehandle, 'command line:\n%s\n' % result[0]
        print >>log_filehandle, divider
        print >>log_filehandle, 'output from process:\n%s\n' % result[1]

    print >>log_filehandle, divider
    print >>log_filehandle, 'End of Log'
    print >>log_filehandle, divider

    workflow_log('Log file: %s' % workflow_log_file)
    workflow_log('End of run.')

