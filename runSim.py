from __future__ import division
# import itertools
# import os
# import shutil
# import matplotlib.pyplot as plt

# import pandas as pd

import sys
import math
import xml.etree.ElementTree
import numpy as np

from operator import attrgetter, itemgetter

import VehicleClass
import TaskClass


# def loadConfigFiles():    

def loadScenarioConfig(trade):

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

    print '\n'

    return taskSelectionMethod, taskSelectionDistanceMeasure, taskStarts, commMode


def loadTaskConfig(trade):

    print '      Loading task configurations...' 

    taskinfo = trade.find('Task_Info')  
    taskGeometry = taskinfo.find('TaskGeometry').text
    numTasks = len(taskinfo.findall('Task'))
    
    print '         There are', numTasks, 'tasks ({}):\n'.format(taskGeometry)

    taskVector = []
    for task in taskinfo.findall('Task'):
        taskID = int(task.find('ID').text)        
        print '         Task', taskID
        xCoord = float(task.find('xCoord').text)
        yCoord = float(task.find('yCoord').text)
        print '            Location: (', xCoord, ',', yCoord, ')'
        priority = float(task.find('priority').text)
        print '            Priority: ', priority, '\n'   
        
        taskObj = TaskClass.Task(taskID, xCoord, yCoord, priority)  #INSTATIATE TASK OBJECT  
        taskVector.append(taskObj)    #ADD OBJECT TO VECTOR OF TASK OBJECTS

    return taskVector, taskGeometry

def loadVehicleConfig(trade, taskSelectionMethod):

    print '      Loading vehicle configurations...'

    vehinfo = trade.find('Vehicle_Info')
    numVehicles = len(vehinfo.findall('Vehicle'))
   
    print '         There are', numVehicles, 'vehicles:\n'

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

        if taskSelectionMethod == 'manual':
            tour = eval(vehicle.find('tour').text)
            print '            Tour={}'.format(tour)
            vehicleObj = VehicleClass.VehicleManual(index, vehicleID, initLocation, initHeading, vehSpeed, vehBankAngle,
            numVehicles, tour) #INSTATIATE VEHICLE OBJECT        
                
        if taskSelectionMethod == 'md2wrp':
            beta = float(vehicle.find('beta').text)
            print '            Beta={}'.format(beta)
            weightVector = eval(vehicle.find('w').text)
            print '            Weight Vector={}'.format(weightVector)      
            vehicleObj = VehicleClass.VehicleMD2WRP(index, vehicleID, initLocation, initHeading, vehSpeed, vehBankAngle,
            numVehicles, beta, weightVector) #INSTATIATE VEHICLE OBJECT

        print '\n' 
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

    #Parse the simulation configuration file
    e = xml.etree.ElementTree.parse(simPath+'simConfiguration.xml').getroot()

    #Perform a simulation for each trade
    for trade in e.findall('Trade'):
        tradeID = int(trade.find('tradeID').text)
        print '   Loading TradeID={}'.format(tradeID)

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
        print 'Longest Euclidean distance between tasks: {}'.format(longestDistance)

        #Calculate normalizing factors for each vehicle (based on speed, so can be different per vehicle)
        for vehicle in vehicleVector:
            vehicle.normFactor = longestDistance/vehicle.speed

        ### Create arrays for storing data

        ### MAIN SIM LOOP ###
        print ''
        print '*******************************************'
        print '*********Beginning Simulation**************'
        print '*******************************************'
        print ''

        visitOrder = np.zeros((taskStarts+1, 3))
        for taskStart in xrange(1, taskStarts+1):

            print 'Task Start #[{}]\n'.format(taskStart)

            # Decide which vehicle makes the next task selection, based on earliest arrival time
            decider = min(vehicleVector, key=lambda x: x.targets[x._indexer, 1])
            print 'Vehicle', decider.ID, 'is selecting the next task.'
            print '   Just arrived: {}'.format(decider.targets[decider._indexer, :])

            ### Documentation time...save data for later analysis
            #Save task visit times
            visitOrder[taskStart-1] = [decider.ID, decider.targets[decider._indexer, 0], decider.targets[decider._indexer, 1]]

            #Update age of visited task in deciding vehicle's own age tracker
            decider.ageTracker[int(decider.targets[decider._indexer, 0])-1] = 0.0
            print '   Current ageTracker = {}'.format(np.around(decider.ageTracker, 3))

            #Update current vehicle location
            decider.location[0] = decider.targets[decider._indexer, 0]
            decider.location[1] = decider.targets[decider._indexer, 1]         

            ### Implement the actual task selection algorithm (the main work right here)        
            
            #Calculate *measurement* travel times from current location to all possible destinations
            if taskSelectionDistanceMeasure == 'euclidean':
                travelTimes = decider.calctraveleuc(taskVector)
                print '   Travel time to each task: {}'.format(np.around(travelTimes, 3))
            # travelTimes = [.3, .5, 1]

            #Select next target
            decider.selecttask(travelTimes)
            print '   New target: {}'.format(decider.targets[decider._indexer, :])

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

            ### All data should be documented, so now just close things out!

            print '\nReady for next Task!\n'
            print '*******************************************\n'

        #Save the last task visit time
        visitOrder[taskStart] = [decider.ID, decider.targets[decider._indexer, 0], decider.targets[decider._indexer, 1]]

        print ''
        print '*******************************************'
        print '*********Simulation Complete***************'
        print '*******************************************'
        print ''

        #Display the visit history to screen
        print 'Simulation Visit History:'
        visitOrder[:,2] = visitOrder[:,2]*vehicleVector[0].normFactor
        print(visitOrder)

    # print ''
    # print '******   Packaging Results   **************'
    # print ''

    # simDataPath = './Data/'   

    # print 'Saving simulation data to {}'.format(simDataPath)




    # results = {'visitOrder':visitOrder}
    # resultsPickle = '{0}Data/Trade_{1}_Results.pickle'.format(simPath, tradeID)
    # # pickle.dump(visitOrder, open(resultsPickle, "wb"))
    # with open(resultsPickle, "wb") as f:
    #     pickle.dump(results, f)


if __name__ == '__main__':

    main()