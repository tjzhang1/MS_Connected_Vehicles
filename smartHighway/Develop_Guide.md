# Develop Guide

This guide is my attempt at summarizing all of the relevant information needed to modify or create a new simulation using Veins. There are several different open-source projects involved with varying levels of documentation available, so working with Veins has an incredibly steep learning curve.

## SUMO

In SUMO, you can define microtraffic simulations using XML files. In any SUMO project, there are several common file definitions explained below.

- *.sumocfg: This holds the high-level SUMO configuration options, defining the other traffic simulation project files, simulated time scale, RNG seed, output files, etc. To see all the possible options to specify in this file, see https://sumo.dlr.de/docs/sumo.html.
- *rou.xml: This typically holds the vehicle types, routes, and flow definitions for the simulation. To see some common options set in these files, see https://sumo.dlr.de/docs/Definition_of_Vehicles%2C_Vehicle_Types%2C_and_Routes.html#repeated_vehicles_flows.
- *add.xml: This typically defines sensors, parking lots, rerouters, bus stops, and many other miscellaneous traffic-related features. Generally these XML definitions are generated using the netedit tool. See https://sumo.dlr.de/docs/Netedit/elementsAdditional.html
- *net.xml: This defines the driveable lanes and junctions, including the speed limits and allowed vehicle types on each edge. You can use the osmWebWizard tool to build a net.xml file from a real-life road, and then edit the network using netedit.

These file-naming practices are not strict rules for developing SUMO simulations; ultimately sumocfg dictates which XML files are treated as net-files, route-files, and additional-files.

To test your traffic sim without any extra V2X connectivity features, use `sumo your_network.net.xml` to run the sim in the commandline or `sumo-gui your_network.net.xml` to run the sim with the GUI. Once you're satisfied with the setup, you're ready to import these files into your OMNeT++ project.

### Useful Tools

- netedit (included with SUMO install)
- sumo and sumo-gui (included with SUMO install)
- osmWebWizard.py (included with SUMO in $SUMO_HOME$/tools/ folder)

## OMNeT++ and Veins Modules

### NED Files

The OMNeT++ simulator works by allowing users to define network geometries and nodes using the NED language. Nodes (called simple modules in the NED documentation) are the simplest NED files; users can define a C++ class associated with the node defining its behavior, as well as input and output gates for message passing. Higher level compound networks define topologies of multiple nodes as well as connections between the gates for message passing. These networks don't have a corresponding C++ class, functionality is added by including more nodes to the network. https://doc.omnetpp.org/omnetpp/manual/#cha:ned-lang

So for example, for our simulations, there is one overall network called Interstate 5 that contains modules for Vehicles, RSUs, and a TraCIConnectionManager, among other utility modules. The TraCIConnectionManager node has a corresponding C++ class that reads the positions of vehicles from TraCI and dynamically initializes additional Vehicle modules for each SUMO vehicle. 

The Vehicle module is actually another compound module involving an Application, Mobility, and Network Interface Card module. The Mobility module updates the position of the vehicle with respect to the OMNeT++ simulation canvas. The NIC module was designed by the Veins project and exposes input and output gates for DSRC communications; it is also a compound module that implements the 802.11p stack. The Application is connected to the gates of the NIC module and has C++ callback functions to handle what happens when messages are received on the NIC. The Application may also transmit DSRC messages via the NIC module as well.

### Messaging

Speaking of messaging, there are also message format definitions in the *.msg files. You can specify common data types such as int, string, and even custom structs that will be held in the message data. Once a .msg is added to your OMNeT++ project, the IDE compiler will recognize these and automatically generate a corresponding C++ header file defining a class for the message as well as prototypes for you to use in your code to populate the message. https://doc.omnetpp.org/omnetpp/manual/#cha:messages

For instance, RSUs send the TMC a custom message format with the data for the observed speed and density of traffic in its nearby area.

### C++ Classes

Simple NED modules can have C++ classes associated with them. They must be written in a particular way in order to work in simulation. Each class must inherit the cObject module. The default cObject API is to use C++ callback functions to program functionality. Common ones include `initialize()`, which is called when the cObject is loaded into the simulation; `handleMessage(char *msg)`, which is called when the cObject receives a message on one of its gates defined in its corresponding NED file; and `finish()`, which is called when the cObject is removed from the simulation.

In our case, for example, the TMC uses `initialize()` to reset some reward accumulators; the car application uses `handleMessage(char *msg)` to receive broadcasts RSUs and determine whether to reroute to the PAR structure; and the car application uses `finish()` to update the global rewards structure of the simulation.

### TraCI

This stands for the Traffic Control Interface and allows an external program (such as Veins and OMNeT++ modules) to control and read data from the SUMO simulation by sending commands. A full list of available commands is here https://sumo.dlr.de/docs/TraCI.html#traci_commands. The Veins module provides a C++ interface implementing many (but not all) of these commands.

For instance, you may obtain the CO2 emissions of a particular vehicle by specifying the 0xa4 command (Get Vehicle Variable) and the 0x60 variable (CO2 emissions).

### Placing Fixed Nodes

At some point, it may be useful to convert SUMO coordinates into OMNeT++ coordinates because they use different coordinate systems. You can use the GatcomSUMO tool to open your net.xml network file and convert SUMO coordinates into OMNeT++ coordinates. This is particularly useful for RSU placement in your network.

### Simulation Output

By default, the `veins_launchd` script acts as an interface in launching the SUMO simulation and exposing the TraCI interface for the Veins simulation. Any output files generated by the SUMO simulation is placed in the `/tmp/` folder and deleted after the simulation is completed. To work around this, you can copy over the contents of the temporary folder into a local folder before the temporary folder is deleted, as demonstrated in my `learning.py` script.

### Useful Tools

- GatcomSUMO (https://sourceforge.net/p/gatcomsumo/wiki/About/)

## Python

The Python learning script interacts with the Veins simulation using the Veins-Gym project, which allows one to treat a Veins simulation like an OpenAI Gym environment utilizing a similar API. https://github.com/tkn-tub/veins-gym

The Veins-Gym project is a bit old and provides an environment API that is outdated by OpenAI's standards, so it is important to install the correct version of the `gym` Python package.

Another quirk that ended up being a big issue with RL training is that when the Veins simulation ends, the Veins-Gym API generates a random state in the state space and returns a reward of zero. It's important that your learning agent handles this end of simulation case correctly in that the RL agent doesn't try to associate this randomized end of simulation behavior with its most recent action taken.