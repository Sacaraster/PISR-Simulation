from __future__ import division

import shutil
import os
import sys
import math
import pickle
import dubins

import numpy as np

from generateMapCoordinates import generateMapCoordinates
from Classes.TaskClass import Task
from Classes.VehicleClass import Vehicle


def loadTaskConfig(trade_config):     

    #Pull task parameters from trade_config
    task_geometry = trade_config['task_geometry']
    x_coords, y_coords = generateMapCoordinates(task_geometry)
    priorities_vector = trade_config['priorities_vector']
    init_ages_vector = trade_config['init_ages_vector']
    task_activation_times_vector = trade_config['task_activation_times_vector']
    task_termination_times_vector = trade_config['task_termination_times_vector']
    
    print ''
    print '      Instantiating task objects ({} tasks, "{}" map)...'.format(len(x_coords), task_geometry)

    #Create the array of Task objects
    task_vector = []
    #For each task...
    for index, task in enumerate(x_coords):
        taskID = index+1
        x_coord = x_coords[index]
        y_coord = y_coords[index]
        priority = priorities_vector[index]
        init_age = init_ages_vector[index]
        t_activate = task_activation_times_vector[index]
        t_terminate = task_termination_times_vector[index]
        print '         Task {} @ ({},{}), Priority={}' \
              .format(taskID, x_coord, y_coord, priority)
        print '            Initial age={}, Activation time: {}, Termination time: {}' \
              .format(init_age,t_activate,t_terminate)
        #Insantiate Task object
        taskObj = Task(taskID, x_coord, y_coord, priority,
                       init_age, t_activate, t_terminate)
        task_vector.append(taskObj)    #Add object to vector of task objects    

    return task_vector, task_geometry

def loadVehicleConfig(trade_config, task_vector): 

    #Pull vehicle parameters from trade_config
    init_locations_vector = trade_config['init_locations_vector']
    init_headings_vector = trade_config['init_headings_vector']
    veh_speeds_vector = trade_config['veh_speeds_vector']
    veh_bank_angles_vector = trade_config['veh_bank_angles_vector']
    veh_activation_times = trade_config['veh_activation_times']
    veh_termination_times = trade_config['veh_termination_times']

    print ''
    print '      Instantiating vehicle objects ({} vehicles)...' \
           .format(len(init_locations_vector))

    #Create the array of Vehicle objects
    vehicle_vector = []
    #For each vehicle...
    for index, vehicle in enumerate(init_locations_vector):
        vehicleID = int((index+1)*100)
        print '         Vehicle', vehicleID
        init_location = init_locations_vector[index]
        #re-assign init_location to correspond to a Task object
        init_location = task_vector[init_location-1]
        print '            Initial Task:', init_location.ID
        init_heading = init_headings_vector[index] #(display in deg)                         
        print '            Initial Heading:',init_heading, 'degrees.'
        init_heading = init_headings_vector[index]*(math.pi/180) #(store in radians)
        veh_speed = veh_speeds_vector[index]
        print '            Vehicle Speed:', veh_speed, 'meters/sec.'
        veh_bank_angle = veh_bank_angles_vector[index] #(display in deg)
        print '            Vehicle Bank Angle: ', veh_bank_angle, 'degrees.'
        veh_bank_angle = veh_bank_angles_vector[index]*(math.pi/180) #(store in radians)
        turn_radius = veh_speed**2/(9.807*math.tan(veh_bank_angle))
        print '            Vehicle Turn Radius: ', np.around(turn_radius,1), 'meters.'
        veh_t_activate = veh_activation_times[index]
        print '            Vehicle Activation Time: ', veh_t_activate, 'secs.'
        veh_t_terminate = veh_termination_times[index]
        print '            Vehicle Termination Time: ', veh_t_terminate, 'secs.'

        #Instantiate the vehicle object
        vehicleObj = Vehicle(index, vehicleID, init_location, init_heading, veh_speed, turn_radius, veh_t_activate, veh_t_terminate) 

        vehicle_vector.append(vehicleObj)

    #Load the modules for each vehicle
    vehicle_vector = loadRoutingConfig(trade_config, vehicle_vector, task_vector)
    vehicle_vector = loadPathingConfig(trade_config, vehicle_vector, task_vector)
    vehicle_vector = loadCommConfig(trade_config, vehicle_vector)
    vehicle_vector = loadDatabaseConfig(trade_config, vehicle_vector, task_vector)
  
    return vehicle_vector

def loadRoutingConfig(trade_config, vehicle_vector, task_vector):
    
    print ''
    print '      Adding Routing modules...'

    routing_data = trade_config['routing_data']
    
    for index, vehicle in enumerate(vehicle_vector):
        vehicle.add_routing(routing_data[index], task_vector)
        print '         Vehicle {} Routing Data:'.format(vehicle.ID)
        vehicle.routing.print_routing_data()

    return vehicle_vector

def loadPathingConfig(trade_config, vehicle_vector, task_vector):

    print ''
    print '      Adding Pathing modules...'

    pathing_data = trade_config['pathing_data']

    for index, vehicle in enumerate(vehicle_vector):
        vehicle.add_pathing(pathing_data[index])
        print '         Vehicle {} Pathing Data:'.format(vehicle.ID)        
        try:
            vehicle.pathing.calc_nfz_impact_rating(pathing_data[index], task_vector)
        except:
            pass
        vehicle.pathing.print_pathing_data()

    return vehicle_vector

def loadCommConfig(trade_config, vehicle_vector):

    print ''
    print '      Adding Communication modules...'

    comm_modes = trade_config['comm_modes']

    # comm_data = [comm_mode]

    for index, vehicle in enumerate(vehicle_vector):
        vehicle.add_comm(comm_modes[index])
        print '         Vehicle {} Comm Data: {}'.format(vehicle.ID, vehicle.comm.type)

    return vehicle_vector

def loadDatabaseConfig(trade_config, vehicle_vector, task_vector):
    
    print ''
    print '      Adding Database modules...'

    database_items = trade_config['database_items']

    for index, vehicle in enumerate(vehicle_vector):
        print '         Vehicle {} Database Items:'.format(vehicle.ID)
        vehicle.add_database(database_items[index], vehicle_vector, task_vector)
        # print '         Vehicle {} Database Items: {}, {}'.format(vehicle.ID, vehicle.database.age_tracker, vehicle.database.vehicle_tracker)
        vehicle.database.print_database_items()

    return vehicle_vector


def main():

    ###################################################################
    ###       LOAD CONFIGURATION FILES AND INSANTIATE OBJECTS   #######
    ###################################################################   
    #Argument supplying location of the simulation configuration files
    sim_path = sys.argv[1]

    #Create directory where simulation data will be saved
    sim_data_path = './Data/'    
    print '\nSimulation data will be saved to {}\n'.format(sim_data_path)

    #Open every pickle file in the sim directory
    for file in os.listdir(sim_path):
        if file.endswith("_Config.pickle"):    
            trade_config_pickle = '{0}{1}'.format(sim_path, file)
            print 'Opening \"', trade_config_pickle, '\"\n'
            trade_config = pickle.load(open(trade_config_pickle, "rb"))
            tradeID = trade_config['tradeID']
            print '***************************************'
            print '          Loading TradeID={}        '.format(tradeID)
            print '***************************************'
            sim_length = trade_config['sim_length']
            sim_length_visits = sim_length[0]
            sim_length_time = sim_length[1]

            print '   Simulation will terminate at {} task visits or {} seconds.' \
                  .format(sim_length_visits, sim_length_time)

            #Load the task parameters; returns a vector of Task objects
            task_vector, task_geometry = loadTaskConfig(trade_config) 

            #Load the vehicle parameters and all modules;
            # returns a vector of Vehicle objects
            vehicle_vector = loadVehicleConfig(trade_config, task_vector)     
    
    ######################################################################
    ######################################################################    
    
            #Don't print in scientific notation
            np.set_printoptions(suppress=True)   
            #Don't truncate large arrays when printing
            np.set_printoptions(threshold='nan')        

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

                # Increment task object ages by travel time (time of this arrival less time of previous arrival)          
                for task in task_vector:
                    # Only tasks within their active window are incremented, all others have age '0'
                    if ((decider.time >= task.t_activate) & (decider.time < task.t_terminate)):
                        task.age = task.age + (decider.time-time)
                        # A task does not begin accruing age until it's activation time
                        if (time < task.t_activate):
                            task.age = task.age - (task.t_activate-time)
                    else:
                        task.age = 0            

                # First, save task ages without setting visited task age to 0
                task_age_vector = []
                task_age_vector.append(decider.time)  #first entry in age vector is timestamp
                for task in task_vector:              
                    task_age_vector.append(task.age)
                task_ages.append(task_age_vector)

                # Zero out age of task that vehicle just arrived at
                task_vector[decider.routing.destination.ID-1].age = 0

                # Now, save task ages again (@ +.01s) with age of visited task at 0
                #   This is needed to perform the latency calculations in the analysis script
                task_age_vector = []
                task_age_vector.append(decider.time+.01)  #first entry in age vector is timestamp
                for task in task_vector:              
                    task_age_vector.append(task.age)
                task_ages.append(task_age_vector)            

                # Document vehicle task visits, trajectory information, and task ages
                # visit_order format is [vehicleID, task, visit_time, trajectory]
                visit_order.append([decider.ID, decider.location.ID, decider.time, decider.pathing.trajectory])             

                print '      Vehicle {} age tracker = \n      {}'.format(decider.ID, np.around(decider.database.age_tracker, 3))            
                print '      True task ages = \n      {}'.format(np.around(task_age_vector[1:], 3))   
                print '      Vehicle {} vehicle tracker = \n{}'.format(decider.ID, np.around(decider.database.vehicle_tracker, 3))

                #Select the next task to visit
                #   (Updates vehicle's destination)
                decider.routing.get_next_task(decider, task_vector)  

                #Calculate path to selected task
                #   (Updates trajectory, arrival_time, & current_heading)   
                decider.pathing.get_path(decider)

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
            print '   Pickling results...'
            trade_results_pickle = '{0}Trade_{1}_Results.pickle'.format(sim_data_path, tradeID)
            trade_results = [visit_order, task_ages, task_vector, vehicle_vector, tradeID, task_geometry]
            pickle.dump(trade_results, open(trade_results_pickle, "wb"))
            print '   Results pickled.'


if __name__ == '__main__':

    main()