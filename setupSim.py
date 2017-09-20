# import sys
# import math
# import matplotlib.pyplot as plt
# import pandas as pd
import os
import shutil
import itertools
import numpy as np

from generateMapCoordinates import generateMapCoordinates

def genConfigFile(simPath, tradeID, taskSelectionMethod, taskSelectionDistanceMeasures, taskStarts, commModes, 
        initLocations, initHeadings, vehSpeed, vehBankAngle, taskGeometry, priorities, xTaskCoords, yTaskCoords, **kwargs):    

    betas = [[None]]
    ws = [[None]]
    tours = [[None]]
    if kwargs is not None:
        for key, value in kwargs.iteritems():
            if (key == 'betas') & (taskSelectionMethod == 'md2wrp'):
                betas = value
            if (key == 'ws') & (taskSelectionMethod == 'md2wrp'):
                ws = value
            if (key == 'tours') & (taskSelectionMethod == 'manual'):
                tours = value

    fileName = simPath + 'simConfiguration.xml'

    configFile = open(fileName,'w')
    configFile.write('<Sim_Config>\n')
    
    initLocationsVectorArray = np.array(list(itertools.product(*initLocations)))   #create all init task trade combos
    initHeadingsVectorArray = np.array(list(itertools.product(*initHeadings)))   #create all init heading trade combos
    wVectorArray = np.array(list(itertools.product(*ws)))   #create all weight vector trade combos
    priorityVectorArray = np.array(list(itertools.product(*priorities))) 
    for taskSelectionDistanceMeasure in taskSelectionDistanceMeasures:
        for commMode in commModes:
            for initLocationsVector in initLocationsVectorArray:
                for initHeadingsVector in initHeadingsVectorArray:
                    for priorityVector in priorityVectorArray:
                        for beta in betas:
                            for w in wVectorArray:
                                for tour in tours:
                                    configFile.write('\t<Trade>\n')
                                    configFile.write('\t\t<tradeID>{}</tradeID>\n'.format(tradeID))
                                    genScenarioConfig(configFile, taskSelectionMethod, taskSelectionDistanceMeasure,
                                        taskStarts, commMode)
                                    genVehicleConfig(configFile, initLocationsVector, initHeadingsVector, vehSpeed,
                                        vehBankAngle, taskSelectionMethod, beta, w, tour)
                                    genTaskConfig(configFile, taskGeometry, xTaskCoords, yTaskCoords, priorityVector)
                                
                                    configFile.write('\t</Trade>\n')
                                    tradeID += 1
    
    
    configFile.write('</Sim_Config>')

    configFile.close()
    

def genVehicleConfig(configFile, initLocations, initHeadings, vehSpeed, vehBankAngle, taskSelectionMethod, beta, w, tour):    

    configFile.write('\t\t<Vehicle_Info>\n')

    for index, vehicle in enumerate(range(1, len(initLocations)+1)):
        vehicleID = int(vehicle*100)
        configFile.write('\t\t\t<Vehicle>\n')
        configFile.write('\t\t\t\t<ID>{}</ID>\n'.format(vehicleID))
        configFile.write('\t\t\t\t<initLocation>{}</initLocation>\n'.format(initLocations[index]))
        configFile.write('\t\t\t\t<initHeading>{}</initHeading>\n'.format(initHeadings[index]))
        configFile.write('\t\t\t\t<vehSpeed>{}</vehSpeed>\n'.format(vehSpeed))
        configFile.write('\t\t\t\t<vehBankAngle>{}</vehBankAngle>\n'.format(vehBankAngle))
        if taskSelectionMethod == 'md2wrp':
            configFile.write('\t\t\t\t<beta>{}</beta>\n'.format(beta))
            configFile.write('\t\t\t\t<w>[')
            for wj in w[:-1]:
                configFile.write('{},'.format(wj))
            configFile.write('{}]</w>'.format(w[-1]))
        if taskSelectionMethod == 'manual':
            configFile.write('\t\t\t\t<tour>{}</tour>\n'.format(tour))
        configFile.write('\t\t\t</Vehicle>\n')
    

    configFile.write('\t\t</Vehicle_Info>\n')

def genTaskConfig(configFile, taskGeometry, xTaskCoords, yTaskCoords, priorities):   

    configFile.write('\t\t<Task_Info>\n')
    configFile.write('\t\t\t<TaskGeometry>{}</TaskGeometry>\n'.format(taskGeometry))

    for index, task in enumerate(range(1, len(xTaskCoords)+1)):
        taskID = int(task)
        configFile.write('\t\t\t<Task>\n')
        configFile.write('\t\t\t\t<ID>{}</ID>\n'.format(taskID))
        configFile.write('\t\t\t\t<xCoord>{}</xCoord>\n'.format(xTaskCoords[index]))
        configFile.write('\t\t\t\t<yCoord>{}</yCoord>\n'.format(yTaskCoords[index]))
        configFile.write('\t\t\t\t<priority>{}</priority>\n'.format(priorities[index]))
        configFile.write('\t\t\t</Task>\n')

    configFile.write('\t\t</Task_Info>\n')

def genScenarioConfig(configFile, taskSelectionMethod, taskSelectionDistanceMeasure, taskStarts, commMode):

    configFile.write('\t\t<Scenario_Parameters>\n')    

    configFile.write('\t\t\t<taskSelectionMethod name="'+taskSelectionMethod+'"> </taskSelectionMethod>\n')    

    configFile.write('\t\t\t<taskSelectionDistanceMeasure name="'+taskSelectionDistanceMeasure+'"> </taskSelectionDistanceMeasure>\n')

    configFile.write('\t\t\t<taskStarts>{}</taskStarts>\n'.format(taskStarts))

    configFile.write('\t\t\t<commMode name="'+commMode+'"> </commMode>\n')

    configFile.write('\t\t</Scenario_Parameters>\n')


def main():

    #############################################
    ########## Simulation Setup #################
    #############################################    

    ### SCENARIO PARAMETERS ###
    #Specify a directory for the set of simulations (config files and results)
    simPath = './Sims/development/'      

    #How will tasks be selected?
    taskSelectionMethod = 'md2wrp'
    # taskSelectionMethod = 'manual'
    
    #Will travel time be based on Euclidean or Dubins paths?
    taskSelectionDistanceMeasures = ['euclidean']
    taskSelectionDistanceMeasures = ['dubins']
    # taskSelectionDistanceMeasures = ['euclidean', 'dubins']

    #How many decision to make? (i.e. no. of tasks to accomplish)
    taskStarts = 250

    #What type of communication to use?
    commModes = ['none']
    # commModes = ['CxBC']
    # commModes = ['CxBD']
    # commModes = ['none', 'CxBD']    
    ###########################


    ### VEHICLES #####
    #Start Locations (task where each vehicle will start; length of this array is number of vehicles)
    initLocations = np.array([[1]])
    # initLocations = np.array([[1], [4]])
    # initLocations = np.array([[1], [4], [8], [9]])

    #Initial Headings (degrees)
    initHeadings = np.array([[0]])
    # initHeadings = np.array([[0], [0]])
    # initHeadings = np.array([[0], [0], [0], [0]])

    #Vehicle speed (m/s)
    vehSpeed = 22

    #Vehicle bank angle (assume always max turn)
    vehBankAngle = 90

    #For 'md2wrp', specify beta and weights (this setup script currently assumes each vehicle has same parameters,
    #but sim can handle different betas and weights
    #...ignored if not using md2wrp
    betas = [7.0]
    ws = np.array([[1], [1], [1]])
    ws = np.array([[1], [1], [1], [1], [1], [1], [1], [1], [1], [1]])

    #For 'manual', specify the tour (this setup script currently assumes each vehicle has same tour,
    #but sim can handle different tours
    #...ignored if not susing manual
    tours = [[1,2,3,2], [1,2,3]]
    ##################

    
    ### TASKS ########
    taskGeometry = 'isotri'
    # taskGeometry = 'clusters'
    taskGeometry = 'random'    
    # taskGeometry = 'custom'

    # priorities = np.array([[3], [2], [1]])
    priorities = np.array([[1], [1], [1], [1], [10], [1], [1], [1], [1], [1]])

    # for taskGeometry = 'custom'
    if taskGeometry == 'custom':
        xTaskCoords = [-100, 100, 0]
        yTaskCoords = [0,    0,   400]
    ##################
    

    #############################################
    ########## End Setup ########################
    #############################################

    if not os.path.exists(simPath):
            os.makedirs(simPath)

    if(taskGeometry != 'custom'):
        xTaskCoords, yTaskCoords = generateMapCoordinates(taskGeometry)

    tradeID = 1000  #initial trade ID number

    genConfigFile(simPath, tradeID, taskSelectionMethod, taskSelectionDistanceMeasures, taskStarts, commModes, 
        initLocations, initHeadings, vehSpeed, vehBankAngle, taskGeometry, priorities, xTaskCoords, yTaskCoords,
        betas=betas, ws=ws, tours=tours)

    #Create a bash script to run the simulation(s)
    runSimScript = '{0}{1}'.format(simPath,'runTrades.sh')
    fileRun = open(runSimScript,'w')
    fileRun.write('#! /bin/bash\n')
    fileRun.write('rm -R ./Data\n')
    fileRun.write('mkdir {0}\n'.format('./Data'))
    fileRun.write('python ../../runSim.py ./')
    os.chmod(runSimScript, 0777)

    #copy the analysis script that works with these results into the directory
    sourceFile = './Resources/analysisScript.py'
    shutil.copy(sourceFile, simPath)  


if __name__ == '__main__':

    main()