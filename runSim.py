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
    
    print ''
    print '      Instantiating task objects ({} tasks, "{}" map)...'.format(len(tasks), task_geometry)

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

def loadVehicleConfig(trade, task_vector):    

    veh_info = trade.find('Vehicle_Info')
    vehicles = veh_info.findall('Vehicle')
   
    print ''
    print '      Instantiating vehicle objects ({} vehicles)...'.format(len(vehicles))

    vehicle_vector = []
    for index, vehicle in enumerate(vehicles):
        vehicleID = int(vehicle.find('ID').text)
        print '         Vehicle', vehicleID
        init_location = int(vehicle.find('init_location').text)
        init_location = task_vector[init_location-1]   #re-assign init_location to be a task object
        print '            Initial Task:', init_location.ID
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

        #Instantiate the vehicle object
        vehicleObj = Vehicle(index, vehicleID, init_location, init_heading, veh_speed, turn_radius, veh_t_activate, veh_t_terminate) 

        vehicle_vector.append(vehicleObj)

    #Parse and load the modules for each vehicle
    vehicle_vector = loadRoutingConfig(vehicles, vehicle_vector, task_vector)
    vehicle_vector = loadPathingConfig(vehicles, vehicle_vector)
    vehicle_vector = loadCommConfig(vehicles, vehicle_vector)
    vehicle_vector = loadDatabaseConfig(vehicles, vehicle_vector, task_vector)
  
    return vehicle_vector

def loadRoutingConfig(vehicles, vehicle_vector, task_vector):
    
    print ''
    print '      Adding Routing modules...'

    for index, vehicle in enumerate(vehicles):        
        veh_routing_info = vehicle.find('Routing_Info')
        routing_data = []
        for ind, entry in enumerate(veh_routing_info):
            if ind == 0:
                routing_data.append(entry.text)
            else:
                routing_data.append(eval(entry.text))        
   
        vehicle_vector[index].add_routing(routing_data, task_vector)

        print '         Vehicle {} Routing Data: {}'.format(vehicle_vector[index].ID, routing_data)

    return vehicle_vector

def loadPathingConfig(vehicles, vehicle_vector):

    print ''
    print '      Adding Pathing modules...'

    for index, vehicle in enumerate(vehicles):        
        veh_pathing_info = vehicle.find('Pathing_Info')
        pathing_data = []
        for ind, entry in enumerate(veh_pathing_info):
            if ind == 0:
                pathing_data.append(entry.text)
            else:
                pathing_data.append(eval(entry.text))        
   
        vehicle_vector[index].add_pathing(pathing_data)

        print '         Vehicle {} Pathing Data: {}'.format(vehicle_vector[index].ID, pathing_data)

    return vehicle_vector

def loadCommConfig(vehicles, vehicle_vector):

    print ''
    print '      Adding Communication modules...'

    for index, vehicle in enumerate(vehicles):        
        veh_comm_info = vehicle.find('Comm_Info')
        comm_data = []
        for ind, entry in enumerate(veh_comm_info):
            if ind == 0:
                comm_data.append(entry.text)
            else:
                comm_data.append(eval(entry.text))

        vehicle_vector[index].add_comm(comm_data)

        print '         Vehicle {} Comm Data: {}'.format(vehicle_vector[index].ID, comm_data)

    return vehicle_vector

def loadDatabaseConfig(vehicles, vehicle_vector, task_vector):
    
    print ''
    print '      Adding Database modules...'

    for index, vehicle in enumerate(vehicles):
        veh_database_info = vehicle.find('Database_Info')
        database_items = []
        for ind, entry in enumerate(veh_database_info):
            database_items.append(entry.text)
    
        vehicle_vector[index].add_database(database_items, vehicle_vector, task_vector)

        print '         Vehicle {} Database Items: {}'.format(vehicle_vector[index].ID, database_items)

    return vehicle_vector


def main():

    ######################################################################
    ###            LOAD CONFIGURATION FILES AND INSANTIATE OBJECTS   #####
    ######################################################################    
    #Argument supplying the location of the simulation configuration files
    sim_path = sys.argv[1]

    #Create and open the XML where simulation data will be saved
    sim_data_path = './Data/'    
    # file_name = sim_data_path + 'sim_data.xml'
    print '\nSimulation data will be saved to {}\n'.format(sim_data_path)
    # dataFile = open(file_name,'w')
    # dataFile.write('<Sim_Data>\n')

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
   
        #Load the task parameters; returns a vector of task objects
        task_vector, task_geometry = loadTaskConfig(trade)

        #Load the vehicle parameters and all modules; returns a vector of vehicle objects
        vehicle_vector = loadVehicleConfig(trade, task_vector)     
    
    ######################################################################
    ######################################################################    
    
        np.set_printoptions(suppress=True)   #Don't print in scientific notation
        np.set_printoptions(threshold='nan')   #Don't truncate large arrays when printing        

        ### MAIN SIM LOOP ###
        print ''
        print '   *******************************************'
        print '   ******    Beginning Simulation   **********'
        print '   *******************************************'

        visit_order = []        
        task_ages = []        
        visit_num = 1
        time = 0
        while ((visit_num < sim_length_visits+1) & (time < sim_length_time+1)):

            print '   Task Visit #[{}]\n'.format(visit_num)

            # Decide which vehicle makes the next task selection, based on earliest arrival time
            # Only vehicle's within their active window are considered
            decider = min((vehicle for vehicle in vehicle_vector if (vehicle.t_terminate > vehicle.routing.arrival_time)),
                key=lambda x: x.routing.arrival_time)

            print '   Vehicle', decider.ID, 'is selecting the next task.'
            print '      Just arrived: Task {} @ {} secs.'.format(decider.routing.destination.ID, decider.routing.arrival_time)
            print '      Vehicle heading: {} degrees'.format(decider.heading*(180/math.pi))          

            # Update deciding vehicle location (old destination is new location)
            decider.location = decider.routing.destination
            decider.time = decider.routing.arrival_time

            # Zero-out age of visited task in deciding vehicle's age tracker  
            decider.database.age_tracker[int(decider.routing.destination.ID)-1] = 0.0

            # Increment task ages by travel time (time of this arrival less time of previous arrival)          
            for task in task_vector:
                # Only tasks within their active window are incremented, all others have age '0'
                if ((decider.time >= task.t_activate) & (decider.time < task.t_terminate)):
                    task.age = task.age + (decider.time-time)
                    # A task does not begin accruing age until it's activation time
                    if (time < task.t_activate):
                        print 'FLAG'
                        task.age = task.age - (task.t_activate-time)
                else:
                    task.age = 0
            # Zero out age of task that vehicle just arrived at
            task_vector[decider.routing.destination.ID-1].age = 0            

            # Document vehicle task visits, trajectory information, and task ages
            # visit_order format is [vehicleID, task, visit_time, trajectory]
            visit_order.append([decider.ID, decider.location.ID, decider.time, decider.pathing.trajectory])
            task_age_vector = []
            for task in task_vector:              
                task_age_vector.append(task.age)
            task_ages.append(task_age_vector) 

            print '      Vehicle age tracker = {}'.format(np.around(decider.database.age_tracker, 3))            
            print '      True task ages = {}'.format(np.around(task_age_vector, 3))   
            
            #Calculate shortest travel times and associated arrival headings from current location to all possible destinations 
            #   These are the actual flight times, based on vehicle flight kinematics 
            flight_times_and_headings = decider.pathing.get_best_paths(decider, task_vector)
            print '      Shortest flight times to each task:'
            print ''
            print flight_times_and_headings[:, 0:2]
            print '' 

            #Select the next task to visit
            #   Also updates vehicle's destination
            decider.routing.get_next_task(decider, task_vector)  #returns a task object      

            #Calculate path to selected task
            #   Also updates trajectory, arrival time, & current heading   
            decider.pathing.get_path(decider, flight_times_and_headings[decider.routing.destination.ID-1, 2])  #arguments: deciding vehicle and arrival heading
           
            print '      New destination: Task {} @ {} secs.'.format(decider.routing.destination.ID, np.around(decider.routing.arrival_time, 3))
            print '      Arrival heading: {} degrees'.format(np.around(decider.heading*(180/math.pi),1))

            # Increment task ages in vehicle's own age tracker by travel time (time of the planned arrival less current time)          
            tij = decider.routing.arrival_time-decider.time
            for task in task_vector:
                # Only tasks within their active window are incremented, all others have age '0'
                if ((decider.routing.arrival_time >= task.t_activate) & (decider.routing.arrival_time < task.t_terminate)):
                    decider.database.age_tracker[task.ID-1] = decider.database.age_tracker[task.ID-1] + tij
                    #A task does not begin accruing age until it's activation time
                    if (decider.time < task.t_activate):
                        decider.database.age_tracker[task.ID-1] = decider.database.age_tracker[task.ID-1] - (task.t_activate-decider.time)
                else:
                    decider.database.age_tracker[task.ID-1] = 0

            #Communicate
            decider.comm.talk(decider, vehicle_vector)

            #Update visit number and current simulation time         
            visit_num += 1            
            time = decider.time

            print ''
            print '   Ready for next Task!\n'
            print '   *******************************************\n'

        # #Save the final task visits for each vehicle
        # for vehicle in vehicle_vector:
        #     visit_order.append([vehicle.ID, vehicle.routing.destination.ID, vehicle.routing.arrival_time, vehicle.pathing.trajectory])

        visit_order = sorted(visit_order,key=lambda x: x[2])
        visit_order = np.array(visit_order, dtype=object) 

        
        print '   *******************************************'
        print '   ******     Simulation Complete   **********'
        print '   *******************************************'

        #Display the visit history to screen
        print ''
        print '   Simulation Visit History:'        
        print(visit_order[:, 0:3])
        print ''

        #Save each trade into a pickle file...
        # This will just be so much easier than using an XML file...
        # Just pickle the visit_order, task_ages, task_vector, vehicle_vector...and everything relevant will be in those
        # DO THIS MONDAY!!!


if __name__ == '__main__':

    main()