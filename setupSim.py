
import os
import shutil
import pickle
import itertools
import numpy as np

def main():

    ###################################################################
    ##################### USER SIMULATION SETUP #######################
    ###################################################################
    # This setup script serves as a guide for ensuring all necessary
    # parameters are set.    
    
    ###################################################################
    ### META PARAMETERS ###############################################
    ###################################################################
    # These parameters are related to the simulation as a whole, and not
    # any particular object within the simulation.

    #Specify a directory to store the simulation config files. Results
    # will be placed in this directory in the "Data" folder.
    sim_path = './Sims/Test/'
    
    #### DO NOT EDIT #######
    #if the sim directory already exists, delete the old config files
    if os.path.exists(sim_path):
        for file in os.listdir(sim_path):
            if file.endswith("_Config.pickle"): 
                os.remove(sim_path+file)
    #if the directory doesn't exist, create it
    if not os.path.exists(sim_path):
        os.makedirs(sim_path)
    ##### RESUME EDITING ### 

    #How long to run the sim - format: [# tasks, time in sec]
    #Whichever condition occurs first will terminate the simulation
    # sim_length = [100, float('inf')]
    sim_length = [float('inf'), 20000]

    ###################################################################
    ###################################################################  

    ###################################################################
    ### TRADE VARIABLES ###############################################
    ###################################################################
    #Use this section if you wish to iterate over multiple combinations
    #of parameters. Declare variables for the different parameters you
    #wish to test, and then create a FOR loop to iterate over them. A
    #config file will be generated for every trade combination.
    #PUMPS will run once for every config file in the
    #directory.

    #Initial trade ID number
    tradeID = 1000

    #Trade variables
    beta_trade = [5.0]

    for beta in beta_trade:    

        ###################################################################
        ### TASK PARAMETERS ###############################################
        ################################################################### 
        # These parameters are used to define the PISR Tasks. Multiple
        # priorities, initial ages, activation times, or termination times
        # assigned to tasks will automatically create multiple trades.
        # (Careful, many combinations can add up!)

        #Load a task geometry defined in "generateMapCoordinates.py"
        task_geometry = 'clusters'

        #Assign a priority to each task. Entries will only be used so long
        # as there are enough tasks. Extra entries are ignored.
        priorities = np.array([1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1])

        #Assign an initial age to each task. Extra entries are ignored.
        init_ages = np.array([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])

        #Assign activation times (in sec) for each task.
        #Extra entries are ignored.
        task_activation_times = np.array([0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                        0, 0, 0, 0, 0, 0])

        #Assign termination times (in sec) for each task.
        #Extra entries are ignored.
        task_termination_times = np.array([float('inf'), float('inf'),
                                          float('inf'), float('inf'),
                                          float('inf'), float('inf'),
                                          float('inf'), float('inf'),
                                          float('inf'), float('inf'),
                                          float('inf'), float('inf'),
                                          float('inf'), float('inf'),
                                          float('inf'), float('inf')])

        
        ###################################################################
        ###################################################################


        ###################################################################
        ### VEHICLE PARAMETERS ############################################
        ###################################################################
        #This script allows each vehicle to have separate flight
        #characteristics (e.g. bank angle or speed) and utilize different
        #types of routing, pathing, communication, etc.

        ##################################
        ### General Vehicle Paramters ####
        ##################################
        #Define general aspects of the vehicle.
        #Parameters related to other functions (e.g. routing) are defined
        #separately by their respective modules.
        #Note: length of init_locations array determines # of vehicles.
        #All other arrays may be longer...extra entries are ignored.

        #Starting task locations (array length determines # of vehicles)
        init_locations = np.array([1, 4, 7])                                 

        #Initial headings of each vehicle (in degrees)
        #Due east is 0 degrees
        init_headings = np.array([0, 45, 180, 0, 0])

        #Speed of each vehicle (in meters/sec)
        veh_speeds = np.array([25, 25, 25, 20, 20])

        #Max bank angle of each vehicle (in degrees).
        #Every turn is a max angle turn.
        #90 deg bank angle implies Euclidean travel.
        veh_bank_angles = np.array([30, 30, 30, 90, 90])

        #Assign activation times (in sec) for each vehicle.
        veh_activation_times = np.array([0, 0, 0, 0, 0])

        #Assign termination times (in sec) for each vehicle.
        veh_termination_times = np.array([float('inf'), float('inf'),
                                        float('inf'), float('inf'),float('inf')])

        ##################################
        ### Vehicle Routing Parameters ###
        ##################################
        #Routing refers to how a vehicle selects tasks.
        #There must be a routing entry for every vehicle.    
        #Two types of routing are currently supported: MD2WRP and Manual
        
        #MD2WRP - uses the MD2WRP utility function to select tasks.
        #MD2WRP format: ['MD2WRP', Beta, [w_1, w_2, ...], 'distance_measure']
        
        #Manual - visits tasks in a pre-determined order   
        #Manual format: ['Manual', [veh tour], [index of veh start w/in tour]]
        #Note: index of veh start refers to the index within the tour array,
        #      not the task number. Also, task number at starting index must
        #      match task number from init_locations array specified in general
        #      parameters.

        # routing_data = [['MD2WRP', 6.0,
        #                 [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        #                 'Dubins'],
        #                 ['MD2WRP', 1.0,
        #                 [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        #                 'Dubins'],
        #                 ['Manual', [1,4,10,9,7,5,6,8,3,2], 0]
        #                 ]

        routing_data = [['Manual', [1,2,6,4,5,8,10,9,7,3], 0],
                        ['MD2WRP', 5.0,
                        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
                        'Dubins'],
                        ['MD2WRP', 4.0,
                        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
                        'Dubins']
                        ]     
        
        #clusters tsp tour -> [1,2,6,4,5,8,10,9,7,3]
        #random tsp tour   -> [1,4,10,9,7,5,6,8,3,2]
        #circle tsp tour   -> [1,2,3,4,5,6,7,8,9,10]
        #grid tsp tour     -> [1,2,3,4,8,12,16,15,11,7,6,10,14,13,9,5]   


        ##################################
        ### Vehicle Pathing Parameters ###
        ##################################
        #Pathing refers to how a vehicle travels between tasks.
        #There must be a pathing entry for every vehicle.    
        #Three types of pathing are currently supported:
        #   'Euclidean' = path length from Euclidean distance matrix of tasks
        #   'Dubins' = path length subject to veh speed and bank angle
        #   'Tripath' = path length calculated w/Tripath software for obstacle
        #             avoidance (assumes Euclidean travel)

        # Note: while "Euclidean" pathing yields the same result as "Dubins"
        # with 90 deg bank angle, the simulation speed is faster w/Euclidean.
        # Similarly, only use Tripath if path constraints must be enforced
        # (as Tripath slows down the sim substantially).
        
        pathing_data = ['Dubins', 'Dubins', 'Dubins']


        ########################################
        ### Vehicle Communication Parameters ###
        ########################################
        #Determines how a vehicle talks to other vehicles.
        #There must be a communication entry for every vehicle.
        #Three types of communication are currently supported:
        #   'None' = does not communicate
        #   'Completion' = tells other vehicles time of task completions
        #   'Destination' = tells other vehicles time of task completions,
        #                   and arrival time to selected task

        comm_modes = ['Destination', 'Destination', 'Destination']  


        ###################################
        ### Vehicle Database Parameters ###
        ###################################
        #Determines the type of information a vehicle tracks during the sim.
        #There must be a database entry for every vehicle.
        #There are currently two standard, mandatory database items:
        # 'Age_Tracker' = stores task ages, based on which tasks the vehicle
        #                 has completed and comms from other vehicles
        # 'Vehicle_Tracker' = tracks where other vehicles are headed and
        #                     their arrival time

        
        database_items = [['Age_Tracker', 'Vehicle_Tracker'],
                          ['Age_Tracker', 'Vehicle_Tracker'],
                          ['Age_Tracker', 'Vehicle_Tracker']]
        

        ###################################################################
        ###################### END USER SETUP #############################
        ###################################################################


        #Create configuration file (pickle format)
        trade_config_pickle = '{0}Trade_{1}_Config.pickle'.format(sim_path,
                                                                  tradeID)
        trade_config = {'tradeID':tradeID, 'sim_length':sim_length,
                        'task_geometry':task_geometry,
                        'priorities_vector':priorities,
                        'init_ages_vector':init_ages,
                        'task_activation_times_vector':task_activation_times,
                        'task_termination_times_vector':task_termination_times,
                        'init_locations_vector':init_locations,
                        'init_headings_vector':init_headings,
                        'veh_speeds_vector':veh_speeds,
                        'veh_bank_angles_vector':veh_bank_angles,
                        'veh_activation_times':veh_activation_times,
                        'veh_termination_times':veh_termination_times,
                        'routing_data':routing_data,                    
                        'pathing_data':pathing_data, 'comm_modes':comm_modes,
                        'database_items':database_items}
        pickle.dump(trade_config, open(trade_config_pickle, "wb"))

        tradeID+=1    

    #Create a Bash script to run the simulation(s)
    run_sim_script = '{0}{1}'.format(sim_path,'runTrades.sh')
    file_run = open(run_sim_script,'w')
    file_run.write('#! /bin/bash\n')
    file_run.write('rm -R ./Data\n')
    file_run.write('mkdir {0}\n'.format('./Data'))
    file_run.write('python /home/chris/Research/PUMPS/runSim.py ./')
    os.chmod(run_sim_script, 0777)

    #Copy your analysis script into the directory
    # Recommended for easy version control. It allows you to store
    # your results along with the analyis script that works with those
    # reuslts. If you later change your anaylsis script, you'll still
    # have the correct version in the simulation folder.
    source_file = '/home/chris/Research/PUMPS/Resources/analysisScript.py'
    shutil.copy(source_file, sim_path)  


if __name__ == '__main__':

    main()