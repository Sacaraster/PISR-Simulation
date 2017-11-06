# import sys
# import math
# import matplotlib.pyplot as plt
# import pandas as pd
import os
import shutil
import itertools
import numpy as np

from generateMapCoordinates import generateMapCoordinates

def genconfig_file(sim_path, tradeID, sim_length, task_geometry, priorities, init_ages, task_activation_times, 
        task_termination_times, init_locations, init_headings, veh_speed, veh_bank_angle, veh_activation_times, veh_termination_times, 
        routing_data, pathing_data, comm_modes, database_items):    

    
    #Declare empty iterable arrays
    betas = [None]
    ws_vector_array = [None]
    tours_vector_array = [None]

    #Create combination vectors
    #These vectors will be used to generate a new "trade" for every combination of parameters specified
    #in the setup script.
    priorities_vector_array = np.array(list(itertools.product(*priorities)))
    init_ages_vector_array = np.array(list(itertools.product(*init_ages)))
    task_activation_times_vector_array = np.array(list(itertools.product(*task_activation_times)))
    task_termination_times_vector_array = np.array(list(itertools.product(*task_termination_times)))
    init_locations_vector_array = np.array(list(itertools.product(*init_locations)))
    init_headings_vector_array = np.array(list(itertools.product(*init_headings)))
    if routing_data[0] == 'MD2WRP':
        betas = routing_data[1]
        ws_vector_array = np.array(list(itertools.product(*routing_data[2])))
    if routing_data[0] == 'Manual':
        tours_vector_array = np.array(list(itertools.product(*routing_data[1])))

    file_name = sim_path + 'sim_configuration.xml'

    config_file = open(file_name,'w')
    config_file.write('<Sim_Config>\n')

    for priorities_vector in priorities_vector_array:
        for init_ages_vector in init_ages_vector_array:
            for task_activation_times_vector in task_activation_times_vector_array:
                for task_termination_times_vector in task_termination_times_vector_array:
                    for init_locations_vector in init_locations_vector_array:
                        for init_headings_vector in init_headings_vector_array:
                            for beta in betas:
                                for ws_vector in ws_vector_array:
                                    for tours_vector in tours_vector_array:
                                        for pathing_type in pathing_data:
                                            for comm_mode in comm_modes:
                                                config_file.write('\t<Trade>\n')                                             
                                                config_file.write('\t\t<tradeID>{}</tradeID>\n'.format(tradeID))
                                                config_file.write('\t\t<sim_length_visits>{}</sim_length_visits>\n'.format(sim_length[0]))
                                                config_file.write('\t\t<sim_length_time>{}</sim_length_time>\n'.format(sim_length[1]))
                                                genTaskConfig(config_file, task_geometry, priorities_vector, init_ages_vector, 
                                                    task_activation_times_vector, task_termination_times_vector)
                                                genVehicleConfig(config_file, init_locations_vector, init_headings_vector, veh_speed, veh_bank_angle, 
                                                    veh_activation_times, veh_termination_times)
                                                genRoutingConfig(config_file, routing_data[0], beta, ws_vector, tours_vector)
                                                genPathingConfig(config_file, pathing_type)
                                                genCommConfig(config_file, comm_mode)
                                                genDatabaseConfig(config_file, database_items)
                                                config_file.write('\t</Trade>\n')
                                                tradeID += 1



    config_file.write('</Sim_Config>')
    config_file.close()     
                

def genTaskConfig(config_file, task_geometry, priorities_vector, init_ages_vector, 
    task_activation_times_vector, task_termination_times_vector):   

    config_file.write('\t\t<Task_Info>\n')
    config_file.write('\t\t\t<Task_geometry>{}</Task_geometry>\n'.format(task_geometry))

    x_task_coords, y_task_coords = generateMapCoordinates(task_geometry)

    for index, task in enumerate(range(1, len(x_task_coords)+1)):
        taskID = int(task)
        config_file.write('\t\t\t<Task>\n')
        config_file.write('\t\t\t\t<ID>{}</ID>\n'.format(taskID))
        config_file.write('\t\t\t\t<xCoord>{}</xCoord>\n'.format(x_task_coords[index]))
        config_file.write('\t\t\t\t<yCoord>{}</yCoord>\n'.format(y_task_coords[index]))
        config_file.write('\t\t\t\t<priority>{}</priority>\n'.format(priorities_vector[index]))
        config_file.write('\t\t\t\t<init_age>{}</init_age>\n'.format(init_ages_vector[index]))
        config_file.write('\t\t\t\t<task_t_activate>{}</task_t_activate>\n'.format(task_activation_times_vector[index]))
        config_file.write('\t\t\t\t<task_t_terminate>{}</task_t_terminate>\n'.format(task_termination_times_vector[index]))
        config_file.write('\t\t\t</Task>\n')
    config_file.write('\t\t</Task_Info>\n') 

def genVehicleConfig(config_file, init_locations_vector, init_headings_vector, veh_speed, veh_bank_angle, 
    veh_activation_times, veh_termination_times):    

    config_file.write('\t\t<Vehicle_Info>\n')

    for index, vehicle in enumerate(xrange(1, len(init_locations_vector)+1)):
        vehicleID = int(vehicle*100)
        config_file.write('\t\t\t<Vehicle>\n')
        config_file.write('\t\t\t\t<ID>{}</ID>\n'.format(vehicleID))
        config_file.write('\t\t\t\t<init_location>{}</init_location>\n'.format(init_locations_vector[index]))
        config_file.write('\t\t\t\t<init_heading>{}</init_heading>\n'.format(init_headings_vector[index]))
        config_file.write('\t\t\t\t<veh_speed>{}</veh_speed>\n'.format(veh_speed))
        config_file.write('\t\t\t\t<veh_bank_angle>{}</veh_bank_angle>\n'.format(veh_bank_angle))
        config_file.write('\t\t\t\t<veh_t_activate>{}</veh_t_activate>\n'.format(veh_activation_times[index]))
        config_file.write('\t\t\t\t<veh_t_terminate>{}</veh_t_terminate>\n'.format(veh_termination_times[index]))         
        config_file.write('\t\t\t</Vehicle>\n') 
    config_file.write('\t\t</Vehicle_Info>\n')

def genRoutingConfig(config_file, routing_type, beta, ws_vector, tours_vector):

    config_file.write('\t\t<Routing_Info>\n')
    config_file.write('\t\t\t<routing_type>{}</routing_type>\n'.format(routing_type))
    if routing_type == 'MD2WRP':
        config_file.write('\t\t\t<beta>{}</beta>\n'.format(beta))
        config_file.write('\t\t\t<w>{}</w>\n'.format(np.array2string(ws_vector, separator=', ')))
    if routing_type == 'Manual':
        config_file.write('\t\t\t<tour>{}</tour>\n'.format(np.array2string(tours_vector, separator=', ')))    
    config_file.write('\t\t</Routing_Info>\n')

def genPathingConfig(config_file, pathing_type):

    config_file.write('\t\t<Pathing_Info>\n')
    config_file.write('\t\t\t<pathing_type>{}</pathing_type>\n'.format(pathing_type))        
    config_file.write('\t\t</Pathing_Info>\n')

def genCommConfig(config_file, comm_mode):

    config_file.write('\t\t<Comm_Info>\n')
    config_file.write('\t\t\t<comm_mode>{}</comm_mode>\n'.format(comm_mode))        
    config_file.write('\t\t</Comm_Info>\n')

def genDatabaseConfig(config_file, database_items):
    config_file.write('\t\t<Database_Info>\n')
    for database_item in database_items:
        config_file.write('\t\t\t<{0}>{0}</{0}>\n'.format(database_item))        
    config_file.write('\t\t</Database_Info>\n')


def main():

    ####################################################################################
    ############################### Simulation Setup ###################################
    ####################################################################################
    # This setup script serves as a guide for ensuring all necessary parameters are set.        

    
    ####################################################################################
    ### META PARAMETERS ################################################################
    ####################################################################################
    # These parameters are related to the simulation as a whole, and not any particular
    # object within the simulation itself.

    #Specify a directory for the simulation config files and results (in .xml format)
    sim_path = './Sims/development/'

    #How long to run the sim - [# tasks, time in sec]
    #Whichever condition occurs first will terminate the simulation
    sim_length = [250, float('inf')]
    # sim_length = [float('inf'), 20000]

    ####################################################################################
    ####################################################################################    

   
    ####################################################################################
    ### TASK PARAMETERS ################################################################
    #################################################################################### 
    # These parameters are used to define the PISR Tasks. Multiple priorities, initial
    # ages, activation times, or termination times assigned to tasks will automatically
    # create multiple trades. (Careful, many combinations can add up!)

    #Load a task geometry defined in "generateMapCoordinates.py"
    task_geometry = 'random'

    #Assign a priority to each task.
    priorities = np.array([[1], [1], [1], [1], [1], [1], [1], [1], [1], [1]])
    # priorities = np.array([[1], [1], [1], [1], [1], [1], [1], [1], [1], [1], [1], [1], [1], [1], [1], [1]])

    #Assign an initial age to each task.
    init_ages = np.array([[0], [0], [0], [0], [0], [0], [0], [0], [0], [0]])

    #Assign activation times (in sec) for each task (when the task becomes eligible
    #for selection)
    task_activation_times = np.array([[0], [0], [0], [0], [0], [0], [0], [0], [0], [0]])

    #Assign termination times (in sec) for each task (When the task becomes ineligible
    #for selection)
    task_termination_times = np.array([[float('inf')], [float('inf')], [float('inf')], [float('inf')], [float('inf')], 
        [float('inf')], [float('inf')], [float('inf')], [float('inf')], [float('inf')]])
    
    ####################################################################################
    ####################################################################################  


    ####################################################################################
    ### VEHICLE PARAMETERS #############################################################
    #################################################################################### 
    # These define aspects of the physical vehicle (e.g. speed). Parameters related to
    # other functions (e.g. pathing) are defined separately by their respective modules

    #Starting task locations (length of this array determines # of vehicles)
    init_locations = np.array([[1], [1], [1]])

    #Initial headings of each vehicle (in degrees)
    init_headings = np.array([[0], [0], [0]])

    #Speed of each vehicle (in meters/sec)
    #(This version of the setup script assumes all vehicles have the same)
    veh_speed = 22

    #Max bank angle of each vehicle (in degrees). Assume every turn at max angle.
    #90 deg bank angle implies Euclidean travel.
    #(This version of the setup script assumes all vehicles use the same bank angle)
    veh_bank_angle = 30

    #Assign activation times (in sec) for each vehicle (when the vehicle becomes eligible
    #to accomplish tasks)
    veh_activation_times = np.array([0, 0, 0])

    #Assign activation times (in sec) for each vehicle (when the vehicle becomes ineligible
    #to accomplish tasks)
    veh_termination_times = np.array([float('inf'), float('inf'), float('inf')])

    ####################################################################################
    ####################################################################################  


    ####################################################################################
    ### ROUTING PARAMETERS #############################################################
    #################################################################################### 
    # Routing refers to how a vehicle selects it's next task.

    #Specify a routing method - Comment out unused methods
    #(This version of the setup script assumes all vehicles have the same)
    
    #MD2WRP format: ['MD2WRP', [Beta_1,Beta_2,Beta_3], [[w_1], [w_2], [w_3]], 'distance_measure']
    routing_data = ['MD2WRP', [0, 4], [[1], [1], [1], [1], [1], [1], [1], [1], [1], [1]], 'Euclidean']
    
    #Manual format: ['Manual', [[stop_1],[stop_2], [stop_3]] ]
    # routing_data = ['Manual', [[1], [2], [3], [4], [5], [6], [7], [8], [9], [10]]]
    
    ####################################################################################
    ####################################################################################     


    ####################################################################################
    ### PATHING PARAMETERS #############################################################
    ####################################################################################
    # Pathing refers to how a vehicle travels between tasks. Note: while "Euclidean"
    # pathing yields the same result as "Dubins" pathing with 90 deg bank angle, the
    # simulation speed is faster using Euclidean. Similarly, only use Tripath if path
    # constraints must be enforced.
    # Dubins = path determined by veh speed and bank angle (as specified above)
    # Tripath = use Tripath software for path constraint avoidance (assumes Euclidean)
    
    #Specify a pathing method
    pathing_data = ['Dubins']
    # pathing_data = ['Tripath']

    ####################################################################################
    #################################################################################### 


    ####################################################################################
    ### COMMUNICATION PARAMETERS #######################################################
    ####################################################################################
    # Determines how vehicles talk to one another.
    # 'Completion' = vehicles tell other vehicles when a task has been completed.
    # 'Destination' = vehicles share destination and completion information.

    #Specify a communication type.
    # comm_modes = ['none']
    # comm_modes = ['Completion']
    comm_modes = ['Destination']
    # comm_modes = ['none', 'Destination']    

    ####################################################################################
    ####################################################################################

    ####################################################################################
    ### DATABASE PARAMETERS ############################################################
    ####################################################################################
    # Determines the type of information that vehicles track during the sim.
    # 'Age_Tracker' = vehicles maintain formation about task ages, based on which tasks
    #                 they have completed and on updates from other vehicles
    # 'Vehicle_Tracker' = vehicles keep track of where other vehicles are located and
    #                     where they are headed

    #Specify the database items to implement
    database_items = ['Age_Tracker', 'Vehicle_Tracker'] 

    ####################################################################################
    #################################################################################### 
    

    ####################################################################################
    ###################################### End Setup ###################################
    ####################################################################################

    if not os.path.exists(sim_path):
            os.makedirs(sim_path)

    tradeID = 1000  #initial trade ID number

    genconfig_file(sim_path, tradeID, sim_length, task_geometry, priorities, init_ages, task_activation_times, 
        task_termination_times, init_locations, init_headings, veh_speed, veh_bank_angle, veh_activation_times, veh_termination_times, 
        routing_data, pathing_data, comm_modes, database_items)

    #Create a bash script to run the simulation(s)
    run_sim_script = '{0}{1}'.format(sim_path,'runTrades.sh')
    file_run = open(run_sim_script,'w')
    file_run.write('#! /bin/bash\n')
    file_run.write('rm -R ./Data\n')
    file_run.write('mkdir {0}\n'.format('./Data'))
    file_run.write('python /home/chris/Research/PISR_Sim_NGpp/runSim.py ./')
    os.chmod(run_sim_script, 0777)

    #copy the analysis script that works with these results into the directory
    source_file = '/home/chris/Research/PISR_Sim_NGpp/Resources/analysisScript.py'
    shutil.copy(source_file, sim_path)  


if __name__ == '__main__':

    main()