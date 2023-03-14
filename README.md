# VEINS_Connected_Vehicles

## Getting Started
This project is intended for Linux because it relies on several open source projects.
 - SUMO v1.15.0 is used to define road geometry, routes, vehicle flows, sensors, traffic lights, emissions models, and driver behaviors. You can install SUMO from this page: https://sumo.dlr.de/docs/Installing/index.html
 - OMNeT++ v5.7 is a network simulator based on C++. In addition to a C++ framework, the tool also provides NED (NEtwork Description language) to create network topologies. The community has modeled various IEEE network stacks for simulating communications. OMNeT++ also provides an IDE that I have used for development of this project. The install guide is here:
 https://doc.omnetpp.org/omnetpp/InstallGuide.pdf
 - VEINS v5.2 is an OMNeT++ project that serves as the connection between OMNeT++ and SUMO. The project comes with a predefined OMNeT++ modules that model the 802.11p stack for DSRC. It provides the tools to control a SUMO simulation and assign OMNeT++ classes to vehicles, traffic lights, and other SUMO objects, providing these objects with network functionality. Assuming SUMO and OMNeT++ are functional, you simply download the project from https://veins.car2x.org/download/ and import the project into the OMNeT++ IDE. If you need additional guidance, see the install guide https://veins.car2x.org/tutorial/.
 - Python 3 is used to write the machine learning script. I used the Miniconda package manager to install my Python dependencies. You may use the provided "environment.yml" file to install the necessary dependencies with the command `conda env create -f environment.yml`, then activate the environment with `conda activate pytorch`.
 
## Other dependencies
libprotobuf17: `sudo apt install libprotobuf-dev`
zmq: `sudo apt install libzmqpp-dev`

## Opening the project
Open OMNeT++ IDE. Ensure the veins-5.2 project has been imported into the IDE, if not check the Veins install guide on their website. Import the SmartHighway project into your OMNeT++ IDE workspace by clicking `File > Import > General: Existing Projects into Workspace` and selecting the smartHighway/ directory in this repo. At this point, you can develop C++ code for the simulation. 

Before compiling the smartHighway project, I have made some changes to the VEINS project that need to be updated. Copy the files in `modified_veins_files/veins/src/veins/modules/mobility/traci/*` to the corresponding `veins-veins-5.2/veins/src/modules/mobility/traci/` directory in your VEINS installation folder.

In the OMNeT++ IDE, navigate to smartHighway/simulations/interstate_5_reduced/omnetpp.ini, right-click this file and click Run. If everything has been installed correctly, the compilation process should begin and a new window should pop up with a GUI view of the simulation. You can exit from this view now, it won't work yet until it establishes connections to the Python scripts described below.

Then, start the SUMO TRaCI connection service; open another terminal, activate the `pytorch` conda environment, navigate to the modified_veins_files/bin folder and run `./veins_launchd -v`.

Next, start the machine learning script, open a terminal, activate the `pytorch` conda environment, and run `python learning.py`.

## Software Overview
![Software Overview](https://github.com/AICPS/SmartCorridor/blob/main/Software%20Flow.jpg)

## Useful information for traffic light development:
 - https://stackoverflow.com/questions/53693016/create-traffic-light-in-sumo-and-omnet
 - https://doc.omnetpp.org/omnetpp/manual/#sec:ned-lang:parameters
 - http://cse.iitkgp.ac.in/~soumya/micro/t2-3.pdf
 - http://cse.iitkgp.ac.in/~soumya/micro/t2-7.pdf
