# VEINS_Connected_Vehicles

## Getting Started
This project is intended for Linux because it relies on several open source projects.
 - SUMO v1.15.0 is used to define road geometry, routes, vehicle flows, and simulate driver behaviors. You can install SUMO from this page: https://sumo.dlr.de/docs/Installing/index.html
 - OMNeT++ v5.7 is a network simulator based on C++. In addition to a C++ framework, the tool also provides NED (NEtwork Description language) to create network topologies. The community has modeled various IEEE network stacks for simulating communications. OMNeT++ also provides an IDE that I have used for development of this project. The install guide is here:
 https://doc.omnetpp.org/omnetpp/InstallGuide.pdf
 - VEINS v5.2 is an OMNeT++ project that serves as the connection between OMNeT++ and SUMO. It provides the tools to control a SUMO simulation and assign OMNeT++ classes to vehicles, traffic lights, and other SUMO objects, providing these objects with network functionality. Assuming SUMO and OMNeT++ are functional, you simply download the project from https://veins.car2x.org/download/ and import the project into the OMNeT++ IDE. If you need additional guidance, see the install guide https://veins.car2x.org/tutorial/.
 - Python 3 is used to write the machine learning script. You may use the provided "requirements.txt" file to install the necessary dependencies with the command
 `pip install -r requirements.txt`.
 
## Other dependencies
libprotobuf
zmq

## Opening the project
Open OMNeT++ IDE. Ensure the veins-5.2 project has been imported into the IDE. Import the SmartHighway project into your OMNeT++ IDE workspace by clicking `File > Import > General: Existing Projects into Workspace` and selecting the smartHighway/ directory in this repo. At this point, you can develop C++ code for the simulation. 

Next, start the machine learning script, open a terminal and run `python learning.py`.

Then, start the SUMO TRaCI connection service, open another terminal, navigate to your veins-veins-5.2/ folder and run `veins-veins-5.2/bin/veins_launchd -vv`.

Finally, back in the OMNeT++ IDE, navigate to smartHighway/simulations/interstate_5/omnetpp.ini, right-click this file and Run. If everything has been installed correctly, a new window should pop up with a GUI view of the simulation.

## Software Overview
![Software Overview](https://github.com/AICPS/SmartCorridor/blob/main/Software%20Flow.jpg)

## Useful information for traffic light development:
 - https://stackoverflow.com/questions/53693016/create-traffic-light-in-sumo-and-omnet
 - https://doc.omnetpp.org/omnetpp/manual/#sec:ned-lang:parameters
 - http://cse.iitkgp.ac.in/~soumya/micro/t2-3.pdf
 - http://cse.iitkgp.ac.in/~soumya/micro/t2-7.pdf
