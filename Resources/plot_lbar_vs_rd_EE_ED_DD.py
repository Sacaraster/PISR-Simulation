import os
import pickle
import numpy as np
import matplotlib
import matplotlib.pyplot as plt 
from matplotlib.ticker import MaxNLocator


def main():
    
    numVehicles = np.array([int(1), int(3)])
    measureTypes = ['ED', 'DD']
    mapType = 'Circle'
    savePath = './'    

    EDAvgLatency1Veh = []
    EDMaxLatency1Veh = []
    EDAvgLatency3Veh = []
    EDMaxLatency3Veh = []

    DDAvgLatency1Veh = []
    DDMaxLatency1Veh = []
    DDAvgLatency3Veh = []
    DDMaxLatency3Veh = []    


    for numVeh in numVehicles:
        for measureType in measureTypes:            

            searchPath = '/home/chris/Research/PISR_Sim_NGplus/Sims/Dubins/{}/{}_Vehicle/{}'.format(mapType, numVeh, measureType)

            for root,dirs,files in os.walk(searchPath):
                for file in files:
                    if file.startswith('performanceSummaryPickle'):
                        performanceSummaryPD = pickle.load(open(os.path.join(root, file), 'rb'))

                        # Pull out average and max latency
                        avgLatency = np.min(np.array(performanceSummaryPD['L_bar']))                                         
                        maxLatency = np.min(np.array(performanceSummaryPD['L_max']))

                        rd = float(np.array(performanceSummaryPD['r/d'][0]))                     

                        # Save latency according to type (ED, DD)
                        if (measureType == 'ED') & (numVeh == int(1)):                           
                            EDAvgLatency1Veh.append([rd, avgLatency])                         
                            EDMaxLatency1Veh.append([rd, maxLatency])

                        if (measureType == 'ED') & (numVeh == int(3)):                           
                            EDAvgLatency3Veh.append([rd, avgLatency])                         
                            EDMaxLatency3Veh.append([rd, maxLatency])  

                        if (measureType == 'DD') & (numVeh == int(1)):
                            DDAvgLatency1Veh.append([rd, avgLatency])                        
                            DDMaxLatency1Veh.append([rd, maxLatency])                        

                        if (measureType == 'DD') & (numVeh == int(3)):
                            DDAvgLatency3Veh.append([rd, avgLatency])                        
                            DDMaxLatency3Veh.append([rd, maxLatency])

    
    
    EDAvgLatency1Veh = sorted(EDAvgLatency1Veh,key=lambda x: x[0], reverse=True)
    EDAvgLatency1Veh = np.array(EDAvgLatency1Veh)
    EDMaxLatency1Veh = sorted(EDMaxLatency1Veh,key=lambda x: x[0], reverse=True)
    EDMaxLatency1Veh = np.array(EDMaxLatency1Veh)

    EDAvgLatency3Veh = sorted(EDAvgLatency3Veh,key=lambda x: x[0], reverse=True)
    EDAvgLatency3Veh = np.array(EDAvgLatency3Veh)
    EDMaxLatency3Veh = sorted(EDMaxLatency3Veh,key=lambda x: x[0], reverse=True)
    EDMaxLatency3Veh = np.array(EDMaxLatency3Veh)

    DDAvgLatency1Veh = sorted(DDAvgLatency1Veh,key=lambda x: x[0], reverse=True)
    DDAvgLatency1Veh = np.array(DDAvgLatency1Veh)
    DDMaxLatency1Veh = sorted(DDMaxLatency1Veh,key=lambda x: x[0], reverse=True)
    DDMaxLatency1Veh = np.array(DDMaxLatency1Veh)

    DDAvgLatency3Veh = sorted(DDAvgLatency3Veh,key=lambda x: x[0], reverse=True)
    DDAvgLatency3Veh = np.array(DDAvgLatency3Veh)
    DDMaxLatency3Veh = sorted(DDMaxLatency3Veh,key=lambda x: x[0], reverse=True)
    DDMaxLatency3Veh = np.array(DDMaxLatency3Veh)


    #Plot AVG latency versus r/d for ED, DD --- 1 Veh
    ax = plt.figure().gca()
    plt.plot(EDAvgLatency1Veh[:,0], EDAvgLatency1Veh[:,1], 'go-', label='ED', markersize=8)
    plt.plot(DDAvgLatency1Veh[:,0], DDAvgLatency1Veh[:,1], 'ro-', label='DD', markersize=8)
    plt.title('{} Map, 1 Vehicle'.format(mapType), fontweight='bold', fontsize=14)
    plt.xlabel('r/d', fontsize=12)
    plt.ylabel(r'Average Latency, $\bar L$', fontsize=12)
    # ax.get_xaxis().set_major_locator(MaxNLocator(integer=True))
    plt.margins(0.05)
    plt.legend(numpoints=1)
    # plt.show()
    plt.tight_layout()
    plt.savefig('{}dubinsRDLbar_{}_1veh'.format(savePath, mapType))
    plt.close()

    #Plot MAX latency versus r/d for ED, DD --- 1 veh
    ax = plt.figure().gca()
    plt.plot(EDMaxLatency1Veh[:, 0], EDMaxLatency1Veh[:, 1], 'go-', label='ED', markersize=8)
    plt.plot(DDMaxLatency1Veh[:, 0], DDMaxLatency1Veh[:, 1], 'ro-', label='DD', markersize=8)
    plt.title('{} Map, 1 Vehicle'.format(mapType), fontweight='bold', fontsize=14)
    plt.xlabel('r/d', fontsize=12)
    plt.ylabel(r'Max Latency, $L_{max}$', fontsize=12)
    # ax.get_xaxis().set_major_locator(MaxNLocator(integer=True))
    plt.margins(0.05)
    plt.legend(numpoints=1)
    # plt.show()
    plt.tight_layout()
    plt.savefig('{}dubinsRDLmax_{}_1veh'.format(savePath, mapType))
    plt.close()

    #Plot AVG latency versus r/d for ED, DD --- 3 Veh
    ax = plt.figure().gca()
    plt.plot(EDAvgLatency3Veh[:,0], EDAvgLatency3Veh[:,1], 'go-', label='ED', markersize=8)
    plt.plot(DDAvgLatency3Veh[:,0], DDAvgLatency3Veh[:,1], 'ro-', label='DD', markersize=8)
    plt.title('{} Map, 3 Vehicles'.format(mapType), fontweight='bold', fontsize=14)
    plt.xlabel('r/d', fontsize=12)
    plt.ylabel(r'Average Latency, $\bar L$', fontsize=12)
    # ax.get_xaxis().set_major_locator(MaxNLocator(integer=True))
    plt.margins(0.05)
    plt.legend(numpoints=1)
    # plt.show()
    plt.tight_layout()
    plt.savefig('{}dubinsRDLbar_{}_3veh'.format(savePath, mapType))
    plt.close()

    #Plot MAX latency versus r/d for ED, DD --- 3 veh
    ax = plt.figure().gca()
    plt.plot(EDMaxLatency3Veh[:, 0], EDMaxLatency3Veh[:, 1], 'go-', label='ED', markersize=8)
    plt.plot(DDMaxLatency3Veh[:, 0], DDMaxLatency3Veh[:, 1], 'ro-', label='DD', markersize=8)
    plt.title('{} Map, 3 Vehicles'.format(mapType), fontweight='bold', fontsize=14)
    plt.xlabel('r/d', fontsize=12)
    plt.ylabel(r'Max Latency, $L_{max}$', fontsize=12)
    # ax.get_xaxis().set_major_locator(MaxNLocator(integer=True))
    plt.margins(0.05)
    plt.legend(numpoints=1)
    # plt.show()
    plt.tight_layout()
    plt.savefig('{}dubinsRDLmax_{}_3veh'.format(savePath, mapType))
    plt.close()




################################################################################
if __name__ == '__main__':
    
    main()