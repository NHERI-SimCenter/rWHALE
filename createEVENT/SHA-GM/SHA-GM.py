#This python script process the input and will use it to run SHA and ground motion selection
#In addition to providing the event file

import json
import os
import sys
import subprocess
import hashlib
from scipy import spatial
import glob
import re
import argparse

def computeScenario(scenarioConfig, scenarioHash, seed):
    eqHazardPath = os.path.abspath(scenarioConfig["AppConfig"]["EQHazardPath"])
    simulateIMPath = os.path.abspath(scenarioConfig["AppConfig"]["SimulateIMPath"])
    selectRecordPath = os.path.abspath(scenarioConfig["AppConfig"]["SelectRecordPath"])

    #We no longer need the app config
    del scenarioConfig["AppConfig"]

    #Separate Selection Config
    selectionConfig = scenarioConfig["RecordSelection"]
    del scenarioConfig["RecordSelection"]

    #Adding the required output
    scenarioConfig["IntensityMeasure"]["EnableJsonOutput"] = True
    with open("./HazardCache/Hazard_Scenario.json", 'w') as hazardFile:
        json.dump(scenarioConfig, hazardFile,  indent=4)
    
    #Now we need to run the EQHazard Process
    hazardCommand = ["java", "-jar", eqHazardPath, "./HazardCache/Hazard_Scenario.json", "./HazardCache/Hazard_Output.json"]
    hazardResult = subprocess.call(hazardCommand)

    if(hazardResult != 0):
        sys.stderr.write("Hazard analysis failed!")
        return -1

    #Now we need to run the SimulateIM Process
    #First we create a simulation config
    simConfig = {
        "GroundMotions":{
            "File": "./HazardCache/Hazard_Output.json"
            }, 
        "NumSimulations": 1,
        "SpatialCorrelation": True,
        "Seed": seed
    }

    with open("./HazardCache/Sim_Config.json", 'w') as simConfigFile:
        json.dump(simConfig, simConfigFile,  indent=4)
    simulateCommand = [simulateIMPath, "./HazardCache/Sim_Config.json", "./HazardCache/Hazard_Sim.json"]
    simResult = subprocess.call(simulateCommand)

    if(simResult != 0):
        sys.stderr.write("Intensity measure simulation failed!")
        return -2
    
    #Now we can run record selection
    #
    selectionConfig["Target"]["File"] = "./HazardCache/Hazard_Sim.json"
    selectionConfig["Database"]["File"] = os.path.abspath(selectionConfig["Database"]["File"])
    with open("./HazardCache/Selection_Config.json", 'w') as selectionConfigFile:
        json.dump(selectionConfig, selectionConfigFile,  indent=4)
    selectionCommand = [selectRecordPath, "./HazardCache/Selection_Config.json", "./HazardCache/Records_Selection.json"]
    simResult = subprocess.call(selectionCommand)

    if(simResult != 0):
        sys.stderr.write("Intensity measure simulation failed!")
        return -2

    with open("./HazardCache/hash", 'w') as hashFile:
        hashFile.write(scenarioHash)

def readNGAWest2File(ngaW2FilePath, scaleFactor):
    series = []
    dt = 0.0
    with open(ngaW2FilePath, 'r') as recordFile:
        canRead = False #We need to process the header first
        for line in recordFile:
            if(canRead):
                series.extend([float(value) * scaleFactor * 9.81 for value in line.split()])

            elif("NPTS=" in line):
                dt = float(re.match(r"NPTS=.+, DT=\s+([0-9\.]+)\s+SEC", line).group(1))
                canRead = True
            

    return series, dt

def createNGAWest2Event(rsn, scaleFactor, recordsFolder, eventFilePath):
    pattern = os.path.join(recordsFolder, "RSN") + str(rsn)  +"_*.AT2"
    recordFiles = glob.glob(pattern)
    if(len(recordFiles) != 2):
        print "Error finding NGA West 2 files"
    h1, dt1 = readNGAWest2File(recordFiles[0], scaleFactor)
    h2, dt2 = readNGAWest2File(recordFiles[1], scaleFactor)

    patternH1 = {}
    patternH1["type"] = "UniformAcceleration"
    patternH1["timeSeries"] = "accel_X"
    patternH1["dof"] = 1

    patternH2 = {}
    patternH2["type"] = "UniformAcceleration"
    patternH2["timeSeries"] = "accel_Y"
    patternH2["dof"] = 2

    seriesH1 = {}
    seriesH1["name"] = "accel_X"
    seriesH1["type"] = "Value"
    seriesH1["dT"] = dt1
    seriesH1["data"] = h1

    seriesH2 = {}
    seriesH2["name"] = "accel_Y"
    seriesH2["type"] = "Value"
    seriesH2["dT"] = dt2
    seriesH2["data"] = h2

    event = {}
    event["name"] = "NGAW2_" + str(rsn)
    event["type"] = "Seismic" 
    event["description"] = "NGA West 2 record " + str(rsn) + " scaled by a factor of " + str(scaleFactor) 
    event["dT"] = dt1
    event["numSteps"] = len(h1)
    event["timeSeries"] = [seriesH1, seriesH2]
    event["pattern"] = [patternH1, patternH2]
    
    eventsDict = {}
    eventsDict["Events"] = [event]
    eventsDict["RandomVariables"] = []

    with open(eventFilePath, 'w') as eventFile:
        json.dump(eventsDict, eventFile,  indent=4)


def main():
    #Input Argument Specifications
    gmArgsParser = argparse.ArgumentParser("Characterize ground motion using seismic hazard analysis and record selection")
    gmArgsParser.add_argument("-filenameBIM", required=True, help="Path to the BIM file")
    gmArgsParser.add_argument("-filenameEVENT", required=True, help="Path to the EVENT file")
    gmArgsParser.add_argument("-scenarioConfig", required=True, help="Path to the earthquake scenario configuration file")
    gmArgsParser.add_argument("-seed", type=int, default=1, help="Seed for random number generation")
    gmArgsParser.add_argument("-getRV", action='store_true', help="Flag showing whether or not this call is to get the random variables definition")
    
    #Parse the arguments
    gmArgs = gmArgsParser.parse_args()


    #Check getRV flag
    if not gmArgs.getRV:
        #We will use the template files so no changes are needed
        #We do not have any random variables for this event for now
        return 0

    #First let's process the arguments
    bimFilePath = gmArgs.filenameBIM
    eventFilePath = gmArgs.filenameEVENT
    scenarioConfigPath = gmArgs.scenarioConfig

    # if "-getRV" in inputArgs:
    #     #We will create an output that only contains empty random variables array
    #     with open(eventFilePath, 'w') as eventFile:
    #         randomVariables = {"RandomVariables":[]}
    #         json.dump(randomVariables, eventFile,  indent=4)

    #     return 0

    #Ensure a hazard cache folder exist
    if not os.path.exists("./HazardCache"):
        os.mkdir("./HazardCache")

    #TODO: we need to hash the hazard cache use that hash to check if computation is needed
    needsCompute = True 
    with open(scenarioConfigPath, 'r') as scenarioConfigFile:
        scenarioConfig = json.load(scenarioConfigFile)
        scenarioConfigFile.seek(0)
        scenarioHash = hashlib.md5(scenarioConfigFile.read()).hexdigest()
        if(os.path.exists("./HazardCache/hash")):
            with open("./HazardCache/hash", 'r') as hashFile:
                if(hashFile.read() == scenarioHash):
                    needsCompute = False
                else:
                    print("Scenario is changed and will require recomputation")
    
    recordsFolder = os.path.abspath(scenarioConfig["AppConfig"]["RecordsFolder"])

    if(needsCompute):
        computeScenario(scenarioConfig, scenarioHash, gmArgs.seed)

    
    #We need to read the building location
    with open(bimFilePath, 'r') as bimFile:
        bim = json.load(bimFile)
        location = [bim["GI"]["location"]["latitude"], bim["GI"]["location"]["longitude"]]

    #Now we can start processing the event
    with open("./HazardCache/Records_Selection.json", 'r') as selectionFile:
        recordSelection = json.load(selectionFile)

    with open("./HazardCache/Hazard_Output.json", 'r') as hazardOutputFile:
        hazardOutput = json.load(hazardOutputFile)

    siteLocations = []
    for gm in hazardOutput["GroundMotions"]:
        siteLocations.append([gm["Location"]["Latitude"], gm["Location"]["Longitude"]])

    # we need to find the nearest neighbor
    sitesTree = spatial.KDTree(siteLocations)

    nearest = sitesTree.query(location)
    selectedRecord = recordSelection["GroundMotions"][nearest[1]]
    rsn = selectedRecord["Record"]["Id"]
    scaleFactor = selectedRecord["ScaleFactor"]
    
    createNGAWest2Event(rsn, scaleFactor, recordsFolder, eventFilePath)

if __name__== "__main__":
    main()