from __future__ import division
# import itertools
# import os
# import matplotlib.pyplot as plt
# import pandas as pd

import shutil
import sys
import math
import xml.etree.ElementTree as ET

import numpy as np

import VehicleClass
import TaskClass


def     loadScenarioConfig(trade):

    print '      Loading scenario configuration...'            

    scen = trade.find('Scenario_Parameters')
    for item in scen.findall('taskSelectionMethod'):
        taskSelectionMethod = item.get('name')
        print '         Task selection method:', taskSelectionMethod  

    for item in scen.findall('taskSelectionDistanceMeasure'):
        taskSelectionDistanceMeasure = item.get('name') 
        print '         Distance measure:', taskSelectionDistanceMeasure           

    for item in scen.findall('taskStarts'):
        taskStarts = int(item.text)
        print '         Task starts:', taskStarts

    for item in scen.findall('commMode'):
        commMode = item.get('name')
        print '         Comm Mode:', commMode

    return taskSelectionMethod, taskSelectionDistanceMeasure, taskStarts, commMode


def loadTaskConfig(trade):     

    taskinfo = trade.find('Task_Info')  
    taskGeometry = taskinfo.find('TaskGeometry').text
    numTasks = len(taskinfo.findall('Task'))
    
    print ''
    print '      Loading task configurations ({} tasks, {})...'.format(numTasks, taskGeometry)

    taskVector = []
    for task in taskinfo.findall('Task'):
        taskID = int(task.find('ID').text)
        xCoord = float(task.find('xCoord').text)
        yCoord = float(task.find('yCoord').text)
        priority = float(task.find('priority').text)  
        print '         Task {} @ ({},{}), Priority={}'.format(taskID, xCoord, yCoord, priority)
        taskObj = TaskClass.Task(taskID, xCoord, yCoord, priority)  #INSTATIATE TASK OBJECT  
        taskVector.append(taskObj)    #ADD OBJECT TO VECTOR OF TASK OBJECTS

    return taskVector, taskGeometry

def loadVehicleConfig(trade, taskSelectionMethod):

    

    vehinfo = trade.find('Vehicle_Info')
    numVehicles = len(vehinfo.findall('Vehicle'))
   
    print ''
    print '      Loading vehicle configurations ({} vehicles)...'.format(numVehicles)

    vehicleVector = []
    for index, vehicle in enumerate(vehinfo.findall('Vehicle')):
        vehicleID = int(vehicle.find('ID').text)
        print '         Vehicle', vehicleID
        initLocation = int(vehicle.find('initLocation').text)
        print '            Initial Location: Task', initLocation
        initHeading = float(vehicle.find('initHeading').text)
        print '            Initial Heading:',initHeading, 'degrees.'
        vehSpeed = float(vehicle.find('vehSpeed').text)
        print '            Vehicle Speed:', vehSpeed
        vehBankAngle = float(vehicle.find('vehBankAngle').text)
        print '            Vehicle Bank Angle: ', vehBankAngle
        turnRadius = vehSpeed**2/(9.807*math.tan(vehBankAngle*math.pi/180))

        if taskSelectionMethod == 'manual':
            tour = eval(vehicle.find('tour').text)
            print '            Tour={}'.format(tour)
            vehicleObj = VehicleClass.VehicleManual(index, vehicleID, initLocation, initHeading, vehSpeed, turnRadius,
            numVehicles, tour) #INSTATIATE VEHICLE OBJECT        
                
        if taskSelectionMethod == 'md2wrp':
            beta = float(vehicle.find('beta').text)
            print '            Beta={}'.format(beta)
            weightVector = eval(vehicle.find('w').text)
            print '            Weight Vector={}'.format(weightVector)      
            vehicleObj = VehicleClass.VehicleMD2WRP(index, vehicleID, initLocation, initHeading, vehSpeed, turnRadius,
            numVehicles, beta, weightVector) #INSTATIATE VEHICLE OBJECT

        vehicleVector.append(vehicleObj)

    return vehicleVector


def calcLongestDistance(taskVector):

    distVector = []
    for fromTask in taskVector:
        for toTask in taskVector:
            dist = math.sqrt(math.pow(fromTask.location[0]-toTask.location[0], 2)+
                math.pow(fromTask.location[1]-toTask.location[1], 2))
            distVector.append(dist)

    distVector = np.array(distVector)
    longestDistance = distVector.max()

    return longestDistance

def main():

    ######################################################################
    ###            LOAD CONFIGURATION FILES AND INSANTIATE OBJECTS   #####
    ######################################################################    
    #Argument supplying the location of the simulation configuration files
    simPath = sys.argv[1]

    #Create and open the XML where simulation data will be saved
    simDataPath = './Data/'
    print '\nSimulation data will be saved to {}\n'.format(simDataPath)
    fileName = simDataPath + 'simData.xml'
    configFile = open(fileName,'w')
    configFile.write('<Sim_Data>\n')

    #Parse the simulation configuration file
    e = ET.parse(simPath+'simConfiguration.xml').getroot()

    #Perform a simulation for each trade
    for trade in e.findall('Trade'):
        tradeID = int(trade.find('tradeID').text)
        print '***************************************'
        print '          Loading TradeID={}        '.format(tradeID)
        print '***************************************'

        # Parse and load the scenario configuration parameters
        taskSelectionMethod, taskSelectionDistanceMeasure, taskStarts, commMode = loadScenarioConfig(trade)       

        #Parse and load the task configuration; returns a vector of task objects
        taskVector, taskGeometry = loadTaskConfig(trade)
    
        #Parse and load the vehicle configuration; returns a vector of vehicle objects
        vehicleVector = loadVehicleConfig(trade, taskSelectionMethod)
    
    ######################################################################
    ######################################################################    
    
        np.set_printoptions(suppress=True)
        np.set_printoptions(threshold='nan')

        #Calculate longest Euclidean distance on the map (for normalization)
        longestDistance = calcLongestDistance(taskVector)
        print ''
        print '      Longest Euclidean distance between tasks: {}'.format(longestDistance)

        #Calculate normalizing factors for each vehicle (based on speed, so can be different per vehicle)
        for vehicle in vehicleVector:
            vehicle.normFactor = longestDistance/vehicle.speed

        ### Create arrays for storing data

        ### MAIN SIM LOOP ###
        print ''
        print '   ******    Beginning Simulation   **********'
        print ''

        # visitOrder = np.zeros((taskStarts+1, 3))
        visitOrder = []        
        for taskStart in xrange(1, taskStarts+1):

            print '   Task Start #[{}]\n'.format(taskStart)

            # Decide which vehicle makes the next task selection, based on earliest arrival time
            decider = min(vehicleVector, key=lambda x: x.targets[x._indexer, 1])
            print '   Vehicle', decider.ID, 'is selecting the next task.'
            print '      Just arrived: {}'.format(decider.targets[decider._indexer, :])

            ### Documentation time...save data for later analysis
            #Save task visit times
            visitOrder.append([decider.ID, decider.targets[decider._indexer, 0], decider.targets[decider._indexer, 1]])

            #Zero-out age of visited task in deciding vehicle's own age tracker
            decider.ageTracker[int(decider.targets[decider._indexer, 0])-1] = 0.0
            print '      Current ageTracker = {}'.format(np.around(decider.ageTracker, 3))

            #Update current vehicle location (old destination is new location)
            decider.location[0] = decider.targets[decider._indexer, 0]
            decider.location[1] = decider.targets[decider._indexer, 1]
            
            ### Calculate travel times from current location to all possible destinations 
            # These are the actual travel times, based on vehicle flight dynamics 
            travelTimesFlight, headings = decider.calctraveldub(taskVector)

            # These are the travel times that will be used for decision making (not necessarily the actual flight times)
            if taskSelectionDistanceMeasure == 'euclidean':
                travelTimesMeasure = decider.calctraveleuc(taskVector)            
            if taskSelectionDistanceMeasure == 'dubins':
                travelTimesMeasure = travelTimesFlight

            print '      Actual flight times: {}'.format(np.around(travelTimesFlight, 3))
            print '      Arrival headings: {}'.format(np.around(headings, 1))
            print '      Measurement flight times: {}'.format(np.around(travelTimesMeasure, 3))

            #Select next target
            decider.selecttask(travelTimesFlight, travelTimesMeasure, headings)
            print '      New target: {}'.format(decider.targets[decider._indexer, :])
            print '      Arrival heading: {}'.format(decider.heading)

            #Increment all ages in vehicle's own age tracker by travel time
            decider.ageTracker = np.add(decider.ageTracker, np.ones(decider.ageTracker.shape[0])*decider.targets[decider._indexer, 2])

            ### Time to communicate, based on the mode
            if commMode == 'none':
                decider.cxnone()

            if commMode == 'CxBC':
                decider.cxbc(vehicleVector)

            if commMode == 'CxBD':
                decider.cxbc(vehicleVector)
                decider.cxbd(vehicleVector)

            ### With all that done, it's time for the next vehicle to decide...the loop restarts

            print ''
            print '   Ready for next Task!\n'
            print '   *******************************************\n'

        #Save the final task visits for each vehicle
        for vehicle in vehicleVector:
            visitOrder = np.append(visitOrder, [[vehicle.ID, vehicle.targets[vehicle._indexer, 0], vehicle.targets[vehicle._indexer, 1]]], axis=0)

        visitOrder = sorted(visitOrder,key=lambda x: x[2])
        visitOrder = np.array(visitOrder) 

        
        print ''
        print '   ******     Simulation Complete   ***********'
        print ''

        #Display the visit history to screen
        print '   Simulation Visit History:'        
        print(visitOrder)
        print ''

        #Write the sim data for this trade to the sim data XML
        configFile.write('\t<Trade>\n')
        configFile.write('\t\t<tradeID>{}</tradeID>\n'.format(tradeID))
        configFile.write('\t\t<taskSelectionMethod>{}</taskSelectionMethod>\n'.format(taskSelectionMethod))
        configFile.write('\t\t<beta>{}</beta>\n'.format(vehicleVector[0].beta))
        configFile.write('\t\t<w>{}</w>\n'.format(vehicleVector[0].w))
        configFile.write('\t\t<normFactor>{}</normFactor>\n'.format(vehicleVector[0].normFactor))
        configFile.write('\t\t<taskGeometry>{}</taskGeometry>\n'.format(taskGeometry))
        configFile.write('\t\t<commMode>{}</commMode>\n'.format(commMode))        
        for visit in visitOrder:
            configFile.write('\t\t<Visit>\n')
            configFile.write('\t\t\t<Vehicle>{}</Vehicle>\n'.format(visit[0]))
            configFile.write('\t\t\t<Task>{}</Task>\n'.format(visit[1]))
            configFile.write('\t\t\t<Priority>{}</Priority>\n'.format(taskVector[int(visit[1]-1)].priority))
            configFile.write('\t\t\t<Time>{}</Time>\n'.format(visit[2]))
            configFile.write('\t\t</Visit>\n')
        configFile.write('\t</Trade>\n')
        
    #Closeout the sim data XML file after all trades are complete
    configFile.write('</Sim_Data>')
    configFile.close()

        
    # results = {'visitOrder':visitOrder}
    # resultsPickle = '{0}Data/Trade_{1}_Results.pickle'.format(simPath, tradeID)
    # # pickle.dump(visitOrder, open(resultsPickle, "wb"))
    # with open(resultsPickle, "wb") as f:
    #     pickle.dump(results, f)


if __name__ == '__main__':

    main()