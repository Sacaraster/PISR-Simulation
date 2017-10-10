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
    

    rdAxis = [0.02707773,0.0406166,0.04873992, 0.0609249, 0.0812332, 0.09747984,0.10595635,0.1218498, 0.13538867,0.16246641,0.17407115,
        0.20308301, 0.2215451, 0.24369961, 0.27077734, 0.30462451, 0.3481423]    
    
    EEAvgLatency1Veh = []
    EEMaxLatency1Veh = []

    EDAvgLatency1Veh = []
    EDMaxLatency1Veh = []

    DDAvgLatency1Veh = []
    DDMaxLatency1Veh = []
    
    # EEAvgLatency3Veh = [None] * len(rdAxis)
    # EEMaxLatency3Veh = [None] * len(rdAxis)
    # EDAvgLatency3Veh = [None] * len(rdAxis)
    # EDMaxLatency3Veh = [None] * len(rdAxis)
    # DDAvgLatency3Veh = [None] * len(rdAxis)
    # DDMaxLatency3Veh = [None] * len(rdAxis)

    for numVeh in numVehicles:
        for measureType in measureTypes:            

            searchPath = '/home/chris/Research/PISR_Sim_NGplus/Sims/Dubins/Clusters/{}_Vehicle/{}'.format(numVeh, measureType)

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

    # print np.flipud(EDAvgLatency1Veh[:,0])

    #Plot AVG latency versus r/d for ED, DD (EE is plotted against r/d, but really just corresponds to map of same size as r/d)
    ax = plt.figure().gca()
    # plt.plot(rdAxis, EEAvgLatency1Veh, 'ko-', label='EE, 1 Vehicle*', markersize=8)
    plt.plot(EDAvgLatency1Veh[:,0], EDAvgLatency1Veh[:,1], 'go-', label='ED, 1 Vehicle', markersize=8)
    plt.plot(DDAvgLatency1Veh[:,0], DDAvgLatency1Veh[:,1], 'ro-', label='DD, 1 Vehicle', markersize=8)
    plt.title('Clusters Map', fontweight='bold', fontsize=14)
    plt.xlabel('r/d', fontsize=12)
    plt.ylabel(r'Average Latency, $\bar L$', fontsize=12)
    # ax.get_xaxis().set_major_locator(MaxNLocator(integer=True))
    plt.margins(0.05)
    plt.legend(numpoints=1)
    # plt.show()
    plt.tight_layout()
    plt.savefig('{0}dubins_rd_plot_avg_latency'.format(savePath))
    plt.close()

    #Plot AVG latency versus r/d for EE, ED, DD (both 1 and 3 vehicles)
    ax = plt.figure().gca()
    # plt.plot(rdAxis, EEMaxLatency1Veh, 'ko-', label='EE, 1 Vehicle*', markersize=8)
    plt.plot(EDMaxLatency1Veh[:, 0], EDMaxLatency1Veh[:, 1], 'go-', label='ED, 1 Vehicle', markersize=8)
    plt.plot(DDMaxLatency1Veh[:, 0], DDMaxLatency1Veh[:, 1], 'ro-', label='DD, 1 Vehicle', markersize=8)
    plt.title('Clusters Map', fontweight='bold', fontsize=14)
    plt.xlabel('r/d', fontsize=12)
    plt.ylabel(r'Max Latency, $L_{max}$', fontsize=12)
    # ax.get_xaxis().set_major_locator(MaxNLocator(integer=True))
    plt.margins(0.05)
    plt.legend(numpoints=1)
    # plt.show()
    plt.tight_layout()
    plt.savefig('{0}dubins_rd_plot_max_latency'.format(savePath))
    plt.close()




################################################################################
if __name__ == '__main__':
    
    main()