from shutil import copyfile

inputFile='HFmeta';
lineCount=0;
with open(inputFile) as f:
    for line in f:
#       if (lineCount > 0 and lineCount < 10):
       if (lineCount > 0):
           lineList=line.split(" ");
           if (lineList[0] != "S_30_20") :
               print(lineList[0])
               copyfile("S_30_20.json",lineList[0]+".json")
       lineCount += 1;


