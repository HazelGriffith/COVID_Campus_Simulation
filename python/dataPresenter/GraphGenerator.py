import matplotlib.pyplot as plt
import numpy as np

#This section reads the data parsed from the simulation
with open("../../data/ParsedData.txt") as dataFile:
    
    dataLines = dataFile.readlines()
    CO2Data = {}
    occupancyData = {}
    timeStamps = []
    infectionProbData = {}
    
    for line in dataLines:
        words = line.split()
        if ("Room" in words[0]) or ("Outdoors" in words[0]):
            if ("CO2" in words[1]):
                if ("Room" in words[0]):
                    ID = words[0].split("m")[1]
                else:
                    ID = words[0]
                if ID in CO2Data.keys():
                    CO2Data[ID].append(words[2])
                else:
                    CO2Data.update({ID: [words[2]]})
            elif ("occupancy" in words[1]):
                if ("Room" in words[0]):
                    ID = words[0].split("m")[1]
                else:
                    ID = words[0]
                if ID in occupancyData.keys():
                    occupancyData[ID].append(words[2])
                else:
                    occupancyData.update({ID: [words[2]]})
        elif (len(words) == 1):
            timeStamps.append(words[0])
        else:
            key = words[0]
            if key in infectionProbData.keys():
                infectionProbData[key].extend([(words[1],words[2])])
            else:
                infectionProbData.update({key: [(words[1],words[2])]})

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
for i in range(1,len(CO2Data.keys())):
    s = "../../data/rooms/" + str(i) + ".xml"
    roomFile = open(s)
    roomLines = roomFile.readlines()
    ventilationRatesPerRoom.update({i:{}})
    for line in roomLines:
        if ("ventilationRating" in line) or ("squareMetres" in line) or ("height" in line):
            value = line.split(">")[1].split("<")[0]
            dataType = line.split(">")[0].split("<")[1]
            ventilationRatesPerRoom[i].update({dataType:float(value)})
    
roomFile = open("../../data/rooms/tunnels.xml")
roomLines = roomFile.readlines()
ventilationRatesPerRoom.update({"tunnels":{}})
for line in roomLines:
    if ("ventilationRating" in line) or ("squareMetres" in line) or ("height" in line):
        value = line.split(">")[1].split("<")[0]
        dataType = line.split(">")[0].split("<")[1]
        ventilationRatesPerRoom["tunnels"].update({dataType:float(value)})
            
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
for timeSlot in occupancyPerRoomPerHalfHour["1"].keys():
    
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

    
#Ventilation versus Overall Average prob of Inf
x = []
for value in ventilationRatesPerRoom.values():
    x.append(value)
    
y = []
for value in overallProbsOfInfectionPerRoom.values():
    y.append(value)

plt.ylabel('Average Probability of Infection(%)')
plt.xlabel('Ventilation Rate(L/min)')
plt.suptitle("Overall Probability of Infection versus Ventilation Rate")
plt.plot(x,y,'bo')
plt.show()

for room in occupancyPerRoomPerHalfHour.keys():
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
    plt.plot(x,y,'b-')
    plt.ylabel('occupancy')
    plt.xlabel('time(hours)')
    if (room == "Outdoors"):
        plt.suptitle("Outdoors Occupancy")
    else:
        plt.suptitle("Room"+room+" Occupancy")
    plt.show()
    
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
        plt.plot(x,y,'b-')
        plt.ylabel("Average Probability of Infection(%)")
        plt.xlabel("time(hours)")
        plt.suptitle("Room"+room+" Average Probability of Infection")
        plt.show()



