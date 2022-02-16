# COVID_Campus_Simulation
A simulation of how COVID-19 spreads across a university campus. It uses an Architecture to study Diffusion Processes in Multiplex dynamic networks (ADPM) as created by Cristina Ruiz-Martin and Gabriel Wainer. To use this simulation, the user must have a Ubuntu terminal installed on their computer.

# CADMIUM Installation Instructions
To use this simulation, CADMIUM must be installed first. A user manual for CADMIUM can be found at the following URL: 
http://www.sce.carleton.ca/courses/sysc-5104/lib/exe/fetch.php?media=cadmium.pdf

# Simulation Directory Setup Instructions
Create a folder with a name of your choice located inside of the "Cadmium-Simulation-Environment/DEVS-Models" directory that was created during the installation of CADMIUM. Download or clone the contents of this repository into this new folder. This folder will be referred to as the main directory for all following instructions.

# Person and Room XML File Generation Instructions
To generate people and room XML files, open a bash prompt in the data folder inside of the main directory and execute "make clean", "make generator", then "./GENERATOR". You will then be prompted to enter the number of people to generate. Once this value is entered, that amount of people XML files will be generated and stored in the data/people folder. The room XML files based on real room data from the Carleton University campus will be generated and stored in the data/rooms folder.

# Running Simulation Instructions
To run the simulation open a bash prompt in the main directory. Execute "make clean", and then "make simulator". Open a bash prompt in the bin folder, and then execute "./ROOM_SIM". It will prompt you for the number of people to include in the simulation, and the number of hours the simulation will run for. Enter both accordingly and the simulation will start running immediately. The required amount of person XML files must already have been generated in the data/people folder, or the simulation will not run correctly. Depending upon the number of people and simulation time, the simulation could take some time to complete. The longest I have seen it run for is 25 minutes.

# Running Parser Instructions
To run the parser, open a bash prompt in the data folder inside of the main directory. Execute "make clean", "make parser", and then "./PARSER". It will prompt you for the number of rooms, and the number of people. Enter these values when requested. The data parsed from the simulation logger files will be stored in a file called parsedData.txt.

# Running the Graph Generator
To run the graph generator open the python script found in the python folder in your IDE of choice. I recommend Spyder as it has a plot viewer. Then run the script, and it will generate graphs representing the data parsed from the simulation logger files.
