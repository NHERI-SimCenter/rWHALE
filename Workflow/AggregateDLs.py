import os
import csv
import sys

DLFolder = sys.argv[1]
aggregateFile = sys.argv[2]

DLSubfolders = os.listdir(DLFolder)
DLSubfolders.sort()

with open(aggregateFile, 'wb') as aggregateDL:
    dlwriter = csv.writer(aggregateDL, delimiter=',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
    dlwriter.writerow("Id,MedianRepairCost,RepairCostStdDev,MedianDowntime,RedTagged,PGA,LossRatio,Latitude,Longitude".split(','))
    for dlSubfolder in DLSubfolders:
        DLSubfolderPath = os.path.join(DLFolder, dlSubfolder)
        dlfiles = os.listdir(DLSubfolderPath)
        dlfiles.sort(key=lambda f: int(f.split('-')[0][3:]))
        for dlfile in dlfiles:
            dlfilePath = os.path.join(DLSubfolderPath, dlfile)
            with open(dlfilePath, 'rb') as dlcsvfile:
                dlfileReader = csv.reader(dlcsvfile, delimiter = ',')
                dlfileReader.next()
                for row in dlfileReader:
                    dlwriter.writerow(row)