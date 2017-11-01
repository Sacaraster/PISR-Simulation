from __future__ import division
# import itertools
# import os
# import matplotlib.pyplot as plt
# import pandas as pd

import shutil
import sys
import math
import dubins
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

    saveTrajectories = int(float(scen.find('saveTrajectories').text))

    for item in scen.findall('taskStarts'):
        taskStarts = int(item.text)
        print '         Task starts:', taskStarts

    for item in scen.findall('commMode'):
        commMode = item.get('name')
        print '         Comm Mode:', commMode

    return taskSelectionMethod, taskSelectionDistanceMeasure, saveTrajectories, taskStarts, commMode


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
        initHeading = float(vehicle.find('initHeading').text)*(math.pi/180)          #store in radians
        print '            Initial Heading:',initHeading*(180/math.pi), 'degrees.'     #output in degrees
        vehSpeed = float(vehicle.find('vehSpeed').text)
        print '            Vehicle Speed:', vehSpeed
        vehBankAngle = float(vehicle.find('vehBankAngle').text)*(math.pi/180)        #store in radians
        print '            Vehicle Bank Angle: ', vehBankAngle*(180/math.pi)         #output in degrees
        turnRadius = vehSpeed**2/(9.807*math.tan(vehBankAngle))

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


def calcDistanceMatrixData(taskVector):

    cxyVector = []
    for task in taskVector:
        x = task.location[0]
        y = task.location[1]
        cxy = x+y*1j
        cxyVector.append(cxy)
    cxyVector = np.array([cxyVector], dtype=complex)

    distanceMatrix = abs(cxyVector.T-cxyVector)

    longestDistance = np.max(distanceMatrix)
    avgDistance = np.sum(distanceMatrix)/((distanceMatrix.shape[0]**2)-distanceMatrix.shape[0])   #don't divide by diaganol entries, which are zero


    # distVector = []
    # for fromTask in taskVector:
    #     for toTask in taskVector:
    #         dist = math.sqrt(math.pow(fromTask.location[0]-toTask.location[0], 2)+
    #             math.pow(fromTask.location[1]-toTask.location[1], 2))
    #         distVector.append(dist)

    # distVector = np.array(distVector)
    # longestDistance = distVector.max()

    return avgDistance, longestDistance

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
    dataFile = open(fileName,'w')
    dataFile.write('<Sim_Data>\n')

    #Parse the simulation configuration file
    e = ET.parse(simPath+'simConfiguration.xml').getroot()

    #Perform a simulation for each trade
    for trade in e.findall('Trade'):
        tradeID = int(trade.find('tradeID').text)
        print '***************************************'
        print '          Loading TradeID={}        '.format(tradeID)
        print '***************************************'

        # Parse and load the scenario configuration parameters
        taskSelectionMethod, taskSelectionDistanceMeasure, saveTrajectories, taskStarts, commMode = loadScenarioConfig(trade)     

        #Parse and load the task configuration; returns a vector of task objects
        taskVector, taskGeometry = loadTaskConfig(trade)
    
        #Parse and load the vehicle configuration; returns a vector of vehicle objects
        vehicleVector = loadVehicleConfig(trade, taskSelectionMethod)
        
    
    ######################################################################
    ######################################################################    
    
        np.set_printoptions(suppress=True)
        np.set_printoptions(threshold='nan')

        #Calculate longest Euclidean distance on the map (for normalization)
        avgDistance, longestDistance = calcDistanceMatrixData(taskVector)
        print ''
        print '      Average value of distance matrix: {}'.format(avgDistance)
        print '      Longest Euclidean distance between tasks: {}'.format(longestDistance)
        print '      Vehicle turn radius: ', vehicleVector[0].turnRadius
        rdRatio = vehicleVector[0].turnRadius/avgDistance
        print '      r/d for scenario: {}'.format(rdRatio)

        #Calculate normalizing factors for each vehicle (based on speed, so can be different per vehicle)
        for vehicle in vehicleVector:
            vehicle.normFactor = longestDistance/vehicle.speed            
            # vehicle.normFactor = 1     #<----Don't normalize

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
            print '      Current heading: {} degrees'.format(decider.heading*(180/math.pi))

            ### Documentation time...save data for later analysis
            #Save task visit times and flight trjactory
            #    format is [vehicleID, task, visit_time, trajectory]
            visitOrder.append([decider.ID, decider.targets[decider._indexer, 0], decider.targets[decider._indexer, 1], decider.trajectory])
            
            #Zero-out age of visited task in deciding vehicle's own age tracker
            decider.ageTracker[int(decider.targets[decider._indexer, 0])-1] = 0.0
            print '      Current ageTracker = {}'.format(np.around(decider.ageTracker, 3))

            #Update current vehicle location (old destination is new location)
            decider.location[0] = decider.targets[decider._indexer, 0]
            decider.location[1] = decider.targets[decider._indexer, 1]
            
            #Calculate shortest travel times and associated arrival headings from current location to all possible destinations 
            #   These are the actual flight times, based on vehicle flight dynamics 
            travelTimesFlight, headings = decider.calctraveldub(taskVector)

            # These are the travel times that will be used for decision making (could be Dubins or Euclidean)
            if taskSelectionDistanceMeasure == 'euclidean':
                travelTimesMeasure = decider.calctraveleuc(taskVector)            
            if taskSelectionDistanceMeasure == 'dubins':
                travelTimesMeasure = travelTimesFlight

            print '      Actual travel times: {}'.format(np.around(travelTimesFlight, 3))
            print '      Arrival headings: {}'.format(np.around(headings*(180/math.pi), 1))
            print '      Measure travel times: {}'.format(np.around(travelTimesMeasure, 3))

            #Select next Task
            selectedTask = decider.selecttask(travelTimesMeasure)            

            #Calculate trajectory to selected Task           
            trajectory = decider.calctrajectory(selectedTask, taskVector, headings)
            
            #Update vehicle state
            travelTimeFlight = travelTimesFlight[selectedTask-1]
            decider.targets[decider._indexer, 0] = selectedTask
            decider.targets[decider._indexer, 1] += travelTimeFlight
            decider.targets[decider._indexer, 2] = travelTimeFlight
            decider.heading = headings[selectedTask-1]
            decider.trajectory = trajectory  
            print '      New target: {}'.format(decider.targets[decider._indexer, :])
            print '      Arrival heading: {}'.format(decider.heading*(180/math.pi))

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

            print ''
            print '   Ready for next Task!\n'
            print '   *******************************************\n'

        #Save the final task visits for each vehicle
        for vehicle in vehicleVector:
            visitOrder.append([vehicle.ID, vehicle.targets[vehicle._indexer, 0], vehicle.targets[vehicle._indexer, 1], vehicle.trajectory])

        visitOrder = sorted(visitOrder,key=lambda x: x[2])
        visitOrder = np.array(visitOrder, dtype=object) 

        
        print ''
        print '   ******     Simulation Complete   ***********'
        print ''

        #Display the visit history to screen
        print '   Simulation Visit History:'        
        print(visitOrder[:, 0:3])
        print ''

        #Write the sim data for this trade to the sim data XML
        dataFile.write('\t<Trade>\n')
        dataFile.write('\t\t<tradeID>{}</tradeID>\n'.format(tradeID))
        dataFile.write('\t\t<taskSelectionMethod>{}</taskSelectionMethod>\n'.format(taskSelectionMethod))
        dataFile.write('\t\t<saveTrajectories>{}</saveTrajectories>\n'.format(saveTrajectories))
        dataFile.write('\t\t<beta>{}</beta>\n'.format(vehicleVector[0].beta))
        dataFile.write('\t\t<w>{}</w>\n'.format(vehicleVector[0].w))
        dataFile.write('\t\t<normFactor>{}</normFactor>\n'.format(vehicleVector[0].normFactor))
        dataFile.write('\t\t<rdRatio>{}</rdRatio>\n'.format(rdRatio))        
        dataFile.write('\t\t<commMode>{}</commMode>\n'.format(commMode))
        dataFile.write('\t\t<taskGeometry>{}</taskGeometry>\n'.format(taskGeometry))
        for task in taskVector:
            dataFile.write('\t\t<Tasks>\n')
            dataFile.write('\t\t\t<Task>{}</Task>\n'.format(task.ID))
            dataFile.write('\t\t\t<xCoord>{}</xCoord>\n'.format(task.location[0]))
            dataFile.write('\t\t\t<yCoord>{}</yCoord>\n'.format(task.location[1]))  
            dataFile.write('\t\t\t<Priority>{}</Priority>\n'.format(task.priority))          
            dataFile.write('\t\t</Tasks>\n')
        for visit in visitOrder:
            dataFile.write('\t\t<Visit>\n')
            dataFile.write('\t\t\t<Vehicle>{}</Vehicle>\n'.format(visit[0]))
            dataFile.write('\t\t\t<Task>{}</Task>\n'.format(visit[1]))
            dataFile.write('\t\t\t<Time>{}</Time>\n'.format(visit[2]))
            if saveTrajectories:
                dataFile.write('\t\t\t<Trajectory>{}</Trajectory>\n'.format(visit[3]))
            dataFile.write('\t\t</Visit>\n')
        dataFile.write('\t</Trade>\n')
        
    #Closeout the sim data XML file after all trades are complete
    dataFile.write('</Sim_Data>')
    dataFile.close()

        
    # results = {'visitOrder':visitOrder}
    # resultsPickle = '{0}Data/Trade_{1}_Results.pickle'.format(simPath, tradeID)
    # # pickle.dump(visitOrder, open(resultsPickle, "wb"))
    # with open(resultsPickle, "wb") as f:
    #     pickle.dump(results, f)


if __name__ == '__main__':

    main()