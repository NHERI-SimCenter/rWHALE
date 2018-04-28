import json
from pprint import pprint
import sys

numSamples = 5;

bimName=sys.argv[1];#'exampleBIM.json'
evtName=sys.argv[2];#'exampleEVENT.json'
samName=sys.argv[3];#'exampleSAM.json'
edpName=sys.argv[4];#'exampleEDP.json'
simName=sys.argv[5];#'exampleSIMULATION.json'
driverFile = sys.argv[6]
scriptDIR = sys.argv[7]

#workflowDIR=sys.argv[6];#'/Users/simcenter/NHERI/Workflow1.1/'

#later
#dakotaName=sys.argv[7]
#numSamples=sys.argv[8];

# 
# parse the data
#

numRandomVariables = 0;
numNormalUncertain = 0;
normalUncertainName=[];
normalUncertainMean =[];
normalUncertainStdDev =[];

numDiscreteDesignSetString = 0;
discreteDesignSetStringName=[];
discreteDesignSetStringValues =[];

#with open('data.txt', 'w') as outfile:  
#    json.dump(data, outfile)
#print data["method"]

def length(b):
    count=0
    for j in b:
        count += 1
    return count

def parseFileForRV(fileName):
    global numRandomVariables;
    global numNormalUncertain;
    global normalUncertainName;
    global normalUncertainMean;
    global normalUncertainStdDev;

    global numDiscreteDesignSetString;
    global normalDiscreteDesignSetName;
    global normalDiscreteSetValues;

    with open(fileName,'r') as data_file:    
        data = json.load(data_file)

        for k in data["RandomVariables"]:
            if (k["distribution"] == "normal"):
                normalUncertainName.append(k["name"])
                normalUncertainMean.append(k["mean"])
                normalUncertainStdDev.append(k["stdDev"])
                numNormalUncertain += 1
                numRandomVariables += 1

            if (k["distribution"] == "discrete_design_set_string"):
                print k
                discreteDesignSetStringName.append(k["name"])
                elements =[];
                for l in k["elements"]:
                    elements.append(l)
                elements.sort()
                discreteDesignSetStringValues.append(elements)
                print elements
                numDiscreteDesignSetString += 1
                numRandomVariables += 1


parseFileForRV(bimName)
parseFileForRV(evtName)
parseFileForRV(samName)
#parseFileForRV(simName)
parseFileForRV(edpName)

#
# Write the input file: dakota.in 
#

# write out the method data
f = open('dakota.in', 'w')

f.write("environment\n")
f.write("tabular_data\n")
f.write("tabular_data_file = \'dakotaTab.out\'\n\n")

f.write("method\n")
f.write("sampling,\n")
f.write('samples=' '{}'.format(numSamples))
#f.write("samples=5\n")
f.write("\nseed=98765,\n")
f.write("sample_type random\n")
f.write('\n\n')

# write out the variable data
f.write('variables,\n')

if (numNormalUncertain > 0):
    f.write('normal_uncertain = ' '{}'.format(numNormalUncertain))
    f.write('\n')
    f.write('means = ')
    for i in xrange(numNormalUncertain):
        f.write('{}'.format(normalUncertainMean[i]))
        f.write(' ')
    f.write('\n')

    f.write('std_deviations = ')
    for i in xrange(numNormalUncertain):
        f.write('{}'.format(normalUncertainStdDev[i]))
        f.write(' ')
    f.write('\n')

    f.write('descriptors = ')    
    for i in xrange(numNormalUncertain):
        f.write('\'')
        f.write(normalUncertainName[i])
        f.write('\' ')
    f.write('\n')

if (numDiscreteDesignSetString > 0):
    f.write('discrete_design_set\n')
    f.write('string ' '{}'.format(numDiscreteDesignSetString))
    f.write('\n')
    f.write('descriptors = ')    
    for i in xrange(numDiscreteDesignSetString):
        f.write('\'')
        f.write(discreteDesignSetStringName[i])
        f.write('\' ')

    f.write('\n')

    f.write('elements_per_variable = ')    
    for i in xrange(numDiscreteDesignSetString):
        #f.write('\'')
        numElements = length(discreteDesignSetStringValues[i])
        f.write(' ' '{}'.format(numElements))
        #f.write(length(discreteDesignSetStringValues[i]))
        print discreteDesignSetStringValues[i]
        print numElements
        #f.write('\' ')

    f.write('\n')
    f.write('elements  ')    
    for i in xrange(numDiscreteDesignSetString):
        elements = discreteDesignSetStringValues[i]
        for j in elements:
            f.write('\'' '{}'.format(j))
            f.write('\' ')
        f.write('\n')

f.write('\n\n')

# write out the interface data
f.write('interface,\n')
f.write('system # asynch evaluation_concurrency = 4\n')
f.write('analysis_driver = \'workflow_driver\' \n')
f.write('parameters_file = \'params.in\' \n')
f.write('results_file = \'results.out\' \n')
f.write('work_directory directory_tag \n')
f.write('copy_files = \'templatedir/*\' \n')
f.write('named \'workdir\' file_save  directory_save \n')
f.write('aprepro \n')
f.write('\n')

#
# write out the responses
#

with open(edpName) as data_file:    
    data = json.load(data_file)

numResponses=data["total_number_edp"]

f.write('responses, \n')
f.write('response_functions = ' '{}'.format(numResponses))
f.write('\n')
f.write('response_descriptors = ')    
#for i in xrange(numResponses):
for i in xrange(1, numResponses+1):
    f.write('\'a')
    f.write('{}'.format(i))
    f.write('\' ')
f.write('\n')
f.write('no_gradients\n')
f.write('no_hessians\n\n')
f.close()  # you can omit in most cases as the destructor will call it

#
# Write the workflow driver
#

f = open('workflow_driver', 'w')

# want to dprepro the files with the random variables
f.write('dpreproSimCenter $1 bim.j ' + bimName + '\n')
f.write('dpreproSimCenter $1 sam.j ' + samName + '\n')
f.write('dpreproSimCenter $1 evt.j ' + evtName + '\n')
f.write('dpreproSimCenter $1 edp.j ' + edpName + '\n')

with open(driverFile) as fp:
    for line in fp:
        f.write(line)
        print(line)

f.write('\n')
f.write(scriptDIR + '/extractEDP ' + edpName + ' results.out \n')

# Run 
#f.write('rm -f *.com *.done *.dat *.log *.sta *.msg')
#f.write('echo 1 >> results.out\n')
f.close()

f = open('finishUP.sh', 'w')
f.write('#!/bin/bash\n')
f.write(scriptDIR + '/postprocessDakota ' + str(numRandomVariables) + ' ' + str(numSamples) + ' ' +  edpName + ' dakotaTab.out \n')
f.write('rm -fr templatedir workdir.* dakota.* LHS* dakotaTab.*')

f.close();
