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

from Classes.TaskClass import Task
from Classes.VehicleClass import Vehicle



def loadTaskConfig(trade):     

    task_info = trade.find('Task_Info')  
    task_geometry = task_info.find('Task_geometry').text
    tasks = task_info.findall('Task')
    numTasks = len(tasks)
    
    print ''
    print '      Instantiating task objects ({} tasks, "{}" map)...'.format(numTasks, task_geometry)

    task_vector = []
    for task in tasks:
        taskID = int(task.find('ID').text)
        x_coord = float(task.find('xCoord').text)
        y_coord = float(task.find('yCoord').text)
        priority = float(task.find('priority').text)
        init_age = float(task.find('init_age').text)
        t_activate = float(task.find('task_t_activate').text)
        t_terminate = float(task.find('task_t_terminate').text)
        print '         Task {} @ ({},{}), Priority={}'.format(taskID, x_coord, y_coord, priority)
        taskObj = Task(taskID, x_coord, y_coord, priority, init_age, t_activate, t_terminate)  #INSTATIATE TASK OBJECT  
        task_vector.append(taskObj)    #ADD OBJECT TO VECTOR OF TASK OBJECTS

    return task_vector, task_geometry

def loadVehicleConfig(trade):    

    veh_info = trade.find('Vehicle_Info')
    vehicles = veh_info.findall('Vehicle')
    numVehicles = len(vehicles)
   
    print ''
    print '      Instantiating vehicle objects ({} vehicles)...'.format(numVehicles)

    vehicle_vector = []
    for index, vehicle in enumerate(vehicles):
        vehicleID = int(vehicle.find('ID').text)
        print '         Vehicle', vehicleID
        init_location = int(vehicle.find('init_location').text)
        print '            Initial Location: Task', init_location
        init_heading = float(vehicle.find('init_heading').text)*(math.pi/180)          #store in radians
        print '            Initial Heading:',init_heading*(180/math.pi), 'degrees.'     #output in degrees
        veh_speed = float(vehicle.find('veh_speed').text)
        print '            Vehicle Speed:', veh_speed, 'meters/sec.'
        veh_bank_angle = float(vehicle.find('veh_bank_angle').text)*(math.pi/180)        #store in radians
        print '            Vehicle Bank Angle: ', veh_bank_angle*(180/math.pi), 'degrees.'         #output in degrees
        turn_radius = veh_speed**2/(9.807*math.tan(veh_bank_angle))
        print '            Vehicle Turn Radius: ', np.around(turn_radius,1), 'meters.'
        veh_t_activate = float(vehicle.find('veh_t_activate').text)
        print '            Vehicle Activation Time: ', veh_t_activate, 'secs.'
        veh_t_terminate = float(vehicle.find('veh_t_terminate').text)
        print '            Vehicle Termination Time: ', veh_t_terminate, 'secs.'

        vehicleObj = Vehicle(index, vehicleID, init_location, init_heading, veh_speed, turn_radius, veh_t_activate, veh_t_terminate)
    
        vehicle_vector.append(vehicleObj)

    return vehicle_vector

def loadRoutingConfig(trade, vehicle_vector):
    routing_info = trade.find('Routing_Info')
    routing_data = []
    for index, entry in enumerate(routing_info):
        if index == 0:
            routing_data.append(entry.text)
        else:
            routing_data.append(eval(entry.text))

    for vehicle in vehicle_vector:
        vehicle.add_routing(routing_data)

    return vehicle_vector


def calcDistanceMatrixData(task_vector):

    cxyVector = []
    for task in task_vector:
        x = task.location[0]
        y = task.location[1]
        cxy = x+y*1j
        cxyVector.append(cxy)
    cxyVector = np.array([cxyVector], dtype=complex)

    distanceMatrix = abs(cxyVector.T-cxyVector)

    longestDistance = np.max(distanceMatrix)
    avgDistance = np.sum(distanceMatrix)/((distanceMatrix.shape[0]**2)-distanceMatrix.shape[0])   #don't divide by diaganol entries, which are zero


    # distVector = []
    # for fromTask in task_vector:
    #     for toTask in task_vector:
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
    sim_path = sys.argv[1]

    #Create and open the XML where simulation data will be saved
    sim_data_path = './Data/'    
    file_name = sim_data_path + 'sim_data.xml'
    print '\nSimulation data will be saved to {}\n'.format(file_name)
    dataFile = open(file_name,'w')
    dataFile.write('<Sim_Data>\n')

    #Parse the simulation configuration files
    e = ET.parse(sim_path+'sim_configuration.xml').getroot()

    #For each trade, parse the configuration data and instantiate associated objects
    for trade in e.findall('Trade'):
        
        # Parse and load the meta parameters
        tradeID = int(trade.find('tradeID').text)
        sim_length_visits = float(trade.find('sim_length_visits').text)
        sim_length_time = float(trade.find('sim_length_time').text)        
        print '***************************************'
        print '          Loading TradeID={}        '.format(tradeID)
        print '***************************************'
   
        #Parse and load the task parameters; returns a vector of task objects
        task_vector, task_geometry = loadTaskConfig(trade)
    
        #Parse and load the vehicle parameters; returns a vector of vehicle objects
        vehicle_vector = loadVehicleConfig(trade)

        #Parse and load the routing parameters; returns vehicle objects with routing modules added
        vehicle_vector = loadRoutingConfig(trade, vehicle_vector)

        #Prase and load the pathing parameters; returns vehicle objects with pathing modules added

        #Prase and load the communication parameters; returns vehicle objects with comm modules added

        #Prase and load the database parameters; returns vehicle objects with database modules added
        
    
    # ######################################################################
    # ######################################################################    
    
    #     np.set_printoptions(suppress=True)
    #     np.set_printoptions(threshold='nan')

    #     #Calculate longest Euclidean distance on the map (for normalization)
    #     avgDistance, longestDistance = calcDistanceMatrixData(task_vector)
    #     print ''
    #     print '      Average value of distance matrix: {}'.format(avgDistance)
    #     print '      Longest Euclidean distance between tasks: {}'.format(longestDistance)
    #     print '      Vehicle turn radius: ', vehicle_vector[0].turn_radius
    #     rdRatio = vehicle_vector[0].turn_radius/avgDistance
    #     print '      r/d for scenario: {}'.format(rdRatio)

    #     #Calculate normalizing factors for each vehicle (based on speed, so can be different per vehicle)
    #     for vehicle in vehicle_vector:
    #         vehicle.normFactor = longestDistance/vehicle.speed            
    #         # vehicle.normFactor = 1     #<----Don't normalize

    #     ### MAIN SIM LOOP ###
    #     print ''
    #     print '   ******    Beginning Simulation   **********'
    #     print ''

    #     # visitOrder = np.zeros((taskStarts+1, 3))
    #     visitOrder = []        
    #     for taskStart in xrange(1, taskStarts+1):

    #         print '   Task Start #[{}]\n'.format(taskStart)

    #         # Decide which vehicle makes the next task selection, based on earliest arrival time
    #         decider = min(vehicle_vector, key=lambda x: x.targets[x._indexer, 1])
    #         print '   Vehicle', decider.ID, 'is selecting the next task.'
    #         print '      Just arrived: {}'.format(decider.targets[decider._indexer, :])
    #         print '      Current heading: {} degrees'.format(decider.heading*(180/math.pi))

    #         ### Documentation time...save data for later analysis
    #         #Save task visit times and flight trjactory
    #         #    format is [vehicleID, task, visit_time, trajectory]
    #         visitOrder.append([decider.ID, decider.targets[decider._indexer, 0], decider.targets[decider._indexer, 1], decider.trajectory])
            
    #         #Zero-out age of visited task in deciding vehicle's own age tracker
    #         decider.ageTracker[int(decider.targets[decider._indexer, 0])-1] = 0.0
    #         print '      Current ageTracker = {}'.format(np.around(decider.ageTracker, 3))

    #         #Update current vehicle location (old destination is new location)
    #         decider.location[0] = decider.targets[decider._indexer, 0]
    #         decider.location[1] = decider.targets[decider._indexer, 1]
            
    #         #Calculate shortest travel times and associated arrival headings from current location to all possible destinations 
    #         #   These are the actual flight times, based on vehicle flight dynamics 
    #         travelTimesFlight, headings = decider.calctraveldub(task_vector)

    #         # These are the travel times that will be used for decision making (could be Dubins or Euclidean)
    #         if taskSelectionDistanceMeasure == 'euclidean':
    #             travelTimesMeasure = decider.calctraveleuc(task_vector)            
    #         if taskSelectionDistanceMeasure == 'dubins':
    #             travelTimesMeasure = travelTimesFlight

    #         print '      Actual travel times: {}'.format(np.around(travelTimesFlight, 3))
    #         print '      Arrival headings: {}'.format(np.around(headings*(180/math.pi), 1))
    #         print '      Measure travel times: {}'.format(np.around(travelTimesMeasure, 3))

    #         #Select next Task
    #         selectedTask = decider.selecttask(travelTimesMeasure)            

    #         #Calculate trajectory to selected Task           
    #         trajectory = decider.calctrajectory(selectedTask, task_vector, headings)
            
    #         #Update vehicle state
    #         travelTimeFlight = travelTimesFlight[selectedTask-1]
    #         decider.targets[decider._indexer, 0] = selectedTask
    #         decider.targets[decider._indexer, 1] += travelTimeFlight
    #         decider.targets[decider._indexer, 2] = travelTimeFlight
    #         decider.heading = headings[selectedTask-1]
    #         decider.trajectory = trajectory  
    #         print '      New target: {}'.format(decider.targets[decider._indexer, :])
    #         print '      Arrival heading: {}'.format(decider.heading*(180/math.pi))

    #         #Increment all ages in vehicle's own age tracker by travel time
    #         decider.ageTracker = np.add(decider.ageTracker, np.ones(decider.ageTracker.shape[0])*decider.targets[decider._indexer, 2])

    #         ### Time to communicate, based on the mode
    #         if commMode == 'none':
    #             decider.cxnone()

    #         if commMode == 'CxBC':
    #             decider.cxbc(vehicle_vector)

    #         if commMode == 'CxBD':
    #             decider.cxbc(vehicle_vector)
    #             decider.cxbd(vehicle_vector)

    #         print ''
    #         print '   Ready for next Task!\n'
    #         print '   *******************************************\n'

    #     #Save the final task visits for each vehicle
    #     for vehicle in vehicle_vector:
    #         visitOrder.append([vehicle.ID, vehicle.targets[vehicle._indexer, 0], vehicle.targets[vehicle._indexer, 1], vehicle.trajectory])

    #     visitOrder = sorted(visitOrder,key=lambda x: x[2])
    #     visitOrder = np.array(visitOrder, dtype=object) 

        
    #     print ''
    #     print '   ******     Simulation Complete   ***********'
    #     print ''

    #     #Display the visit history to screen
    #     print '   Simulation Visit History:'        
    #     print(visitOrder[:, 0:3])
    #     print ''

    #     #Write the sim data for this trade to the sim data XML
    #     dataFile.write('\t<Trade>\n')
    #     dataFile.write('\t\t<tradeID>{}</tradeID>\n'.format(tradeID))
    #     dataFile.write('\t\t<taskSelectionMethod>{}</taskSelectionMethod>\n'.format(taskSelectionMethod))
    #     dataFile.write('\t\t<saveTrajectories>{}</saveTrajectories>\n'.format(saveTrajectories))
    #     dataFile.write('\t\t<beta>{}</beta>\n'.format(vehicle_vector[0].beta))
    #     dataFile.write('\t\t<w>{}</w>\n'.format(vehicle_vector[0].w))
    #     dataFile.write('\t\t<normFactor>{}</normFactor>\n'.format(vehicle_vector[0].normFactor))
    #     dataFile.write('\t\t<rdRatio>{}</rdRatio>\n'.format(rdRatio))        
    #     dataFile.write('\t\t<commMode>{}</commMode>\n'.format(commMode))
    #     dataFile.write('\t\t<task_geometry>{}</task_geometry>\n'.format(task_geometry))
    #     for task in task_vector:
    #         dataFile.write('\t\t<Tasks>\n')
    #         dataFile.write('\t\t\t<Task>{}</Task>\n'.format(task.ID))
    #         dataFile.write('\t\t\t<x_coord>{}</x_coord>\n'.format(task.location[0]))
    #         dataFile.write('\t\t\t<y_coord>{}</y_coord>\n'.format(task.location[1]))  
    #         dataFile.write('\t\t\t<Priority>{}</Priority>\n'.format(task.priority))          
    #         dataFile.write('\t\t</Tasks>\n')
    #     for visit in visitOrder:
    #         dataFile.write('\t\t<Visit>\n')
    #         dataFile.write('\t\t\t<Vehicle>{}</Vehicle>\n'.format(visit[0]))
    #         dataFile.write('\t\t\t<Task>{}</Task>\n'.format(visit[1]))
    #         dataFile.write('\t\t\t<Time>{}</Time>\n'.format(visit[2]))
    #         if saveTrajectories:
    #             dataFile.write('\t\t\t<Trajectory>{}</Trajectory>\n'.format(visit[3]))
    #         dataFile.write('\t\t</Visit>\n')
    #     dataFile.write('\t</Trade>\n')
        
    #Closeout the sim data XML file after all trades are complete
    dataFile.write('</Sim_Data>')
    dataFile.close()

        
    # results = {'visitOrder':visitOrder}
    # resultsPickle = '{0}Data/Trade_{1}_Results.pickle'.format(sim_path, tradeID)
    # # pickle.dump(visitOrder, open(resultsPickle, "wb"))
    # with open(resultsPickle, "wb") as f:
    #     pickle.dump(results, f)


if __name__ == '__main__':

    main()