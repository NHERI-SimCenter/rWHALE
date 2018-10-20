[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1442914.svg)](https://doi.org/10.5281/zenodo.1442914)

# Regional Earthquake Simulation Workflow

The regional earthquake simulation workflow is a set of applications that together can estimate damage and loss based on a buildings database and hazard information for a region. The workflow goes through the following five main processes to obtain damage and loss estimates for each building in the region:

1. Obtain the Building Information Model (BIM).

2. Obtain the hazard event input on the building.

3. Produce a Structural Analysis Model (SAM) of the building.

4. Define and calculate the Engineering Demand Parameters (EDP) for the building model.

5. Estimate the damage and loss.

In addition to these main processes, the following five intermediate files are defined and are used in the workflow as intermediate interfaces between the processes:

1. **BIM** file: A file containing general information about the building.

2. **EVENT** file: A file containing information about the hazard events input for the building.

3. **SAM** file: A description of the structural analysis model including geometry, structural and material properties.

4. **EDP** file: a file containing the definition and values of the engineering demand parameters (EDPs).

5. **DL** file: a file containing the obtained damage and loss estimates for a building.

In this framework for regional simulation, the JSON format is adopted for all the intermediate files defined above.  The implementations of the workflow applications provided in this repository are organized based on the previous 5 main processes, categorized based on their purpose, in the following sub-directories:

* **[createBIM](./createBIM/Readme.md)**: This directory contains applications that can obtain a Building Information Model (BIM) from a database of buildings. Some examples of these applications are:

  * **UrbanSimDatabase**: This application generates BIM models given UrbanSim buildings and parcels csv files.

  * **GenericBimDatabase**: This application generates BIM models given a generic database in csv format with a minimal set of information about each building.

  More detailed documentation for the createBIM applications is available [here](./createBIM/Readme.md)

* **createEVENT**: This directory contains applications to generate hazard event input given BIM.

  * **LLNL-SW4**: This application will read the building location from the BIM model and will output ground motion for simulating building response. The application uses a set of pre-computed ground motions for SF Bay Area earthquake event (Magnitude 7.0 earthquake at the Hayward fault) simulated using [SW4](https://geodynamics.org/cig/software/sw4/).

  * **SHA-GM** [Experimental]: This application combines seismic hazard analysis (SHA) and ground motion record selection/scaling to provide ground motions for structural analysis based on a user-defined earthquake scenarios.

* **createSAM**:  This directory contains applications to create structural analysis model (SAM) given BIM and Event data

  * **MDOF_LU**:  This application use Hazus data to generate a MDOF non-linear shear building model.

  * **AI-M-3** [Under Development]: This is an application that is currently under development that uses artificial intelligence to obtain SAM from BIM.

* **createEDP**: This directory contains applications that generates a list of the required Engineering Demand Parameters (EDPs) to be obtained from structural analysis.
  * **StandardEarthquakeEDP**: This application uses the SAM to define the EDPs needed for damage and loss calculations due to an earthquake event.

* **performSIMULATION**: applications in this directory will perform preprocessing of SAM, structural analysis and post processing to obtain EDP values.

  * **OpenSeesSimulation.sh**: This application will pre-process the SAM file to create an OpenSees model, run the model and post-process OpenSees outputs to update the EDP values.

* **performUQ**: This directory contains scripts and applications to introduce uncertainties in the workflow applications. Current implementation relies on DAKOTA for handling uncertainties.

  * **Dakota-FEM-Simulation.sh**: This implementation of performUQ relies on DAKOTA for handling uncertainties.

* **createLOSS**:  Applications in this directory are used to determine damage and loss.

  * **FEMA_P58_LU**: Damage and loss assessment tool that uses FEMA-P58 methodology. This tool was developed by Prof. Xinzheng Lu's research group at Tsinghua University [1].

  * **OpenPerform** [Under development]: an efficient object-oriented damage and loss assessment python tool using FEMA-P58 methodology is currently under development.

* **Workflow**: This directory contains scripts and examples to run the workflow with different configurations.

Other directories included in this repository are:

* **include**: a directory containing header files or header-only libraries used by the workflow applications.

* **utils**: a directory containing utility applications which are not an essential part of the workflow but can facilitate using some of the workflow applications.

## Dependencies

* **Jansson Library**: Many of the provided applications require the [Jansson library](http://www.digip.org/jansson/) to be installed. Jansson is a free native C library for encoding, decoding and manipulating JSON data. It is licensed under the MIT license. For Unix systems, it is assumed to be installed in /usr/local/jansson as seen in the included Makefiles.

* **C++11 compliant compiler**: Some applications in the createLOSS and performUQ folders use C++11 features, consequently they may need a newer C++11 compliant compiler.

* **OpenSees**: The workflow applications require an installation of [OpenSees](http://opensees.berkeley.edu/) to carry out structural analysis using the finite element method.

* **DAKOTA**: The workflow applications require an installation of [DAKOTA](https://dakota.sandia.gov/) to handle and propagate the uncertainties defined in the input files for the workflow applications.

* **Python**: The workflow requires Python 2.7.

* **Perl**: The workflow makes use of a modified version of the `DPrePro` preprocessor Perl script distributed with DAKOTA, and thus requires Perl. Later releases may switch to the newer python version of the script as the Perl script is deprecated as of DAKOTA 6.8.

*	**nanoflann**: A C++11 library that performs nearest neighbor search using k-dimensional trees. nanoflann is a header-only library and is included with the source of the workflow. It is currently being used by LLNL_SW4 createEVENT application.

* **Simcenter-EQSS**: the ground motion tools developed at the SimCenter are needed, if using SHA-GM.py as a createEVENT application. To obtain and build these applications please refer to [SimCenter-EQSS](https://github.com/el7addad/Simcenter-EQSS) repository for documentation.

## Building the source code on Unix-like systems

Before building the workflow, the following dependencies will need to be installed:

1. [GNU Compiler Collection](https://gcc.gnu.org/) (gcc & g++ ) version 4.8.1 or newer.
2. [GNU Make](https://www.gnu.org/software/make/).
3. [Jansson Library](http://www.digip.org/jansson/).

The repository uses Makefiles to build the applications.
To build all the workflow applications, use ```make``` or ```make all``` in the root directory. To build for debugging use ```make debug```  and to clean, use ```make clean```.

## Building the source code on Ubuntu Linux

Before building the source code on Ubuntu Linux and running it, the dependencies can be installed using the following commands:

```shell
sudo apt-get update
sudo apt-get install gcc g++ python perl
```

After installing the dependencies, building the workflow applications can then be done similar to other unix systems, using `make all`. Running the workflow applications will require installing other dependencies like OpenSees and DAKOTA. Instructions to building these software applications from their source code on Ubuntu Linux are available on their respective websites.

## Building the source code on Windows

Visual studio projects is under development. Currently the workflow can be built and run in Windows using the Windows subsystem for Linux and building using the same commands for Linux systems.

## Data

Some data files required to run the workflow are not included in this repository, and are distributed using the [SimCenter Box account](https://berkeley.box.com/s/7es601ve766fprph88n67khfip7fqupd). If you cannot access the data, please contact the SimCenter using [Slack](https://designsafe-ci.slack.com/messages/C92HT3GG4) or by [email](nheri-simcenter@berkeley.edu).

* **BIM Datasets**: The workflow is distributed with a sample UrbanSim data to run a sample number of buildings, e.g. sample buildings.csv and parcels.csv files for UrbanSimDatabase with 100 buildings. Data for SF Bay Area is available only for the GenericBimDatabase application and can be obtained from the SimCenter Box account.

* **Ground Motion**: files for the LLNL Hayward 7.0 scenario are available through the SimCenter Box account.

## Pegasus Workflow

The Pegasus workflow can be found in the [Pegasus](./Pegasus) directory.

Before submitting workflow, please run `make` to make sure all the executables
required by the workflow have been built.

The workflow is set up to execute in two different execution
environments: local HTCondor pool, Stampede 2 or TACC Wrangler. The first
is good for small to medium sized runs as turnaround time is usually
pretty good. Large workflows should be mapped to TACC Wrangler.

Submitting a workflow for local HTCondor pool execution can be done
by executing:

```shell
    ./submit-workflow
```

Submitting to TACC Stampede2 can be done on the workflow.isi.edu host with the command:

```shell
    ./submit-workflow stampede2
    ./submit-workflow wrangler
```

## Testing Workflow Components

If you wish to try out existing workflow components with new parameters, or to
develop new components, you may wish to consider using the 'test harness' provided in this repository.
See the [README](./tests/README.md) in the ``tests`` directory for details.

## Disclaimer

NHERI SimCenter provides the source code of the regional earthquake simulation workflow as-is and makes no representations or warranties of any kind concerning the source code and the results obtained from them, whether expressed, implied, statutory, or other. This includes, without limitation, warranties of title, merchantability, fitness for a particular purpose, non-infringement, absence of latent or other defects, accuracy, or the presence or absence of errors, whether or not known or discoverable.

## Acknowledgements

The SimCenter would like to acknowledge Prof. Jack Baker for his discussions about seismic hazard analysis, ground motion spatial correlation, selection and scaling of ground motion records, in addition to sharing Matlab codes developed by his research group to simulate spatially-correlated ground motions. The SimCenter would also like to acknowledge the contributions of OpenSHA, a library used by the hazard tool documented herein.
We thank the Computational Infrastructure for [Geodynamics](http://geodynamics.org) which is funded by the National Science Foundation under awards EAR-0949446 and EAR-1550901.
The SimCenter is supported by a grant from the National Science Foundation (NSF) under cooperative agreement CMMI 1612843. Materials in this document do not necessarily represent the views of NSF.

## References

[1] Zeng X., Lu X.Z., Yang T., Xu Z., "Application of the FEMA-P58 methodology for regional earthquake loss prediction", Natural Hazards (2016), 10.1007/s11069-016-2307-z