
import json
import os
import sys
import subprocess
import hashlib
from scipy import spatial
import glob
import re
import argparse
from textwrap import wrap

def ReadSMC(smcFilePath):
    with open(smcFilePath, 'r+') as smcFile:
        series = []
        smcLines = smcFile.readlines()
        dT = 1.0/float(smcLines[17].strip().split()[1])
        nCommentLines = int(smcLines[12].strip().split()[7])
        for line in smcLines[(27+nCommentLines):]:
            for value in wrap(line, 10, drop_whitespace=False):
                if value.strip():
                    series.append(float(value)/100.0)

        return [series, dT]

def ReadCOSMOS(cosmosFilePath):
    with open(cosmosFilePath, 'r+') as cosmosFile:
        series = []
        cosmosLines = cosmosFile.readlines()
        headerSize = int(cosmosLines[0][46:48])
        intSize = int(cosmosLines[headerSize][37:40])
        realSize = int(cosmosLines[headerSize + intSize + 1][34:37])
        commentSize = int(cosmosLines[headerSize + intSize + realSize + 2][0:4])
        totalHeader = headerSize + intSize + realSize + commentSize + 3
        recordSize = int(cosmosLines[totalHeader].strip().split()[0])
        dT = float(cosmosLines[37].strip().split()[1])/1000.0

        for line in cosmosLines[totalHeader + 1:totalHeader + recordSize + 1]:
            series.append(float(line.strip())/100.0)

        return [series, dT]

def createEvent(recordsFolder, h1File, h2File, eventFilePath):

    if h1File.endswith(".smc"):
        h1, dt1 = ReadSMC(os.path.join(recordsFolder, h1File))
    else:
        h1, dt1 = ReadCOSMOS(os.path.join(recordsFolder, h1File))

    if h2File.endswith(".smc"):
        h2, dt2 = ReadSMC(os.path.join(recordsFolder, h2File))
    else:
        h2, dt2 = ReadCOSMOS(os.path.join(recordsFolder, h2File))

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
    event["name"] = h1File
    event["type"] = "Seismic" 
    event["description"] = h1File 
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
    gmArgsParser.add_argument("-groundMotions", required=True, help="Path to the ground motions configuration file")
    gmArgsParser.add_argument("-recordsFolder", required=True, help="Path to the ground motions records folder")
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
    gmConfigPath = gmArgs.groundMotions
    recordsFolder = gmArgs.recordsFolder
 
    with open(gmConfigPath, 'r') as gmConfigFile:
        gmConfig = json.load(gmConfigFile)
        
    #We need to read the building location
    with open(bimFilePath, 'r') as bimFile:
        bim = json.load(bimFile)
        location = [bim["GI"]["location"]["latitude"], bim["GI"]["location"]["longitude"]]

    siteLocations = []
    for gm in gmConfig["GroundMotion"]:
        siteLocations.append([gm["Location"]["Latitude"], gm["Location"]["Longitude"]])

    # we need to find the nearest neighbor
    sitesTree = spatial.KDTree(siteLocations)

    nearest = sitesTree.query(location)
    nearestGM = gmConfig["GroundMotion"][nearest[1]]
    h1File = nearestGM["Records"]["Horizontal1"]
    h2File = nearestGM["Records"]["Horizontal2"]
    
    createEvent(os.path.abspath(recordsFolder), h1File, h2File, eventFilePath)

if __name__== "__main__":
    main()