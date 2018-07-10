#!/usr/bin/env python

import glob
import re
import os
import sys
import obspy
import fileinput
import json
import numpy 


class MyEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, numpy.integer):
            return int(obj)
        elif isinstance(obj, numpy.floating):
            return float(obj)
        elif isinstance(obj, numpy.ndarray):
            return obj.tolist()
        else:
            return super(MyEncoder, self).default(obj)


def processaxisfile(station):
    a = []
    for i in range(0, len(stn[0].data)-1):
        a.append((stn[0].data[i+1]-stn[0].data[i])/float(stn[0].stats['delta']))
    return a

def processStationRecord(record):

    fileroot = record

    #check that x,y,z for the file root exists
    if ( not os.path.isfile(fileroot+'.x') or not os.path.isfile(fileroot+'.y') or not os.path.isfile(fileroot+'.z') ):
        print( "Missing files for axis.")
    
    mainDict = {}
    mainDict['type'] = 'Earthquake'
    mainDict['subtype'] = 'UniformAcceleration'
    mainDict['name'] = ''
    
    eventList = {}
    eventList1 = []
    
    # read and process the x axis, then add to list
    stn = obspy.read( fileroot + '.x' )
    mainDict['dT'] = stn[0].stats['delta']
    mainDict['data_x'] = processaxisfile(stn)

    stn = obspy.read( fileroot + '.y' )
    mainDict['data_y'] = processaxisfile(stn)

    # read and process the z axis, then add to list
    stn = obspy.read( fileroot + '.z' )
    #print(stn[0].stats)
    #eventList.append( processaxisfile(stn) )
    mainDict['data_z'] = processaxisfile(stn)

    print('list size: ' + str(len(eventList)) )

    # package all three axis into one file with multiple events
    eventList1.append(mainDict)
    #mainDict['Events'] = eventList
    eventList['Events'] = eventList1

    # write results
    with open(fileroot + '.json', 'w') as f:
        json.dump(eventList, f ,  cls=MyEncoder)


#
# main part of code
#

inputFile='HFmeta';
lineCount=0;
with open(inputFile) as f:
    for line in f:
       if (lineCount > 1):
           lineList=line.split(" ");
           station = lineList[0]
           stn = obspy.read( station + '.x' )
           processStationRecord(station)
           print(station)
       lineCount += 1;



