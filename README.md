# COVID_Campus_Simulation
A simulation of how COVID-19 spreads across a university campus. It uses an Architecture to study Diffusion Processes in Multiplex dynamic networks (ADPM) as created by Cristina Ruiz-Martin and Gabriel Wainer. 

# People XML File Generation Instructions
To generate x number of people XML files, open a bash prompt in the data folder, and execute "make generator", then "./PEOPLE_XML_GENERATOR". It will prompt you for the number of people you want to randomly generator, enter it. Then it will prompt you for the number of rooms in the simulation to randomly create their schedules around, enter that and it will finish generating the people XML files and store them in the data/people folder.

# Running Simulation Instructions
To run the simulation open a bash prompt in the main directory. Execute "make clean", and then "make simulator". Open a bash prompt in the bin folder, and then execute "./ROOM_SIM". It will prompt you for the number of people to include in the simulation, and the number of hours the simulation will run for. Enter both accordingly and the simulation will start running. Depending upon the values used it could take some time to complete. The longest I have seen it run for is 25 minutes.

# Running Parser Instructions
To run the parser, open a bash prompt in the data folder. Execute "make clean", "make parser", and then "./PARSER". It will prompt you for the number of rooms, and the number of people. Enter these values when requested. The data parsed from the simulation logger files will be stored in a file called parsedData.txt.

# Running the Graph Generator
To run the graph generator open the python script found in the python folder in your IDE of choice. I recommend Spyder as it has a plot viewer. Then run the script, and it will generate graphs representing the data parsed from the simulation logger files.
