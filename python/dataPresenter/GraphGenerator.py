import matplotlib.pyplot as plt
import numpy as np

#This section reads the data parsed from the simulation
with open("../../data/ParsedData.txt") as dataFile:
    
    dataLines = dataFile.readlines()
    CO2Data = {}
    occupancyData = {}
    timeStamps = []
    infectionProbData = {}
    highestInfectionProbInstances = {}
    
    for line in dataLines:
        words = line.split(',')
        if "CO2" in line:
            room = words[0]
            CO2 = words[-1]
            if room in CO2Data.keys():
                CO2Data[room].append(CO2)
            else:
                CO2Data.update({room: [CO2]})
        elif "occupancy" in line:
            room = words[0]
            occupancy = words[-1]
            if room in occupancyData.keys():
                occupancyData[room].append(occupancy)
            else:
                occupancyData.update({room: [occupancy]})
        elif (len(words) == 1):
            timeStamps.append(words[0])
        else:
            key = words[0]
            room = words[1]
            if key in infectionProbData.keys():
                infectionProbData[key].extend([(room,words[-1])])
            else:
                infectionProbData.update({key: [(room,words[-1])]})
                
            key = room
                
            if key in highestInfectionProbInstances.keys():
                if float(words[-1]) > float(highestInfectionProbInstances[key]):
                    highestInfectionProbInstances.update({key: words[-1]})
            else:
                highestInfectionProbInstances.update({key: words[-1]})
            
highestInfectionProbInstances = sorted(highestInfectionProbInstances.items(), key=lambda x: x[1], reverse=True)[:10]

#data formatting

#The overall averages of the probabilities of infection for each room are calculated from the data
overallProbsOfInfectionPerRoom = {}
for room in CO2Data.keys():
    overallProbsOfInfectionPerRoom.update({room:[]})
    
for key in infectionProbData.keys():
    roomProbList = infectionProbData[key]
    for room in overallProbsOfInfectionPerRoom.keys():
        for roomInfTup in roomProbList:
            ID = roomInfTup[0].split("m")[1]
            if (ID == room):
                overallProbsOfInfectionPerRoom[room].append(float(roomInfTup[1]))
                
for key in overallProbsOfInfectionPerRoom.keys():
    size = len(overallProbsOfInfectionPerRoom[key])
    total = 0
    for value in overallProbsOfInfectionPerRoom[key]:
        total += value
    if (size > 0):
        total /= size
    overallProbsOfInfectionPerRoom[key] = total
    
#The ventilation rates are calculated from the data extracted from each room's xml file
ventilationRatesPerRoom = {}

keys = []
for key in CO2Data.keys():
    keys.append(key)

for i in range(len(keys)):
    if "Room" in keys[i]:
        filename = keys[i].split("m")[1]
    else:
        filename = keys[i]
    s = "../../data/rooms/" + filename + ".xml"
    print(s)
    roomFile = open(s)
    roomLines = roomFile.readlines()
    ventilationRatesPerRoom.update({i:{}})
    for line in roomLines:
        if ("ventilationRating" in line) or ("squareMetres" in line) or ("height" in line):
            value = line.split(">")[1].split("<")[0]
            dataType = line.split(">")[0].split("<")[1]
            ventilationRatesPerRoom[i].update({dataType:float(value)})
            print(value)
    roomFile.close()
"""   
roomFile = open("../../data/rooms/tunnels.xml")
roomLines = roomFile.readlines()
ventilationRatesPerRoom.update({"tunnels":{}})
for line in roomLines:
    if ("ventilationRating" in line) or ("squareMetres" in line) or ("height" in line):
        value = line.split(">")[1].split("<")[0]
        dataType = line.split(">")[0].split("<")[1]
        ventilationRatesPerRoom["tunnels"].update({dataType:float(value)})
"""           
for key in ventilationRatesPerRoom.keys():
    variables = ventilationRatesPerRoom[key]
    ACH = variables["ventilationRating"]
    squareMetres = variables["squareMetres"]
    height = variables["height"]
    volumeOfRoom = squareMetres*height
        
    ventilationRatePerMin = ((ACH*volumeOfRoom)/60)*1000
    ventilationRatesPerRoom[key] = ventilationRatePerMin
#The occupancy for each room at half hour intervals are counted and stored
occupancyPerRoomPerHalfHour = {}
#The average prob of infection for each room at half hour intervals are calculated and stored
probOfInfPerRoomPerHalfHour = {}

for room in occupancyData.keys():
    occupancyPerRoomPerHalfHour.update({room:{}})
    if ("Outdoors" not in room):
        #Outdoors is not considered because prob of Inf always = 0
        probOfInfPerRoomPerHalfHour.update({room:{}})
        
#Prepare ending time of half hour intervals
endTime = timeStamps[len(timeStamps)-1]
hours,minutes,sec,msec = endTime.split(":")
minutes = int(minutes)
hours = int(hours)
if (minutes != 0) or (minutes != 30):
    if (minutes < 30):
        endTime = str(hours) + ":30"
    else:
        endTime = str(hours+1) + ":00"

#Initializes each half hour interval in both dictionaries
hours = int(endTime.split(":")[0])
for hour in range(hours+1):
    for minutes in range(2):
        for room in occupancyPerRoomPerHalfHour.keys():
            occupancyPerRoomPerHalfHour[room].update({str(hour)+":"+str(minutes*30):set()})
            if ("Outdoors" not in room):
                probOfInfPerRoomPerHalfHour[room].update({str(hour)+":"+str(minutes*30):[]})
                
#Prepares list of timeStamps used for the average prob of Infection
infTimeStamps = []
for key in infectionProbData.keys():
    infTimeStamps.append(key)
    
#This iterates through each half hour interval
for timeSlot in occupancyPerRoomPerHalfHour["RoomTunnels"].keys():
    
    slotHours,slotMinutes = timeSlot.split(":")
    slotHours = int(slotHours)
    slotMinutes = int(slotMinutes)
    
    #Occupancy is counted here
    if (len(timeStamps) > 0):
        hours,minutes,sec,msec = timeStamps[0].split(":")
        hours = int(hours)
        minutes = int(minutes)
    else:
        hours = slotHours + 1
        minutes = 0
    #While the next timeStamp falls within the half hour interval it is counted for each room
    while ((hours == slotHours) and (minutes < slotMinutes)) or (hours < slotHours):
        sameLoop = False
        timeStamps.pop(0)
        for room in occupancyPerRoomPerHalfHour.keys():
            #sameLoop checks if the while loop is still looping
            #It is used to determine if the occupancy increases, then the counted value increases
            #Otherwise it would mean the same person is still in the room, or has left the room
            
            if occupancyData[room] == []:
                continue;
            
            people = occupancyData[room].pop(0)
            if (people != "none"):
                if (len(people) == 1):
                    occupancyPerRoomPerHalfHour[room][timeSlot].add(people)
                else:
                    peopleList = people.split(",")
                    for i in range(len(peopleList)):
                        occupancyPerRoomPerHalfHour[room][timeSlot].add(peopleList[i])
                            
        if (len(timeStamps) > 0):
            hours,minutes,sec,msec = timeStamps[0].split(":")
            hours = int(hours)
            minutes = int(minutes)
            sameLoop = True
        else:
            hours = slotHours + 1
    
    #Average Prob of Infection is calculated here
    if (len(infTimeStamps) > 0):
        hours,minutes,sec,msec = infTimeStamps[0].split(":")
        hours = int(hours)
        minutes = int(minutes)
    else:
        hours = slotHours + 1
        minutes = 0
    while((hours == slotHours) and (minutes < slotMinutes)) or (hours < slotHours):
        #The list of tuples is popped and stored
        roomProbCombo = infectionProbData[infTimeStamps.pop(0)]
        for room in probOfInfPerRoomPerHalfHour.keys():
            for roomInfTup in roomProbCombo:
                ID = roomInfTup[0].split("m")[1]
                if (ID == room):
                    #The probs of Infection are added to a list for each half hour interval
                    probOfInfPerRoomPerHalfHour[room][timeSlot].append(float(roomInfTup[1]))
        if (len(infTimeStamps) > 0):
            hours,minutes,sec,msec = infTimeStamps[0].split(":")
            hours = int(hours)
            minutes = int(minutes)
        else:
            hours = slotHours + 1
            minutes = 0
            
#Each list of infection probs are averaged
for key in probOfInfPerRoomPerHalfHour.keys():
    for timeSlot in probOfInfPerRoomPerHalfHour[key]:
        size = len(probOfInfPerRoomPerHalfHour[key][timeSlot])
        total = 0
        for value in probOfInfPerRoomPerHalfHour[key][timeSlot]:
            total += value
        if (size > 0):
            total = total/size
        probOfInfPerRoomPerHalfHour[key][timeSlot] = total
        
#Each set of Person ID's is used to obtain the number of people who visited the room in each time slot
for key in occupancyPerRoomPerHalfHour.keys():
    for timeSlot in occupancyPerRoomPerHalfHour[key].keys():
        size = len(occupancyPerRoomPerHalfHour[key][timeSlot])
        occupancyPerRoomPerHalfHour[key][timeSlot] = size
        
#PLOTS


#rooms ranked by highest reached infection probability

x = [h[0] for h in highestInfectionProbInstances]
highest = [float(h[1]) for h in highestInfectionProbInstances]
overall = []


import pandas as pd

for key in x:
    if key[4:] in overallProbsOfInfectionPerRoom.keys() or key in overallProbsOfInfectionPerRoom.keys():
        overall.append(overallProbsOfInfectionPerRoom.get(key[4:]))


df = pd.DataFrame(np.c_[highest,overall], index=x)
df = df.rename(columns={0: "Highest Value Reached", 1: "Average Value"})
df.plot.bar(title = "Rooms Ranked by Highest Reached Infection Probability")
plt.ylabel('Probability of Infection(%)')
plt.xlabel('Room')
plt.show()      
    
#Ventilation versus Overall Average prob of Inf
x = []
for value in ventilationRatesPerRoom.values():
    x.append(value)
        
y = []

levels = []

for value in overallProbsOfInfectionPerRoom.values():
    y.append(value)
    if value > 20:
        levels.append('r')
    elif value > 10:
        levels.append('y')
    else:
        levels.append('g')

plt.ylabel('Average Probability of Infection(%)')
plt.xlabel('Ventilation Rate(L/min)')
plt.suptitle("Overall Probability of Infection versus Ventilation Rate")
plt.scatter(x,y,color = levels)
plt.show()

for room in occupancyPerRoomPerHalfHour.keys():
    
    fig, subplts = plt.subplots(2, sharex=True)
    
    x.clear()
    y.clear()
    for timeSlot in occupancyPerRoomPerHalfHour[room].keys():
        hours,minutes = timeSlot.split(":")
        hours = int(hours)
        minutes = int(minutes)
        if (hours >= 8):
            time = hours;
            time += minutes/60
            x.append(time)
            y.append(occupancyPerRoomPerHalfHour[room][timeSlot])
            
    subplts[0].plot(x,y,'y-')
    subplts[0].set_ylabel('occupancy')
    
    x.clear()
    y.clear()
    if (room != "Outdoors"):
        for timeSlot in probOfInfPerRoomPerHalfHour[room].keys():
            hours,minutes = timeSlot.split(":")
            hours = int(hours)
            minutes = int(minutes)
            if (hours >= 8):
                time = hours;
                time += minutes/60
                x.append(time)
                y.append(probOfInfPerRoomPerHalfHour[room][timeSlot])
                
        subplts[1].plot(x, y,'r-')
        
    subplts[1].set_ylabel("Average Probability of Infection(%)")
    subplts[1].set_xlabel("time(hours)")
    plt.suptitle("Room"+room+" Occupancy & Average Probability of Infection")
    plt.show()       
