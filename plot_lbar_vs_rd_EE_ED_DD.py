import os
import pickle
import numpy as np
import matplotlib
import matplotlib.pyplot as plt 
from matplotlib.ticker import MaxNLocator


def main():
    
    numVehicles = np.array([int(1)])
    measureTypes = ['EE', 'ED', 'DD']
    savePath = './'
    

    rdAxis = [0.0384842021204, 0.0577263031806, 0.0692715638167, 0.0865894547709, 0.115452606361, 0.138543127633, 
        0.150590356123, 0.173178909542,0.192421010602,0.230905212722,0.247398442203,0.288631515903,0.314870744621,0.346357819084,
        0.384842021204, 0.432947273854,0.494796884405]    
    
    EEAvgLatency1Veh = []
    EEMaxLatency1Veh = []

    EDAvgLatency1Veh = []
    EDMaxLatency1Veh = []

    DDAvgLatency1Veh = []
    DDMaxLatency1Veh = []
    
    EEAvgLatency3Veh = [None] * len(rdAxis)
    EEMaxLatency3Veh = [None] * len(rdAxis)
    EDAvgLatency3Veh = [None] * len(rdAxis)
    EDMaxLatency3Veh = [None] * len(rdAxis)
    DDAvgLatency3Veh = [None] * len(rdAxis)
    DDMaxLatency3Veh = [None] * len(rdAxis)

    for numVeh in numVehicles:
        for measureType in measureTypes:            

            searchPath = '/home/chris/Research/PISR_Sim_NGplus/Sims/Dubins/Random/{}_Vehicle/{}'.format(numVeh, measureType)

            for root,dirs,files in os.walk(searchPath):
                for file in files:
                    if file.startswith('performanceSummaryPickle'):
                        performanceSummaryPD = pickle.load(open(os.path.join(root, file), 'rb'))

                        # Pull out average and max latency
                        avgLatency = np.min(np.array(performanceSummaryPD['L_bar']))                                         
                        maxLatency = np.min(np.array(performanceSummaryPD['L_max']))

                        rd = float(np.array(performanceSummaryPD['r/d'][0]))                     

                        # Save latency according to type (EE, ED, DD)                   
                        if (measureType == 'EE') & (numVeh == int(1)):
                            EEAvgLatency1Veh.append(avgLatency)                        
                            EEMaxLatency1Veh.append(maxLatency)

                        if (measureType == 'ED') & (numVeh == int(1)):                           
                            EDAvgLatency1Veh.append([rd, avgLatency])                         
                            EDMaxLatency1Veh.append([rd, maxLatency]) 

                        if (measureType == 'DD') & (numVeh == int(1)):
                            DDAvgLatency1Veh.append([rd, avgLatency])                        
                            DDMaxLatency1Veh.append([rd, maxLatency])

    
    EEAvgLatency1Veh = sorted(EEAvgLatency1Veh, reverse=True)
    EEMaxLatency1Veh = sorted(EEMaxLatency1Veh, reverse=True)
    
    EDAvgLatency1Veh = sorted(EDAvgLatency1Veh,key=lambda x: x[0], reverse=True)
    EDAvgLatency1Veh = np.array(EDAvgLatency1Veh)
    EDMaxLatency1Veh = sorted(EDMaxLatency1Veh,key=lambda x: x[0], reverse=True)
    EDMaxLatency1Veh = np.array(EDMaxLatency1Veh)

    DDAvgLatency1Veh = sorted(DDAvgLatency1Veh,key=lambda x: x[0], reverse=True)
    DDAvgLatency1Veh = np.array(DDAvgLatency1Veh)
    DDMaxLatency1Veh = sorted(DDMaxLatency1Veh,key=lambda x: x[0], reverse=True)
    DDMaxLatency1Veh = np.array(DDMaxLatency1Veh)

    #Plot AVG latency versus r/d for ED, DD (EE is plotted against r/d, but really just corresponds to map of same size as r/d)
    ax = plt.figure().gca()
    # plt.plot(rdAxis, EEAvgLatency1Veh, 'ko-', label='EE, 1 Vehicle*', markersize=8)
    plt.plot(EDAvgLatency1Veh[:,0], EDAvgLatency1Veh[:,1], 'go-', label='ED, 1 Vehicle', markersize=8)
    plt.plot(DDAvgLatency1Veh[:,0], DDAvgLatency1Veh[:,1], 'ro-', label='DD, 1 Vehicle', markersize=8)
    plt.title('Random Map', fontweight='bold', fontsize=14)
    plt.xlabel('r/d', fontsize=12)
    plt.ylabel(r'Average Latency, $\bar L$', fontsize=12)
    # ax.get_xaxis().set_major_locator(MaxNLocator(integer=True))
    plt.margins(0.05)
    plt.legend(numpoints=1)
    # plt.show()
    plt.tight_layout()
    plt.savefig('{0}dubinsRDLbar_Random'.format(savePath))
    plt.close()

    #Plot AVG latency versus r/d for EE, ED, DD (both 1 and 3 vehicles)
    ax = plt.figure().gca()
    # plt.plot(rdAxis, EEMaxLatency1Veh, 'ko-', label='EE, 1 Vehicle*', markersize=8)
    plt.plot(EDMaxLatency1Veh[:, 0], EDMaxLatency1Veh[:, 1], 'go-', label='ED, 1 Vehicle', markersize=8)
    plt.plot(DDMaxLatency1Veh[:, 0], DDMaxLatency1Veh[:, 1], 'ro-', label='DD, 1 Vehicle', markersize=8)
    plt.title('Random Map', fontweight='bold', fontsize=14)
    plt.xlabel('r/d', fontsize=12)
    plt.ylabel(r'Max Latency, $L_{max}$', fontsize=12)
    # ax.get_xaxis().set_major_locator(MaxNLocator(integer=True))
    plt.margins(0.05)
    plt.legend(numpoints=1)
    # plt.show()
    plt.tight_layout()
    plt.savefig('{0}dubinsRDLmax_Random'.format(savePath))
    plt.close()




################################################################################
if __name__ == '__main__':
    
    main()