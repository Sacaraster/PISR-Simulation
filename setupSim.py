# import sys
# import math
# import matplotlib.pyplot as plt
# import pandas as pd
import os
import shutil
import pickle
import math
import itertools
import numpy as np

def genconfig_file(sim_path, tradeID, sim_length, task_geometry, priorities, init_ages, task_activation_times, 
        task_termination_times, init_locations, init_headings, veh_speeds, veh_bank_angles, veh_activation_times, veh_termination_times, 
        routing_data, pathing_data, comm_modes, database_items):    

    #Declare empty iterable arrays
    betas = [None]
    ws_vector_array = [None]
    distance_measure = [None]
    tours_vector_array = [None]
    veh_start_index_vector_array = [None]

    #Create combination vectors
    #These vectors will be used to generate a new "trade" for every combination of parameters specified
    #in the setup script.
    priorities_vector_array = np.array(list(itertools.product(*priorities)))
    init_ages_vector_array = np.array(list(itertools.product(*init_ages)))
    task_activation_times_vector_array = np.array(list(itertools.product(*task_activation_times)))
    task_termination_times_vector_array = np.array(list(itertools.product(*task_termination_times)))
    init_locations_vector_array = np.array(list(itertools.product(*init_locations)))
    init_headings_vector_array = np.array(list(itertools.product(*init_headings)))*(math.pi/180) 
    veh_speeds_vector_array = np.array(list(itertools.product(*veh_speeds)))
    veh_bank_angles_vector_array = np.array(list(itertools.product(*veh_bank_angles)))*(math.pi/180) 
    if routing_data[0] == 'MD2WRP':
        routing_type = routing_data[0]
        betas = routing_data[1]
        ws_vector_array = np.array(list(itertools.product(*routing_data[2])))
        distance_measure = routing_data[3]        
    if routing_data[0] == 'Manual':
        routing_type = routing_data[0]
        tours_vector_array = np.array(list(itertools.product(*routing_data[1])))
        veh_start_index_vector_array = np.array(list(itertools.product(*routing_data[2])))


    for priorities_vector in priorities_vector_array:
        for init_ages_vector in init_ages_vector_array:
            for task_activation_times_vector in task_activation_times_vector_array:
                for task_termination_times_vector in task_termination_times_vector_array:
                    for init_locations_vector in init_locations_vector_array:
                        for init_headings_vector in init_headings_vector_array:
                            for veh_speeds_vector in veh_speeds_vector_array:
                                for veh_bank_angles_vector in veh_bank_angles_vector_array:    
                                    for beta in betas:
                                        for ws_vector in ws_vector_array:
                                            for tours_vector in tours_vector_array:
                                                for veh_start_index_vector in veh_start_index_vector_array:
                                                    for pathing_type in pathing_data:
                                                        for comm_mode in comm_modes:
                                                            trade_config_pickle = '{0}Trade_{1}_Config.pickle'.format(sim_path, tradeID)
                                                            trade_config = {'tradeID':tradeID,
                                                                'sim_length':sim_length,
                                                                'task_geometry':task_geometry, 'priorities_vector':priorities_vector, 'init_ages_vector':init_ages_vector,
                                                                'task_activation_times_vector':task_activation_times_vector, 'task_termination_times_vector':task_termination_times_vector,
                                                                'init_locations_vector':init_locations_vector, 'init_headings_vector':init_headings_vector, 'veh_speeds_vector':veh_speeds_vector,
                                                                'veh_bank_angles_vector':veh_bank_angles_vector, 'veh_activation_times':veh_activation_times, 'veh_termination_times':veh_termination_times,
                                                                'routing_type':routing_type, 'beta':beta, 'ws_vector':ws_vector, 'distance_measure':distance_measure,
                                                                'tours_vector':tours_vector, 'veh_start_index_vector':veh_start_index_vector,
                                                                'pathing_type':pathing_type, 'comm_mode':comm_mode, 'database_items':database_items}
                                                            pickle.dump(trade_config, open(trade_config_pickle, "wb"))
                                                            tradeID += 1

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
    # sim_length = [100, float('inf')]
    sim_length = [float('inf'), 10000]

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
    # Note: length of init_locations array determines # of vehicles, all other arrays
    #       may be longer...only the required elements will be used

    #Starting task locations (length of this array determines # of vehicles)
    init_locations = np.array([[1], [1], [1]])

    #Initial headings of each vehicle (in degrees)
    init_headings = np.array([[0], [0], [0], [0], [0]])

    #Speed of each vehicle (in meters/sec)
    veh_speeds = np.array([[22], [22], [22], [22], [22]])

    #Max bank angle of each vehicle (in degrees). Assume every turn at max angle.
    #90 deg bank angle implies Euclidean travel.
    # veh_bank_angles = np.array([[10], [10], [10], [10], [10]])
    veh_bank_angles = np.array([[30], [30], [30], [30], [30]])
    # veh_bank_angles = np.array([[90], [90], [90], [90], [90]])

    #Assign activation times (in sec) for each vehicle (when the vehicle becomes eligible
    #to accomplish tasks)
    veh_activation_times = np.array([0, 0, 0, 0, 0])

    #Assign activation times (in sec) for each vehicle (when the vehicle becomes ineligible
    #to accomplish tasks)
    veh_termination_times = np.array([float('inf'), float('inf'), float('inf'), float('inf'),
        float('inf')])

    ####################################################################################
    ####################################################################################  


    ####################################################################################
    ### ROUTING PARAMETERS #############################################################
    #################################################################################### 
    # Routing refers to how a vehicle selects it's next task.

    #Specify a routing method - Comment out unused methods
    #(This version of the setup script assumes all vehicles have the same routing method
    #and parameters)
    
    #MD2WRP format: ['MD2WRP', [Beta_1,Beta_2,Beta_3], [[w_1], [w_2], [w_3], ...], ['distance_measure']
    # distance_measure == (1 for Euclidean), (2 for Dubins), (3 for Tripath)
    # routing_data = ['MD2WRP', [5], [[1], [1], [1], [1], [1], [1], [1], [1], [1], [1]], ['Euclidean']]
    routing_data = ['MD2WRP', [5], [[1], [1], [1], [1], [1], [1], [1], [1], [1], [1]], ['Tripath', task_geometry, 1]]
    # routing_data = ['MD2WRP', np.arange(7, 8.1, .1), [[1], [1], [1], [1], [1], [1], [1], [1], [1], [1]], 2]
    
    #Manual format: ['Manual', [[stop_1],[stop_2], [stop_3], ...], [[veh_1_index],[veh_2_index]]]
    # routing_data = ['Manual', [[1], [2], [6], [4], [5], [8], [10], [9], [7], [3]], [[0], [4]]]
    
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
    # pathing_data = [['Euclidean']]
    # pathing_data = [['Dubins']]
    pathing_data = [['Tripath', task_geometry, 1]]

    ####################################################################################
    #################################################################################### 


    ####################################################################################
    ### COMMUNICATION PARAMETERS #######################################################
    ####################################################################################
    # Determines how vehicles talk to one another.
    # 'Completion' = vehicles tell other vehicles when a task has been completed.
    # 'Destination' = vehicles share destination and completion information.

    #Specify a communication type.
    # comm_modes = ['None']
    # comm_modes = ['Completion']
    comm_modes = ['Destination']  

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

    #if the directory already exists, delete the old config files
    if os.path.exists(sim_path):
        for file in os.listdir(sim_path):
            if file.endswith("_Config.pickle"): 
                os.remove(sim_path+file)

    #if the directory doesn't exist, create it
    if not os.path.exists(sim_path):
        os.makedirs(sim_path)

    tradeID = 1000  #initial trade ID number

    #Create configuration files (pickles) for each trade
    genconfig_file(sim_path, tradeID, sim_length, task_geometry, priorities, init_ages, task_activation_times, 
        task_termination_times, init_locations, init_headings, veh_speeds, veh_bank_angles, veh_activation_times, veh_termination_times, 
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