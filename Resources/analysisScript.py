import os
import sys
import math
import pickle

sys.path.append('/home/chris/Research/PISR_Sim_NGpp')
# import Classes.VehicleClass

import numpy as np
import pandas as pd
from datetime import datetime
import matplotlib
import matplotlib.lines as mlines
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator

def calcLatency(task_ages, task_vector, num_vehicles):

    #extract the time steps
    latency_t_vector = task_ages[:,0]

    indiv_latencies = task_ages[:,1:11]
    for task in task_vector:
        indiv_latencies[:, task.ID-1] = indiv_latencies[:, task.ID-1]*task.priority
    
    total_latency = np.sum(task_ages[:,1:11], 1)
    
    max_latency = total_latency.max()
    avg_latency = total_latency[int(num_vehicles*2)-1::].mean()   #this disregards multiple zeros at the beginning

    return latency_t_vector, indiv_latencies, total_latency, max_latency, avg_latency

def plotResults(tradeID, task_vector, vehicle_vector, latency_t_vector, indiv_latencies, total_latency, max_latency, avg_latency, visit_order,
                save_path, normVehicles, cmapVehicles):
    
    #Create a color map for the tasks
    normTasks = matplotlib.colors.Normalize(vmin=0, vmax=(len(task_vector)-1))
    cmapTasks = matplotlib.cm.get_cmap('jet') 

    #Create the priority vector
    priority_vector = []
    for task in task_vector:
        priority_vector.append(task.priority)
    priority_vector = np.array(priority_vector)


    #Generate the figure
    fig, axarr = plt.subplots(2, sharex=True)
    plt.xlabel('Time, (s)', fontsize=14) 
    plt.margins(.02,.1)    #provides buffer around plots to avoid cutting off markers

    #Plot the latency
    # for task in task_vector:
    #     axarr[0].plot(latency_t_vector, indiv_latencies[:, task.ID-1], label='Task[{}]'.format(task.ID), color=cmapTasks(normTasks(task.ID-1)))
    axarr[0].plot(latency_t_vector, total_latency, label='Total Latency', color='b')
    # if indiv_latencies.shape[1] < 8:
    #     axarr[0].legend(fontsize=9)
    # if indiv_latencies.shape[1] >= 8:
    #     axarr[0].legend(fontsize=9, ncol=2)
    axarr[0].set_title('Just a test', fontsize=16)      
    if vehicle_vector[0].routing.type == 'MD2WRP':     #Note: this assumes all vehicles have same routing information
        axarr[0].set_title('{}:'.format(vehicle_vector[0].routing.type)
            +r' $\beta$={}, '.format(vehicle_vector[0].routing.beta)+r'$w$={0}'.format(vehicle_vector[0].routing.w)+'\n'+
            r'$p$={0}'.format(priority_vector)+'\n'+
            r'$\bar L$={}'.format(np.around(avg_latency,decimals=2))+ r', $L_\max$={}'.format(np.around(max_latency, decimals=2)),
            fontsize=16)
    if vehicle_vector[0].routing.type == 'Manual':     #Note: this assumes all vehicles have same routing information
        axarr[0].set_title('{}:'.format(vehicle_vector[0].routing.type)
            +r'$seq$={0}'.format(vehicle_vector[0].routing.sequence_vector)+'\n'+
            r'$p$={0}'.format(priority_vector)+'\n'+
            r'$\bar L$={}'.format(np.around(avg_latency,decimals=2))+ r', $L_\max$={}'.format(np.around(max_latency, decimals=2)),
            fontsize=16)    
    axarr[0].set_ylabel('Latency', fontsize=14)

    #Plot the task visit times
    for entry in visit_order:
        visit_time = entry[2]
        axarr[1].plot(visit_time, entry[1], linestyle='', marker='*', markersize=19,
            color=cmapVehicles(normVehicles(entry[0])))    
    axarr[1].set_ylabel('Task ID', fontsize=14)
    axarr[1].get_yaxis().set_major_locator(MaxNLocator(integer=True))
    vehicleLegendEntries = []
    for vehicle in vehicle_vector:
        vehicleLegendEntries.append(mlines.Line2D([], [], color=cmapVehicles(normVehicles(int(vehicle.ID))),
            linestyle='', marker='*', markersize=15, label='Vehicle[{}]'.format(int(vehicle.ID))))
    axarr[1].legend(handles=[i for i in vehicleLegendEntries], numpoints=1, fontsize=9)

    plt.tight_layout()
    plt.savefig('{0}Trade{1}.png'.format(save_path, tradeID))
    plt.close()

def plotScenarioMap(task_vector, task_geometry, save_path):
    
    max_y = max(task.location[1] for task in task_vector)
    min_y = min(task.location[1] for task in task_vector)

    label_scaling = (max_y-min_y)*.025

    plt.figure()
    plt.margins(.1)
    for task in task_vector:
        taskLabel = 'T[{}]'.format(task.ID)
        plt.plot(task.location[0],task.location[1], color=[.5,.5,.5], linestyle=' ', marker='o', markersize=14)
        plt.text(task.location[0],task.location[1]-label_scaling, taskLabel,
            color=[.1,.1,.1], horizontalalignment='center', verticalalignment='top', size=12, zorder=3)    
        
    plt.title('{} Task Configuration'.format(task_geometry), fontsize=18)
    plt.xlabel('East, (m)', fontsize=14)
    plt.ylabel('North, (m)', fontsize=14)
    plt.axes().set_aspect('equal')   
    plt.savefig('{0}{1}_ScenarioMap.png'.format(save_path, task_geometry))

def plotTrajectories(save_path, tradeID, visit_order, task_vector, vehicle_vector, task_geometry, normVehicles, cmapVehicles):

    plt.figure()
    plt.margins(.1)    
    
    # Plot each trajectory
    for index, trajectory in enumerate(visit_order[len(vehicle_vector):, 3], len(vehicle_vector)):
        x = []
        y = []
        veh = int(visit_order[index, 0])
        for step in trajectory:
            x.append(step[0])
            y.append(step[1])
        plt.plot(x,y, color=cmapVehicles(normVehicles(veh)))
        # plt.show()
    
    # Overlay the tasks and their priorities
    max_y = max(task.location[1] for task in task_vector)
    min_y = min(task.location[1] for task in task_vector)
    label_scaling = (max_y-min_y)*.025
    for task in task_vector:
        taskLabel = 'T[{}]'.format(task.ID)
        plt.plot(task.location[0],task.location[1], color=[.5,.5,.5], linestyle=' ', marker='o', markersize=14)
        plt.text(task.location[0],task.location[1]-label_scaling, taskLabel,
            color=[.1,.1,.1], horizontalalignment='center', verticalalignment='top', size=12, zorder=3)

    # Overlay the no-fly zones (if they exist)
    for vehicle in vehicle_vector:
        if vehicle.pathing.nfz:
            nfz_xcoords, nfz_ycoords = getNFZcoords(task_geometry, vehicle.pathing.nfz)
            plt.plot(nfz_xcoords, nfz_ycoords, '-', color='gray')

    plt.title('Vehicle Trajectories')
    plt.xlabel('East, (m)', fontsize=14)
    plt.ylabel('North, (m)', fontsize=14)
    plt.axes().set_aspect('equal')
    plt.tight_layout()
    plt.savefig('{0}Trade{1}_Trajectories.png'.format(save_path, tradeID))
    plt.close()

def getNFZcoords(task_geometry, nfz):

    if task_geometry == 'random':
        if nfz == 1:
            nfz_xcoords = [0, 0, 500, 500, 0]
            nfz_ycoords = [-2000, 4000, 4000, -2000, -2000]

    if task_geometry == 'clusters':
        if nfz == 1:
            nfz_xcoords = [0, 4000, 4000, 0, 0]
            nfz_ycoords = [0, 0, -1000, -1000, 0]

    return nfz_xcoords, nfz_ycoords


def calcVisitsPerHour(save_path, tradeID, visit_order):

    visit_rates = []    

    unique_tasks = np.unique(visit_order[:,1])
    time_max = visit_order[-1,2]
    running_time = time_max/3600
    # print visit_order
    for task in unique_tasks:
        num_visits = len(np.where(task==visit_order[:,1])[0])
        task_visit_rate = num_visits/running_time
        visit_rates.append([tradeID,task,task_visit_rate])

    return visit_rates

def plotVisitsPerHour(save_path, visit_rates_data):

    visit_ratesPD = pd.DataFrame(data=visit_rates_data, columns=['Trade ID','Task','Visit Rate'])
    visit_ratesPlotablePD = visit_ratesPD.pivot(index='Trade ID', columns='Task', values='Visit Rate')

    ax = visit_ratesPlotablePD.plot(kind='bar', title='Mean Visit Rates')
    # ax.set_xticklabels(np.arange(0, 10.5, .5))
    ax.grid(color='b', linestyle='-', linewidth=0.1)
    ax.set_xlabel('Trade ID') #, fontweight='bold', fontsize=28)
    # ax.set_xlabel(r'Distance Discount Parameter, $\beta$')
    # ax.set_xlabel(r'Weight of Task 3, $w_3$') #, fontweight='bold', fontsize=28)
    ax.set_ylabel('Mean Visit Rate (vph)') #, fontweight='bold', fontsize=28)
    plt.tight_layout()
    plt.savefig('{0}VisitsPerHour.png'.format(save_path))
    plt.close()


def main():

    np.set_printoptions(suppress=True)
    np.set_printoptions(threshold='nan')

    #Specify folder where sim data (pickles) are stored
    data_file = './Data/'
    
    #Specify folder where analyzed data will be saved
    save_path = './'

    performance_table = []
    visit_rates_data = []  
    
    #Open every pickle file (one for each trade) and unpack it
    for file in os.listdir(data_file):
        if file.endswith("_Results.pickle"):
            trade_results_pickle = '{0}{1}'.format(data_file, file)
            print '************************************************'
            print 'Opening \"', trade_results_pickle, '\"\n'
            trade_results = pickle.load(open(trade_results_pickle, "rb"))
            visit_order = trade_results[0]
            task_ages = trade_results[1] 
            task_ages = np.array(task_ages)
            task_vector = trade_results[2]
            vehicle_vector = trade_results[3]
            num_vehicles = len(vehicle_vector)
            tradeID = trade_results[4]
            task_geometry = trade_results[5]              

            # #Calculate the visit rates for the trade and save for plotting at the end
            # !NOT WORKING!
            # print ''
            # print '   Performing visit rate calculations...'
            # visit_rates = calcVisitsPerHour(save_path, tradeID, visit_order)
            # visit_rates_data.append(visit_rates)
            # print '   ...complete!\n'

            print ''
            print '   Performing Latency calculations...' 
            latency_t_vector, indiv_latencies, total_latency, max_latency, avg_latency = calcLatency(task_ages, task_vector, num_vehicles)           
            print '   ...complete!\n'

            #Create a color map for the vehicles
            vehicleIDs = []
            for vehicle in vehicle_vector:
                vehicleIDs.append(vehicle.ID)
            vehicleIDs = np.array(vehicleIDs)
            normVehicles = matplotlib.colors.Normalize(vmin=vehicleIDs.min(), vmax=vehicleIDs.max())
            cmapVehicles = matplotlib.cm.get_cmap('Spectral')

            print '   Plotting latency and visit times...'                            
            plotResults(tradeID, task_vector, vehicle_vector, latency_t_vector, indiv_latencies, total_latency, max_latency, avg_latency, visit_order,
                save_path, normVehicles, cmapVehicles)   
            print '   ...complete!\n'

            print '   Plotting trajectories...'
            plotTrajectories(save_path, tradeID, visit_order, task_vector, vehicle_vector, task_geometry, normVehicles, cmapVehicles)
            print '   ...complete!\n'

            if vehicle_vector[0].routing.type == 'MD2WRP':
                performance_table.append([tradeID, len(vehicle_vector), avg_latency, max_latency,
                    vehicle_vector[0].routing.type, vehicle_vector[0].routing.beta, vehicle_vector[0].routing.w, 'N/A',
                    vehicle_vector[0].comm.type])
            if vehicle_vector[0].routing.type == 'Manual':
                performance_table.append([tradeID, len(vehicle_vector), avg_latency, max_latency,
                    vehicle_vector[0].routing.type, 'N/A', 'N/A', vehicle_vector[0].routing.sequence_vector,
                    vehicle_vector[0].comm.type])


    #Plot and save the scenario map  
    print 'Plotting the scenario map...'                                
    plotScenarioMap(task_vector, task_geometry, save_path)
    print '...complete!\n'

    # #Plot and save the visit rates chart
    # !NOT WORKING!    
    # print 'Plotting visits per hour...'
    # plotVisitsPerHour(save_path, visit_rates_data)
    # print '...complete!\n'       

    #Print sorted performance to file and pickle it
    print 'Saving performance summary...'  
    f = open('{}Summary_of_Performance'.format(save_path), 'w')
    sys.stdout = f
    performance_tablePD = pd.DataFrame(data=performance_table, columns=['TradeID', '# Veh', 'L_bar', 'L_max', 'Routing Type', 'Beta', 'w', 'Sequence' ,'Cx Mode'])
    performance_tablePD.sort_values(by=['L_bar', 'L_max'], inplace=True) 
    pd.set_option("display.max_rows",1000)
    pd.set_option("display.max_colwidth",1000) 
    print performance_tablePD  
    sys.stdout = sys.__stdout__
    f.close()
    performance_summary_pickle = '{0}performance_summary.pickle'.format(save_path)
    pickle.dump(performance_tablePD, open(performance_summary_pickle, "wb"))
    print '...complete!\n'

    print 'ANALYSIS COMPLETE!'
    

################################################################################

if __name__ == '__main__':
    
    main()