# written: fmk

import json
import os
import sys
import stat
import shutil
import subprocess
from pprint import pprint

# function to return result of invoking an application
def runApplication(application, args):
    argsPopen=[];
    argsPopen.append(application)
    argsPopen.extend(args);

    p = subprocess.Popen(argsPopen, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    for line in p.stdout.readlines():
        print line
    retval = p.wait()    


def main():
    inputArgs = sys.argv

    inputFile = inputArgs[1]
    applicationsRegistry = "WorkflowApplications.json"

    #
    # first we parse the applications registry to load all possible applications
    #  - for each application type we place in a dictionary key being name, value containing path to executable
    #

    with open(applicationsRegistry, 'r') as data_file: 
        registryData = json.load(data_file)


    buildingApplications = dict()
    eventApplications = dict()
    modelingApplications = dict()
    edpApplications = dict()
    simulationApplications = dict()
    uqApplications = dict()
    dlApplications = dict()

    if 'BuildingApplications' in registryData:
        buildingApplicationData = registryData["BuildingApplications"]
        applicationsData = buildingApplicationData["Applications"]

        for app in applicationsData:
            appName = app["Name"]
            appExe = app["ExecutablePath"]
            buildingApplications[appName] = appExe

    if 'EventApplications' in registryData:
        eventApplicationData = registryData["EventApplications"]
        applicationsData = eventApplicationData["Applications"]

        for app in applicationsData:
            appName = app["Name"]
            appExe = app["ExecutablePath"]
            eventApplications[appName] = appExe

    if 'ModelingApplications' in registryData:
        modelingApplicationData = registryData["ModelingApplications"]
        applicationsData = modelingApplicationData["Applications"]

        for app in applicationsData:
            appName = app["Name"]
            appExe = app["ExecutablePath"]
            modelingApplications[appName] = appExe

    if 'EDP' in registryData:
        edpApplicationData = registryData["EDP"]
        applicationsData = edpApplicationData["Applications"]

        for app in applicationsData:
            appName = app["Name"]
            appExe = app["ExecutablePath"]
            edpApplications[appName] = appExe

    if 'SimulationApplications' in registryData:
        simApplicationData = registryData["SimulationApplications"]
        applicationsData = simApplicationData["Applications"]

        for app in applicationsData:
            appName = app["Name"]
            appExe = app["ExecutablePath"]
            simulationApplications[appName] = appExe

    if 'UQApplications' in registryData:
        uqApplicationData = registryData["UQApplications"]
        applicationsData = uqApplicationData["Applications"]

        for app in applicationsData:
            appName = app["Name"]
            appExe = app["ExecutablePath"]
            uqApplications[appName] = appExe

    if 'DamageAndLossApplications' in registryData:
        dlApplicationData = registryData["DamageAndLossApplications"]
        applicationsData = dlApplicationData["Applications"]

        for app in applicationsData:
            appName = app["Name"]
            appExe = app["ExecutablePath"]
            dlApplications[appName] = appExe

    #
    # open input file, and parse json into data
    #

    with open(inputFile, 'r') as data_file:    
        data = json.load(data_file)

    #
    # get all application data, quit if error
    #

    if 'WorkflowType' in data:
        typeWorkflow = data["WorkflowType"]
    else:
        print('Need a Workflow Type')
        quit()

    # check correct workflow type

    #
    # now we parse for the applications & app specific data in workflow
    #

    if 'Applications' in data:
        applications = data["Applications"]
    else:
        print('Need an Applications Entry')
        quit()

    #
    # get building application and its data
    #

    if 'Buildings' in applications:
        buildingApp = applications["Buildings"]

        if 'BuildingApplication' in buildingApp:
            buildingApplication = buildingApp["BuildingApplication"]

            # check building app in registry, if so get full executable path
            buildingAppData = buildingApp["ApplicationData"]
            if buildingApplication in buildingApplications.keys():
                buildingAppExe = buildingApplications.get(buildingApplication)
            else:
                print('Building application %s not in registry',buildingApplication)        
                quit()
        else:
            print('Need a Building Generator Application in Buildings')        
            quit()
    else:
        print('Need a Buildings Entry in Applications')
        quit()

    #
    # get events, for each the  application and its data .. FOR NOW 1 EVENT
    #

    if 'Events' in applications:
        events = applications["Events"]
        
        for event in events:
            if 'EventClassification' in event:
                eventClassification = event["EventClassification"]
                if eventClassification == 'Earthquake':
                    if 'EventApplication' in event:
                        eventApplication = event['EventApplication']
                        eventAppData = event["ApplicationData"]
                        eventData = event['ApplicationData']
                        if eventApplication in eventApplications.keys():
                            eventAppExe = eventApplications.get(eventApplication)
                        else:
                            print('Event application %s not in registry',eventApplication)        
                            quit()
                    else:
                        print('Need an EventApplication section')
                        quit()
            
                else:
                    print('Event classification must be Earthquake, not %s' % eventClassification)
                    quit()
            else:
                print('Need Event Classification')
                quit()
    else:
        print('Need an Events Entry in Applications')
        quit()

    #
    # get modeling application and its data
    #

    if 'Modeling' in applications:
        modelingApp = applications["Modeling"]

        if 'ModelingApplication' in modelingApp:
            modelingApplication = modelingApp["ModelingApplication"]

            # check modeling app in registry, if so get full executable path
            modelingAppData = modelingApp["ApplicationData"]
            if modelingApplication in modelingApplications.keys():
                modelingAppExe = modelingApplications.get(modelingApplication)
            else:
                print('Modeling application %s not in registry',modelingApplication)        
                quit()
        else:
            print('Need a ModelingApplication in Modeling data')        
            quit()

    else:
        print('Need a Modeling Entry in Applications')
        quit()


    #
    # get edp application and its data
    #

    if 'EDP' in applications:
        edpApp = applications["EDP"]

        if 'EDPApplication' in edpApp:
            edpApplication = edpApp["EDPApplication"]

            # check modeling app in registry, if so get full executable path
            edpAppData = edpApp["ApplicationData"]
            if edpApplication in edpApplications.keys():
                edpAppExe = edpApplications.get(edpApplication)
            else:
                print('EDP application %s not in registry',edpApplication)        
                quit()
        else:
            print('Need an EDPApplication in EDP data')        
            quit()

    else:
        print('Need an EDP Entry in Applications')
        quit()


    if 'Simulation' in applications:
        simulationApp = applications["Simulation"]

        if 'SimulationApplication' in simulationApp:
            simulationApplication = simulationApp["SimulationApplication"]

            # check modeling app in registry, if so get full executable path
            simAppData = simulationApp["ApplicationData"]
            if simulationApplication in simulationApplications.keys():
                simAppExe = simulationApplications.get(simulationApplication)
            else:
                print('Simulation application %s not in registry',simulationApplication)        
                quit()
        else:
            print('Need an SimulationApplication in Simulation data')        
            quit()

    else:
        print('Need a Simulation Entry in Applications')
        quit()


    if 'UQ-Simulation' in applications:
        uqApp = applications["UQ-Simulation"]

        if 'UQApplication' in uqApp:
            uqApplication = uqApp["UQApplication"]

            # check modeling app in registry, if so get full executable path
            uqAppData = uqApp["ApplicationData"]
            if uqApplication in uqApplications.keys():
                uqAppExe = uqApplications.get(uqApplication)
            else:
                print('UQ application %s not in registry',uqApplication)        
                quit()
        else:
            print('Need a UQApplication in UQ data')        
            quit()

    else:
        print('Need a Simulation Entry in Applications')
        quit()

    

    if 'Damage&Loss' in applications:
        DLApp = applications["Damage&Loss"]

        if 'Damage&LossApplication' in DLApp:
            dlApplication = DLApp["Damage&LossApplication"]

            # check modeling app in registry, if so get full executable path
            dlAppData = DLApp["ApplicationData"]
            if dlApplication in dlApplications.keys():
                dlAppExe = dlApplications.get(dlApplication)
            else:
                print('Dmage & Loss application %s not in registry',dlApplication)        
                quit()
        else:
            print('Need a Damage&LossApplicationApplication in Damage & Loss data')        
            quit()
    else:
        print('Need a Simulation Entry in Applications')
        quit()

    print('SUCCESS: Parsed workflow input')

    #
    # now invoke the applications
    #

    # 
    # put building generator application data into list and exe
    #

    buildingsFile = "buildings.json"
    buildingAppDataList = [buildingAppExe,buildingsFile]

    for key in buildingAppData.keys():
        buildingAppDataList.append("-"+key.encode('ascii', 'ignore'))
        buildingAppDataList.append(buildingAppData.get(key).encode('ascii', 'ignore'))

    buildingAppDataList.append('-getRV')
    subprocess.call(buildingAppDataList)

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
        id = building["id"]
        bimFILE = building["file"]
        eventFILE = id + "-EVENT.json"
        samFILE = id + "-SAM.json"
        edpFILE = id + "-EDP.json"
        dlFILE = id + "-DL.json"
        simFILE = id + "-SIM.json"
        driverFile = id + "-driver"

        # open driver file & write building app (minus the -getRV) to it
        driverFILE = open(driverFile,'w')
        del buildingAppDataList[-1]
        for item in buildingAppDataList:
            driverFILE.write("%s " % item)
        driverFILE.write("\n")

        # get RV for event
        eventAppDataList = [eventAppExe,'-filenameBIM',bimFILE,'-filenameEVENT',eventFILE]
        if(eventAppExe.endswith(".py")):
            eventAppDataList.insert(0, "python")

        for key in eventAppData.keys():
            eventAppDataList.append("-"+key.encode('ascii', 'ignore'))
            value = eventAppData.get(key)
            if(os.path.exists(value)):
                value = os.path.abspath(value)
            eventAppDataList.append(value.encode('ascii', 'ignore'))

        for item in eventAppDataList:
            driverFILE.write("%s " % item)
        driverFILE.write("\n")

        eventAppDataList.append('-getRV')
        subprocess.call(eventAppDataList)

        # get RV for building model
        modelAppDataList = [modelingAppExe,'-filenameBIM',bimFILE,'-filenameEVENT',eventFILE,'-filenameSAM',samFILE]

        for key in modelingAppData.keys():
            modelAppDataList.append("-"+key.encode('ascii', 'ignore'))
            modelAppDataList.append(modelingAppData.get(key).encode('ascii', 'ignore'))

        for item in modelAppDataList:
            driverFILE.write("%s " % item)
        driverFILE.write("\n")

        modelAppDataList.append('-getRV')
        subprocess.call(modelAppDataList)

        # get RV for EDP!
        edpAppDataList = [edpAppExe,'-filenameBIM',bimFILE,'-filenameEVENT',eventFILE,'-filenameSAM',samFILE,'-filenameEDP',edpFILE]

        for key in edpAppData.keys():
            edpAppDataList.append("-"+key.encode('ascii', 'ignore'))
            edpAppDataList.append(edpAppData.get(key).encode('ascii', 'ignore'))

        for item in edpAppDataList:
            driverFILE.write("%s " % item)
        driverFILE.write("\n")

        edpAppDataList.append('-getRV')
        subprocess.call(edpAppDataList)

        # get RV for Simulation
        simAppDataList = [simAppExe,'-filenameBIM',bimFILE,'-filenameSAM',samFILE,'-filenameEVENT',eventFILE,'-filenameEDP',edpFILE,'-filenameSIM',simFILE]

        for key in simAppData.keys():
            simAppDataList.append("-"+key.encode('ascii', 'ignore'))
            simAppDataList.append(simAppData.get(key).encode('ascii', 'ignore'))

        for item in simAppDataList:
            driverFILE.write("%s " % item)
        driverFILE.write("\n")

        simAppDataList.append('-getRV')
        subprocess.call(simAppDataList)

        # perform the simulation
        driverFILE.close()

        uqAppDataList = [uqAppExe,'-filenameBIM',bimFILE,'-filenameSAM',samFILE,'-filenameEVENT',eventFILE,'-filenameEDP',edpFILE,'-filenameSIM',simFILE,'driverFile',driverFile]

        for key in uqAppData.keys():
            uqAppDataList.append("-"+key.encode('ascii', 'ignore'))
            uqAppDataList.append(simAppData.get(key).encode('ascii', 'ignore'))

        print(uqAppDataList)

        subprocess.call(uqAppDataList)

        # compute damage and oss
        dlAppDataList = [dlAppExe,'-filenameBIM',bimFILE,'-filenameEDP',edpFILE,'-filenameLOSS',dlFILE]

        for key in dlAppData.keys():
            dlAppDataList.append("-"+key.encode('ascii', 'ignore'))
            dlAppDataList.append(dlAppData.get(key).encode('ascii', 'ignore'))

        print(dlAppDataList)
        subprocess.call(dlAppDataList)


if __name__ == "__main__":
    main()
