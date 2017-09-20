import os
import sys
import math

import numpy as np
import pandas as pd
from datetime import datetime
import matplotlib
import matplotlib.lines as mlines
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import xml.etree.ElementTree as ET
from matplotlib.ticker import MaxNLocator

def calcLatency(visitOrder, taskSelectionMethod, normFactor):
    latencyStep = .01
    latencyStepDecimals = 2

    visitTimes = visitOrder[:, 2]
    latencyTimeVector = np.arange(visitTimes[0], visitTimes[-1]+2*latencyStep, latencyStep)

    visitTimes = np.around(visitTimes, decimals=latencyStepDecimals)
    latencyTimeVector = np.around(latencyTimeVector, decimals=latencyStepDecimals)

    uniqueTasks, uniqueTaskInd = np.unique(visitOrder[:,1], return_index=True)
    numTasks = len(uniqueTasks)
    priorityVector = []
    for index in uniqueTaskInd:
        priorityVector.append(visitOrder[index, 3])
    priorityVector = np.array(priorityVector)

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

    # if utilityFunction != 'Stanford':
    latencyTimeVector = np.multiply(latencyTimeVector, normFactor)
    individualLatencies = np.multiply(individualLatencies, normFactor)
    totalLatency = np.multiply(totalLatency, normFactor)
    maxLatency = totalLatency.max()
    avgLatency = totalLatency.mean()

    return latencyTimeVector, individualLatencies, totalLatency, priorityVector, maxLatency, avgLatency

def plotResults(tradeID, latencyTimeVector, normFactor, individualLatencies, totalLatency, visitOrder, 
                priorityVector, commMode, savePath, taskSelectionMethod, **kwargs):
    
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
    
    vehicleIDs = np.unique(visitOrder[:,0])

    #Create a color map for the vehicles
    normVehicles = matplotlib.colors.Normalize(vmin=vehicleIDs.min(), vmax=vehicleIDs.max())
    cmapVehicles = matplotlib.cm.get_cmap('Spectral')

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
            +r' $\beta$={}, '.format(beta)+r'$w_j$={0}'.format(weightVector)+'\n'+r'$p_j$={}'.format(priorityVector)+'\n'+
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


def plotDiscountFactorChart(betas, travelTimeMatrixNorm, savePath):
    
    tij = np.linspace(0,1,21)
    discountFactor = np.empty(tij.shape[0])    
    
    # plt.figure()
    fig, ax1 = plt.subplots()
    travelTimeMatrixNorm = np.tril(travelTimeMatrixNorm, k=0)
    mask = np.nonzero(travelTimeMatrixNorm)    
    ax1.hist(travelTimeMatrixNorm[mask],color='.5',normed=1, cumulative=1)#, 10, facecolor='green', alpha=0.75)
    ax1.set_ylabel('Cumulative Distribution of tij')
    ax1.axis([0,1,0,1])

    
    ax2 = ax1.twinx()
    for beta in betas:
        for stepIndex, step in enumerate(tij):
            discountFactor[stepIndex] = math.exp(-beta*step)
        # print discountFactor
        ax2.plot(tij, 1-discountFactor, label='Beta={}'.format(beta) )
    ax2.axis([0,1,0,1])
    ax2.legend(loc='best')
    ax2.set_ylabel('Discount Factor')
    ax2.set_title('Utility Discount Factors by Beta')
    ax1.set_xlabel('t_ij (normalized)')


    fig.tight_layout()
    fig.savefig('{0}DiscountFactors.png'.format(savePath))
    # plt.show()
    plt.close(fig)


def plotScenarioMap(rawTaskCoords, taskGeometry, savePath, toFile):
    #Argument toFile == 1, save the vaniall map drawing; toFile == 0, just return the figure without saving

    x = np.real(rawTaskCoords)
    y = np.imag(rawTaskCoords)
    labelScaling = (y.max()-y.min())*.025

    fig = plt.figure()
    plt.margins(.1)
    for taskIndex, task in enumerate(x[0, :]):
        taskLabel = 'T[{0}]'.format(taskIndex+1)
        plt.plot(x[0,taskIndex], y[0,taskIndex], color=[.5,.5,.5], linestyle=' ', marker='o', markersize=14)
        plt.text(x[0,taskIndex], y[0,taskIndex]-labelScaling, taskLabel,
            color=[.1,.1,.1], horizontalalignment='center', verticalalignment='top', size=12, zorder=3)    
        
    plt.title('Task Configuration', fontsize=18)
    plt.xlabel('East (m)', fontsize=14)
    plt.ylabel('North (m)', fontsize=14)
    plt.axes().set_aspect('equal')
    
    if toFile == 1:
        plt.savefig('{0}{1}ScenarioMap.png'.format(savePath, taskGeometry))

    return fig


def plotBetaRings(rawTaskCoords, betas, taskGeometry, discountRadius, savePath):

    x = np.real(rawTaskCoords)
    y = np.imag(rawTaskCoords)
    trueTravelTimeMatrixNorm = abs(rawTaskCoords.T-rawTaskCoords)
    maxTravelTime = trueTravelTimeMatrixNorm.max()


    for beta in betas:
        if beta != 0:
            radius = math.log(1-discountRadius)/(-beta)
            if radius > 1:
                print '        Beta={0} is too small to have a {1}% discount!'.format(beta, int(discountRadius*100))
                break
            radius = radius*maxTravelTime    #convert from normalized tij to true tij
            fig = plotScenarioMap(rawTaskCoords, taskGeometry, savePath, 0)
            ax = fig.gca() 
            for taskIndex, task in enumerate(x[0,:]):
                # if taskIndex in [0,3,6]:
                circle = patches.Circle((x[0,taskIndex], y[0,taskIndex]), radius, fill=0, color='b', clip_on=0)
                ax.add_artist(circle)
            ax.set_title('{0}% Discount Rings for Beta={1}'.format(int(discountRadius*100),beta))
            plt.savefig('{0}{1}withBeta{2}Rings.png'.format(savePath, taskGeometry, beta))
            ax.cla()
            plt.close(fig)


def findPattern(visitOrder, noTasks, normFactor):

    sequence = visitOrder[:,1]
    sequenceTimes = visitOrder[:,2]
    totalVisits = len(sequence)
    sequence = sequence[totalVisits//2:totalVisits+1]
    sequenceTimes = sequenceTimes[totalVisits//2:totalVisits+1]

    max_length = len(sequence)//2
    pattern = np.array([[0]])
    patternLength = 0
    patternCycle = 0
    for x in range(noTasks, max_length):
        if np.array_equal(sequence[0:x], sequence[x:2*x]):
            pattern = sequence[0:x]
            patternLength = len(pattern)
            patternTimes = np.multiply(sequenceTimes[0:x], normFactor)
            patternCycle = patternTimes[-1]-patternTimes[0]
            break

    return pattern, patternLength, patternCycle


def main():

    np.set_printoptions(suppress=True)
    np.set_printoptions(threshold='nan')

    #Specify file where sim data is stored
    dataFile = './Data/simData.xml'

    #Create directory to save the results to
    savePath = './Data_Analysis_{}/'.format(datetime.now().strftime('%Y%m%d_%H%M%S'))    
    if not os.path.exists(savePath):
        os.makedirs(savePath)

    #Load data from XML file
    e = ET.parse(dataFile).getroot()
    for trade in e.findall('Trade'):
        tradeID = int(trade.find('tradeID').text)
        print '***************************************'
        print '   Loading TradeID={}'.format(tradeID)
        taskSelectionMethod = trade.find('taskSelectionMethod').text
        print '      Task selection method:', taskSelectionMethod
        beta = float(trade.find('beta').text)
        print '      Beta:', beta
        w = eval(trade.find('w').text)
        print '      w:', w
        normFactor = float(trade.find('normFactor').text)
        print '      normFactor:', normFactor        
        taskGeometry = trade.find('taskGeometry').text
        print '      taskGeometry:', taskGeometry
        commMode = trade.find('commMode').text
        print '      commMode:', commMode
        visitOrder = []
        for visit in trade.findall('Visit'):
            visitVeh = int(float(visit.find('Vehicle').text))
            visitTask = int(float(visit.find('Task').text))
            visitTime = float(visit.find('Time').text)
            visitPri = float(visit.find('Priority').text)            
            visitOrder.append([visitVeh, visitTask, visitTime, visitPri])
        visitOrder = np.array(visitOrder)
        print(visitOrder)

        print '        Performing Latency calculations...' 
        latencyTimeVector, individualLatencies, totalLatency, priorityVector, maxLatency, avgLatency = calcLatency(visitOrder, taskSelectionMethod, normFactor)           
        print '        Latency calculations complete!\n'

        print '        Plotting latency and visit times...'
        if taskSelectionMethod == 'md2wrp':                    
            plotResults(tradeID, latencyTimeVector, normFactor, individualLatencies, totalLatency, visitOrder, 
                priorityVector, commMode, savePath, taskSelectionMethod, beta=beta, w=w)
    #         if utilityFunction == 'DLM':
    #             plotResults(tradeID, vehicleIDs, latencyTimeVector, normFactor, individualLatencies, totalLatency, visitOrder, lookahead,
    #                 priorityVector, commMode, savePath, utilityFunction)
    #         if utilityFunction == 'TSP':
    #             plotResults(tradeID, vehicleIDs, latencyTimeVector, normFactor, individualLatencies, totalLatency, visitOrder, lookahead,
    #                 priorityVector, commMode, savePath, utilityFunction)
    #         if utilityFunction == 'Stanford':                    
    #             plotResults(tradeID, vehicleIDs, latencyTimeVector, normFactor, individualLatencies, totalLatency, visitOrder, lookahead,
    #                 priorityVector, commMode, savePath, utilityFunction, w0=w0, w1=w1)
        print '        Latency and visit times complete!\n'
        print '        ************************************************'


    
    # betas.sort(reverse=True)

    # #Plot and save the scenario map  
    # print ''
    # print '    Plotting the scenario map...'                                
    # plotScenarioMap(rawTaskCoords, taskGeometry, savePath, 1)
    # print '    Map plotted!\n'

    # # #Plot the scenario maps with beta rings
    # # print '    Plotting the beta rings...'    
    # # plotBetaRings(rawTaskCoords, betas, taskGeometry, discountRadius, savePath)    
    # # print '    Beta rings plotted!\n'

    # # #Plot the beta vs tij chart
    # # print '    Plotting the discount factor chart...'    
    # # plotDiscountFactorChart(betas, travelTimeMatrixNorm, savePath)
    # # print '    Discount factor chart complete!'    

    # #Print sorted performance to screen and to file
    # #Sort the trades by performance, first by avg latency and then by max latency    
    # tradeDataArchivePD = pd.DataFrame(data=tradeDataArchive, columns=['TradeID', 'L_bar', 'L_max', 'Beta', 'W_j', 'w0', 'w1', 'Pattern', 'Pattern Length', 'Cycle Time' , 'Util. Fx', 'Cx Mode', 'Start Tasks', 'Lookaheads','P_j', 'Task Geometry'])
    # tradeDataArchivePD.to_pickle('{0}tradeDataArchive.pickle'.format(savePath))
    # f = open('{}Summary_of_Performance'.format(savePath), 'w')
    # sys.stdout = f    
    # pd.set_option("display.max_rows",1000)
    # pd.set_option("display.max_colwidth",1000)
    # print tradeDataArchivePD.sort_values(by=['L_bar'])

    # # print tradeDataArchivePD[['TradeID', 'L_bar', 'L_max', 'Beta', 'W_j', 'Pattern Length', 'Cycle Time' , 'Start Tasks']]
    # f.close()

    # sys.stdout = sys.__stdout__
    # print '\nAnalysis complete!'
    

################################################################################

if __name__ == '__main__':
    
    main()