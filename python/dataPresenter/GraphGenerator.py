import matplotlib.pyplot as plt
import numpy as np
import math

#This section reads the data parsed from the simulation
with open("../../data/ParsedData.txt") as dataFile:
    
    dataLines = dataFile.readlines()
    CO2Data = {}
    occupancyData = {}
    timeStamps = []
    infectionProbData = {}
    highestInfectionProbInstances = {}
    personInfectionProbData = {}
    personList = [];
    
    for line in dataLines:
        words = line.split(',')
        if "CO2" in line:
            room = words[0]
            CO2 = words[-1]
            CO2 = CO2.split('\n')[0]
            if room in CO2Data.keys():
                CO2Data[room].append(float(CO2))
            else:
                CO2Data.update({room: [float(CO2)]})
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
            person = words[2]
            if person not in personList:
                personList.append(person);
            if key in infectionProbData.keys():
                infectionProbData[key].extend([(room,words[-1])])
                personInfectionProbData[key].extend([(person, words[-1])])
            else:
                infectionProbData.update({key: [(room,words[-1])]})
                personInfectionProbData.update({key: [(person, words[-1])]})
                
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
   # print(s)
    roomFile = open(s)
    roomLines = roomFile.readlines()
    ventilationRatesPerRoom.update({i:{}})
    for line in roomLines:
        if ("ventilationRating" in line) or ("squareMetres" in line) or ("height" in line):
            value = line.split(">")[1].split("<")[0]
            dataType = line.split(">")[0].split("<")[1]
            ventilationRatesPerRoom[i].update({dataType:float(value)})
           # print(value)
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

probOfInfPerPersonPerHalfHour = {}

CO2PerRoomPerHalfHour = {}

for room in occupancyData.keys():
    occupancyPerRoomPerHalfHour.update({room:{}})
    if ("Outdoors" not in room):
        #Outdoors is not considered because prob of Inf always = 0
        probOfInfPerRoomPerHalfHour.update({room:{}})
        CO2PerRoomPerHalfHour.update({room:{}})

for person in personList:
    probOfInfPerPersonPerHalfHour.update({person:{}})

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
                CO2PerRoomPerHalfHour[room].update({str(hour)+":"+str(minutes*30):[]})
        
        for person in probOfInfPerPersonPerHalfHour.keys():
            probOfInfPerPersonPerHalfHour[person].update({str(hour)+":"+str(minutes*30):0})
                
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
            if ("Outdoors" not in room):
                CO2 = CO2Data[room].pop(0)
                CO2PerRoomPerHalfHour[room][timeSlot].append(CO2)
            
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
        nextTimeStamp = infTimeStamps.pop(0)
        roomProbCombo = infectionProbData[nextTimeStamp]
        for room in probOfInfPerRoomPerHalfHour.keys():
            for roomInfTup in roomProbCombo:
                ID = roomInfTup[0].split("m")[1]
                if (ID == room):
                    #The probs of Infection are added to a list for each half hour interval
                    probOfInfPerRoomPerHalfHour[room][timeSlot].append(float(roomInfTup[1]))
                    
        personProbCombo = personInfectionProbData[nextTimeStamp]
        for person in probOfInfPerPersonPerHalfHour.keys():
            for personInfTup in personProbCombo:
                ID = personInfTup[0]
                if (ID == person):
                    probOfInfPerPersonPerHalfHour[person][timeSlot] = (float(personInfTup[1]))
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


        
for key in CO2PerRoomPerHalfHour.keys():
    for timeSlot in CO2PerRoomPerHalfHour[key]:
        tHours = int(timeSlot.split(':')[0])
        tDay = math.floor(tHours/24)
        timeOfDay = tHours - (24*tDay)
        if (timeOfDay <= 22) and (timeOfDay >= 9):
            size = len(CO2PerRoomPerHalfHour[key][timeSlot])
            total = 0
            for value in CO2PerRoomPerHalfHour[key][timeSlot]:
                total += value
            if (size > 0):
                total = total/size
            CO2PerRoomPerHalfHour[key][timeSlot] = total
        else:
            CO2PerRoomPerHalfHour[key][timeSlot] = 0
        '''
for key in CO2PerRoomPerHalfHour.keys():
    start = 0
    startSlot = ""
    end = 0
    nullPoints = []
    for timeSlot in CO2PerRoomPerHalfHour[key]:
        CO2 = CO2PerRoomPerHalfHour[key][timeSlot]
        if (CO2 != 0):
            if (len(nullPoints) == 0):
                start = CO2
                startSlot = timeSlot
            elif (len(nullPoints) != 0):
                if (start != 0):
                    end = CO2
                    endSlot = timeSlot.split(':')
                    endHours = int(endSlot[0])
                    endMinutes = int(endSlot[1])
                    endTime = endHours + (endMinutes/60)
                    startSlot = startSlot.split(':')
                    startHours = int(startSlot[0])
                    startMinutes = int(startSlot[1])
                    startTime = startHours + (startMinutes/60)
                    a = (end - start)/(endTime - startTime)
                    b = -1*(a*startTime) + start
                    for point in nullPoints:
                        pointSlot = point.split(':')
                        pointHours = int(pointSlot[0])
                        pointMinutes = int(pointSlot[1])
                        pointTime = pointHours + (pointMinutes/60)
                        CO2PerRoomPerHalfHour[key][point] = a*pointTime + b
                    nullPoints.clear()
                    start = end
                    startSlot = timeSlot
                    end = 0
                else:
                    for point in nullPoints:
                        pointSlot = point.split(':')
                        pointHours = int(pointSlot[0])
                        pointMinutes = int(pointSlot[1])
                        pointTime = pointHours + (pointMinutes/60)
                        CO2PerRoomPerHalfHour[key][point] = CO2
                    nullPoints.clear()
                    start = CO2
                    startSlot = timeSlot
        else:
            nullPoints.append(timeSlot)
            '''
for key in probOfInfPerPersonPerHalfHour.keys():
    for timeSlot in probOfInfPerPersonPerHalfHour[key]:
        if (probOfInfPerPersonPerHalfHour[key][timeSlot] == 0):
            probOfInfPerPersonPerHalfHour[key][timeSlot] = 200
            
for key in probOfInfPerPersonPerHalfHour.keys():
    start = 0
    startSlot = ""
    end = 0
    nullPoints = []
    for timeSlot in probOfInfPerPersonPerHalfHour[key]:
        prob = probOfInfPerPersonPerHalfHour[key][timeSlot]
        if (prob != 200):
            if (len(nullPoints) == 0):
                start = prob
                startSlot = timeSlot
            elif (len(nullPoints) != 0):
                if (start != 0):
                    end = prob
                    endSlot = timeSlot.split(':')
                    endHours = int(endSlot[0])
                    endMinutes = int(endSlot[1])
                    endTime = endHours + (endMinutes/60)
                    startSlot = startSlot.split(':')
                    startHours = int(startSlot[0])
                    startMinutes = int(startSlot[1])
                    startTime = startHours + (startMinutes/60)
                    a = (end - start)/(endTime - startTime)
                    b = -1*(a*startTime) + start
                    for point in nullPoints:
                        pointSlot = point.split(':')
                        pointHours = int(pointSlot[0])
                        pointMinutes = int(pointSlot[1])
                        pointTime = pointHours + (pointMinutes/60)
                        probOfInfPerPersonPerHalfHour[key][point] = a*pointTime + b
                    nullPoints.clear()
                    start = end
                    startSlot = timeSlot
                    end = 0
                else:
                    for point in nullPoints:
                        pointSlot = point.split(':')
                        pointHours = int(pointSlot[0])
                        pointMinutes = int(pointSlot[1])
                        pointTime = pointHours + (pointMinutes/60)
                        probOfInfPerPersonPerHalfHour[key][point] = prob
                    nullPoints.clear()
                    start = prob
                    startSlot = timeSlot
        else:
            nullPoints.append(timeSlot)

averageExpProbPerPerson = {}
amountOfValuesPerPerson = {}
averageExpProb = 0
varianceExpProb = 0
for key in probOfInfPerPersonPerHalfHour.keys():
    averageExpProbPerPerson.update({key: 0})
    amountOfValuesPerPerson.update({key: 0})
    prevOne = False
    for timeSlot in probOfInfPerPersonPerHalfHour[key].keys():
        if (probOfInfPerPersonPerHalfHour[key][timeSlot] <= 100):
            if ((probOfInfPerPersonPerHalfHour[key][timeSlot] == 1) and (prevOne == False)):
                averageExpProbPerPerson[key] = averageExpProbPerPerson[key] + probOfInfPerPersonPerHalfHour[key][timeSlot]
                amountOfValuesPerPerson[key] += 1
                prevOne = True
            elif (probOfInfPerPersonPerHalfHour[key][timeSlot] != 1):
                averageExpProbPerPerson[key] = averageExpProbPerPerson[key] + probOfInfPerPersonPerHalfHour[key][timeSlot]
                amountOfValuesPerPerson[key] += 1
                prevOne = False
    averageExpProbPerPerson[key] = averageExpProbPerPerson[key]/amountOfValuesPerPerson[key]
    averageExpProb += averageExpProbPerPerson[key]

averageExpProb = averageExpProb/1200

for key in averageExpProbPerPerson.keys():
    varianceExpProb += (averageExpProbPerPerson[key] - averageExpProb) ** 2
    
varianceExpProb = varianceExpProb / (len(averageExpProbPerPerson)-1)

print("averageExpProb: " + str(averageExpProb))
print("varianceExpProb: " + str(varianceExpProb))

'''
averageCO2PerRoom = {}
averageCO2 = 0
varianceCO2 = 0
for key in CO2PerRoomPerHalfHour.keys():
    averageCO2PerRoom.update({key: 0})
    for timeSlot in CO2PerRoomPerHalfHour[key].keys():
        if (CO2PerRoomPerHalfHour[key][timeSlot] != 0):
            averageCO2PerRoom[key] += CO2PerRoomPerHalfHour[key][timeSlot]
    averageCO2PerRoom[key] = averageCO2PerRoom[key] / 669
    averageCO2 += averageCO2PerRoom[key]

averageCO2 = averageCO2 / len(averageCO2PerRoom)
    
for key in averageCO2PerRoom.keys():
    varianceCO2 += (averageCO2PerRoom[key] - averageCO2) ** 2
    
varianceCO2 = varianceCO2 / (len(averageCO2PerRoom)-1)

print("averageCO2: " + str(averageCO2))
print("varianceCO2: "+ str(varianceCO2))
'''
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
df.plot.bar(title = "Rooms Ranked by Highest Reached Exposure Probability")
plt.ylabel('Probability of Exposure(%)')
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

plt.ylabel('Average Probability of Exposure(%)')
plt.xlabel('Ventilation Rate(L/min)')
plt.suptitle("Overall Probability of Exposure versus Ventilation Rate")
plt.scatter(x,y,color = levels)
plt.show()

x.clear()
y.clear()
listOfPeople = ['id25', 'id1', 'id53', 'id889', 'id543', 'id1099', 'id765', 'id999', 'id812', 'id1111']

x1 = []
y1 = []
for key in listOfPeople:
    for timeSlot in probOfInfPerPersonPerHalfHour[key].keys():
        hours,minutes = timeSlot.split(":")
        hours = int(hours)
        minutes = int(minutes)
        time = hours
        time += minutes/60
        x.append(time)
        if (probOfInfPerPersonPerHalfHour[key][timeSlot] != 200):
            y.append(probOfInfPerPersonPerHalfHour[key][timeSlot])
            if ((time >= 8.5)and(time <= 22.5)):
                x1.append(time)
                y1.append(probOfInfPerPersonPerHalfHour[key][timeSlot])
        else:
            y.append(0)
            if ((time >= 8.5)and(time <= 22.5)):
                x1.append(time)
                y1.append(0)
            
    plt.ylabel('Probability of Exposure(%)')
    plt.xlabel('time(hours)')
    plt.suptitle('Probability of Exposure for Person ' + key)
    plt.scatter(x,y)
    plt.show()
        
    plt.ylabel('Probability of Exposure(%)')
    plt.xlabel('time(hours)')
    plt.suptitle('Probability of Exposure for Person ' + key)
    plt.scatter(x1,y1)
    plt.show()
    
    x.clear()
    y.clear()
    x1.clear()
    y1.clear()
    '''
for key in CO2PerRoomPerHalfHour.keys():
    for t in CO2PerRoomPerHalfHour[key].keys():
        hours,minutes = t.split(":")
        hours = int(hours)
        minutes = int(minutes)
        time = hours
        time += minutes/60
        if (CO2PerRoomPerHalfHour[key][t] != 0):
            x.append(time)
            y.append(CO2PerRoomPerHalfHour[key][t])
            if ((time >= 8.5)and(time <= 22.5)):
                x1.append(time)
                y1.append(CO2PerRoomPerHalfHour[key][t])
        
    plt.ylabel('CO2 Concentration(mg/L)')
    plt.xlabel('time(hours)')
    plt.suptitle('CO2 Concentration for Room ' + key)
    plt.scatter(x,y)
    plt.show()
        
    plt.ylabel('CO2 Concentration(mg/L)')
    plt.xlabel('time(hours)')
    plt.suptitle('CO2 Concentration for Room ' + key)
    plt.scatter(x1,y1)
    plt.show()
        
    x.clear()
    y.clear()
    x1.clear()
    y1.clear()
'''


'''
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
    plt.show()     '''  
