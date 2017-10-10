import os
import sys
import math
import pickle

import numpy as np
import pandas as pd
import xml.etree.ElementTree as ET
from datetime import datetime
import matplotlib
import matplotlib.lines as mlines
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator

def calcLatency(visitOrder, priorityVector, normFactor):
    latencyStep = .01
    latencyStepDecimals = 2

    numTasks = len(priorityVector)

    visitTimes = visitOrder[:, 2]
    visitTimes = np.array(visitTimes, dtype=float)
    latencyTimeVector = np.arange(visitTimes[0], visitTimes[-1]+2*latencyStep, latencyStep)

    visitTimes = np.around(visitTimes, decimals=latencyStepDecimals)
    latencyTimeVector = np.around(latencyTimeVector, decimals=latencyStepDecimals)

    individualLatencies = np.empty([latencyTimeVector.shape[0], numTasks])
    totalLatency = np.empty(latencyTimeVector.shape[0])

    for timeIndex, timeEntry in enumerate(latencyTimeVector):
        if timeIndex == 0:
            individualLatencies[timeIndex, :] = 0
            totalLatency[timeIndex] = 0

        else:            
            individualLatencies[timeIndex, :] = individualLatencies[timeIndex-1]+(latencyTimeVector[timeIndex]-latencyTimeVector[timeIndex-1])*priorityVector
            totalLatency[timeIndex] = sum(individualLatencies[timeIndex])
            visitCondition = timeEntry == visitTimes        
            if np.any(visitCondition):
                visitIndex = np.nonzero(visitCondition)
                visitedTasks = visitOrder[visitIndex, 1]
                for entry in visitedTasks:
                    for task in entry:
                        individualLatencies[timeIndex, int(task-1)] = 0                 

    latencyTimeVector = np.multiply(latencyTimeVector, normFactor)
    individualLatencies = np.multiply(individualLatencies, normFactor)
    totalLatency = np.multiply(totalLatency, normFactor)
    maxLatency = totalLatency.max()
    avgLatency = totalLatency.mean()

    return latencyTimeVector, individualLatencies, totalLatency, maxLatency, avgLatency

def plotResults(tradeID, latencyTimeVector, normFactor, individualLatencies, totalLatency, visitOrder, 
                priorityVector, commMode, savePath, taskSelectionMethod, vehicleIDs, normVehicles, cmapVehicles, **kwargs):
    
    if kwargs is not None:
        for key, value in kwargs.iteritems():
            if key == 'beta':
                beta = value
            if key == 'w':
                weightVector = value
            if key == 'w0':
                w0 = value
            if key == 'w1':
                w1 = value

    #Create a color map for the tasks
    normTasks = matplotlib.colors.Normalize(vmin=0, vmax=(len(priorityVector)-1))
    cmapTasks = matplotlib.cm.get_cmap('jet') 

    #Generate the figure
    fig, axarr = plt.subplots(2, sharex=True)
    plt.xlabel('Time, (s)', fontsize=14) 
    plt.margins(.02,.1)    #provides buffer around plots to avoid cutting off markers

    #Plot the latency
    for index, task in enumerate(priorityVector):
        axarr[0].plot(latencyTimeVector, individualLatencies[:, index], label='Task[{}]'.format(index+1), color=cmapTasks(normTasks(index)))
    axarr[0].plot(latencyTimeVector, totalLatency, label='Total Latency', color='b')
    if individualLatencies.shape[1] < 8:
        axarr[0].legend(fontsize=9)
    if individualLatencies.shape[1] >= 8:
        axarr[0].legend(fontsize=9, ncol=2)
    if taskSelectionMethod == 'md2wrp':
        axarr[0].set_title('{}:'.format(taskSelectionMethod)
            +r' $\beta$={}, '.format(beta)+r'$w$={0}'.format(weightVector)+'\n'+r'$p$={}'.format(priorityVector)+'\n'+
            r'$\bar L$={}'.format(np.around(totalLatency.mean(),decimals=2))+ r', $L_\max$={}'.format(np.around(totalLatency.max(), decimals=2)),
            fontsize=16)
    # if utilityFunction == 'DLM':
    #     axarr[0].set_title('{}:'.format(utilityFunction)+' Comm Mode={}'.format(commMode)+', {}-lookahead'.format(lookahead)+
    #         '\nP{}'.format(priorityVector)+
    #         '\nL_max={}'.format(np.around(totalLatency.max(),decimals=2))+', L_avg={}'.format(np.around(totalLatency.mean(),decimals=2)),
    #         fontweight='bold', fontsize=16)
    # if utilityFunction == 'TSP':
    #     axarr[0].set_title('{}'.format(utilityFunction)+
    #         '\nP{}'.format(priorityVector)+
    #         '\nL_max={}'.format(np.around(totalLatency.max(),decimals=2))+', L_avg={}'.format(np.around(totalLatency.mean(),decimals=2)),
    #         fontweight='bold', fontsize=16)
    # if utilityFunction == 'Stanford':
    #     axarr[0].set_title('{}'.format(utilityFunction)+
    #         '\nP{}, '.format(priorityVector)+ 'w0[{}], '.format(w0)+ 'w1[{}], '.format(w1)+
    #         '\nL_max={}'.format(np.around(totalLatency.max(),decimals=2))+', L_avg={}'.format(np.around(totalLatency.mean(),decimals=2)),
    #         fontweight='bold', fontsize=16)
    axarr[0].set_ylabel('Latency', fontsize=14)

    #Plot the task visit times
    for entry in visitOrder:
        visitTime = entry[2]*normFactor
        axarr[1].plot(visitTime, entry[1], linestyle='', marker='*', markersize=19,
            color=cmapVehicles(normVehicles(entry[0])))    
    axarr[1].set_ylabel('Task ID', fontsize=14)
    axarr[1].get_yaxis().set_major_locator(MaxNLocator(integer=True))
    vehicleLegendEntries = []
    for vehicle in vehicleIDs:
        vehicleLegendEntries.append(mlines.Line2D([], [], color=cmapVehicles(normVehicles(int(vehicle))),
            linestyle='', marker='*', markersize=15, label='Vehicle[{}]'.format(int(vehicle))))
    axarr[1].legend(handles=[i for i in vehicleLegendEntries], numpoints=1, fontsize=9)

    plt.tight_layout()
    plt.savefig('{0}Trade{1}_{2}.png'.format(savePath, tradeID, taskSelectionMethod))
    plt.close()

def plotScenarioMap(taskVector, taskGeometry, savePath):
    
    labelScaling = (taskVector[:,2].max()-taskVector[:,2].min())*.025

    plt.figure()
    plt.margins(.1)
    for task in taskVector:
        ID = int(task[0])
        x = task[1]
        y = task[2]
        pri = task[3]
        taskLabel = 'T[{}]'.format(ID)
        plt.plot(x,y, color=[.5,.5,.5], linestyle=' ', marker='o', markersize=14)
        plt.text(x,y-labelScaling, taskLabel,
            color=[.1,.1,.1], horizontalalignment='center', verticalalignment='top', size=12, zorder=3)    
        
    plt.title('Task Configuration', fontsize=18)
    plt.xlabel('East, (m)', fontsize=14)
    plt.ylabel('North, (m)', fontsize=14)
    plt.axes().set_aspect('equal')   
    plt.savefig('{0}{1}_ScenarioMap.png'.format(savePath, taskGeometry))

def plotTrajectories(savePath, tradeID, visitOrder, taskVector, vehicleIDs, normVehicles, cmapVehicles):

    plt.figure()
    plt.margins(.1)    
    # Plot each trajectory
    for index, trajectory in enumerate(visitOrder[len(vehicleIDs):, 3], len(vehicleIDs)):
        x = []
        y = []
        veh = int(visitOrder[index, 0])
        for step in trajectory:
            x.append(step[0])
            y.append(step[1])
        plt.plot(x,y, color=cmapVehicles(normVehicles(veh)))
        # plt.show()
    # Overlay the tasks and their priorities
    labelScaling = (taskVector[:,2].max()-taskVector[:,2].min())*.025
    for task in taskVector:
        ID = int(task[0])
        x = task[1]
        y = task[2]
        pri = task[3]
        taskLabel = 'T[{}]\np={}'.format(ID, pri)
        plt.plot(x,y, color=[.5,.5,.5], linestyle=' ', marker='o', markersize=10)
        plt.text(x,y-labelScaling, taskLabel,
            color=[.1,.1,.1], horizontalalignment='center', verticalalignment='top', size=10, zorder=3)

    plt.title('Vehicle Trajectories')
    plt.xlabel('East, (m)', fontsize=14)
    plt.ylabel('North, (m)', fontsize=14)
    plt.axes().set_aspect('equal')
    plt.tight_layout()
    plt.savefig('{0}Trade{1}_Trajectories.png'.format(savePath, tradeID))
    plt.close()

def calcVisitsPerHour(savePath, visitRates, tradeID, visitOrder, normFactor):    

    uniqueTasks = np.unique(visitOrder[:,1])
    timeMax = visitOrder[-1,2]
    runningTime = (timeMax*normFactor)/3600
    # print visitOrder
    for task in uniqueTasks:
        numVisits = len(np.where(task==visitOrder[:,1])[0])
        taskVisitRate = numVisits/runningTime
        visitRates.append([tradeID,task,taskVisitRate])

    return visitRates

def plotVisitsPerHour(savePath, visitRates):

    visitRatesPD = pd.DataFrame(data=visitRates, columns=['Trade ID','Task','Visit Rate'])
    visitRatesPlotablePD = visitRatesPD.pivot(index='Trade ID', columns='Task', values='Visit Rate')

    ax = visitRatesPlotablePD.plot(kind='bar', title='Mean Visit Rates')
    ax.set_xticklabels(np.arange(0, 10.5, .5))
    ax.grid(color='b', linestyle='-', linewidth=0.1)
    # ax.set_xlabel('Trade ID') #, fontweight='bold', fontsize=28)
    ax.set_xlabel(r'Distance Discount Parameter, $\beta$')
    # ax.set_xlabel(r'Weight of Task 3, $w_3$') #, fontweight='bold', fontsize=28)
    ax.set_ylabel('Mean Visit Rate (vph)') #, fontweight='bold', fontsize=28)
    plt.tight_layout()
    plt.savefig('{0}VisitsPerHour.png'.format(savePath))
    plt.close()


def main():

    np.set_printoptions(suppress=True)
    np.set_printoptions(threshold='nan')

    #Specify file where sim data is stored
    dataFile = './Data/simData.xml'

    #Create directory to save the results to
    # savePath = './Data_Analysis_{}/'.format(datetime.now().strftime('%Y%m%d_%H%M%S'))
    # if not os.path.exists(savePath):
    #     os.makedirs(savePath)
    savePath = './' 
    performanceTable = [] 
    visitRates = []   

    #Load sim data from XML file
    e = ET.parse(dataFile).getroot()
    
    #Load data for each trade
    for trade in e.findall('Trade'):
        tradeID = int(trade.find('tradeID').text)
        print '***************************************'
        print 'Loading TradeID={}'.format(tradeID)
        taskSelectionMethod = trade.find('taskSelectionMethod').text
        print '   Task selection method:', taskSelectionMethod
        saveTrajectories = int(float(trade.find('saveTrajectories').text))
        print '   Save Trajectories?', bool(saveTrajectories)
        beta = float(trade.find('beta').text)
        print '   Beta:', beta
        w = eval(trade.find('w').text)
        print '   w:', w
        normFactor = float(trade.find('normFactor').text)
        print '   normFactor:', normFactor
        rdRatio = float(trade.find('rdRatio').text)
        print '   r/d:', rdRatio          
        taskGeometry = trade.find('taskGeometry').text
        print '   taskGeometry:', taskGeometry
        commMode = trade.find('commMode').text
        print '   commMode:', commMode
        
        taskVector = []
        for task in trade.findall('Tasks'):
            taskID = int(float(task.find('Task').text))
            taskxCoord = float(task.find('xCoord').text)
            taskyCoord = float(task.find('yCoord').text)
            taskPriority = int(float(task.find('Priority').text))
            taskVector.append([taskID, taskxCoord, taskyCoord, taskPriority])
        taskVector = np.array(taskVector)

        visitOrder = []
        for visit in trade.findall('Visit'):
            visitVeh = int(float(visit.find('Vehicle').text))
            visitTask = int(float(visit.find('Task').text))
            visitTime = float(visit.find('Time').text)
            if saveTrajectories == True:
                visitTrajectory = np.array(eval(visit.find('Trajectory').text))            
                visitOrder.append([visitVeh, visitTask, visitTime, visitTrajectory])
            elif saveTrajectories == False:
                visitOrder.append([visitVeh, visitTask, visitTime])
        visitOrder = np.array(visitOrder, dtype=object)    

        priorityVector = []
        for task in taskVector:
            priorityVector.append(task[3])            
        priorityVector = np.array(priorityVector)

        print ''
        print '   Performing Latency calculations...' 
        latencyTimeVector, individualLatencies, totalLatency, maxLatency, avgLatency = calcLatency(visitOrder, priorityVector, normFactor)           
        print '   ...complete!\n'
        
        # #Calculate the visit rates for the trade and save for plotting at the end
        # visitRates = calcVisitsPerHour(savePath, visitRates, tradeID, visitOrder, normFactor)

        #Create a color map for the vehicles
        vehicleIDs = np.unique(visitOrder[:,0]) 
        normVehicles = matplotlib.colors.Normalize(vmin=vehicleIDs.min(), vmax=vehicleIDs.max())
        cmapVehicles = matplotlib.cm.get_cmap('Spectral')

    #     print '   Plotting latency and visit times...'
    #     if taskSelectionMethod == 'md2wrp':                    
    #         plotResults(tradeID, latencyTimeVector, normFactor, individualLatencies, totalLatency, visitOrder, 
    #             priorityVector, commMode, savePath, taskSelectionMethod, vehicleIDs, normVehicles, cmapVehicles, 
    #             beta=beta, w=w)
    # #         if utilityFunction == 'DLM':
    # #             plotResults(tradeID, vehicleIDs, latencyTimeVector, normFactor, individualLatencies, totalLatency, visitOrder, lookahead,
    # #                 priorityVector, commMode, savePath, utilityFunction)
    # #         if utilityFunction == 'TSP':
    # #             plotResults(tradeID, vehicleIDs, latencyTimeVector, normFactor, individualLatencies, totalLatency, visitOrder, lookahead,
    # #                 priorityVector, commMode, savePath, utilityFunction)
    # #         if utilityFunction == 'Stanford':                    
    # #             plotResults(tradeID, vehicleIDs, latencyTimeVector, normFactor, individualLatencies, totalLatency, visitOrder, lookahead,
    # #                 priorityVector, commMode, savePath, utilityFunction, w0=w0, w1=w1)
    #     print '   ...complete!\n'
        

        # if saveTrajectories == True:
        #     print '   Plotting trajectories...'
        #     plotTrajectories(savePath, tradeID, visitOrder, taskVector, vehicleIDs, normVehicles, cmapVehicles)
        #     print '   ...complete!\n'       
        
        numVehicles = len(vehicleIDs)
        performanceTable.append([tradeID, numVehicles, avgLatency, maxLatency, beta, w, commMode, rdRatio])
      

        print '************************************************'

    

    # #Plot and save the visit rates chart    
    # print 'Plotting visits per hour...'
    # plotVisitsPerHour(savePath, visitRates)
    # print '...complete!\n'


    # #Plot and save the scenario map  
    # print 'Plotting the scenario map...'                                
    # plotScenarioMap(taskVector, taskGeometry, savePath)
    # print '...complete!\n' 
       

    #Print sorted performance to file and pickle it
    print 'Saving performance summary...'  
    f = open('{}Summary_of_Performance'.format(savePath), 'w')
    sys.stdout = f
    performanceTablePD = pd.DataFrame(data=performanceTable, columns=['TradeID', '# Veh', 'L_bar', 'L_max', 'Beta', 'w', 'Cx Mode', 'r/d'])
    performanceTablePD.sort_values(by=['L_bar', 'L_max'], inplace=True) 
    pd.set_option("display.max_rows",1000)
    pd.set_option("display.max_colwidth",1000) 
    print performanceTablePD  
    sys.stdout = sys.__stdout__
    f.close()
    performanceSummaryPickle = '{0}performanceSummaryPickle.pickle'.format(savePath)
    pickle.dump(performanceTablePD, open(performanceSummaryPickle, "wb"))
    print '...complete!\n'

    print 'ANALYSIS COMPLETE!'
    

################################################################################

if __name__ == '__main__':
    
    main()