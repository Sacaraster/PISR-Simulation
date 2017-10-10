import os
import pickle
import numpy as np
import matplotlib
import matplotlib.pyplot as plt 
from matplotlib.ticker import MaxNLocator


def main():
    ##############################################    
    # mapType = 'Circle'
    # mapType = 'Clusters'
    # mapType = 'Grid'
    mapType = 'Random'    
    maxVehicles = 4
    measureTypes = ['EE', 'ED', 'DD']
    ##############################################

    savePath = './'
    searchPath = './{}/'.format(mapType)

    EEAvgLatency = [None] * maxVehicles
    EEMaxLatency = [None] * maxVehicles
    EDAvgLatency = [None] * maxVehicles
    EDMaxLatency = [None] * maxVehicles
    DDAvgLatency = [None] * maxVehicles
    DDMaxLatency = [None] * maxVehicles
    
    for vehicle in xrange(1, maxVehicles+1, 1):
        for measureType in measureTypes:
            newPath = searchPath + '{}_Vehicle/{}/'.format(vehicle, measureType)
            for root,dirs,files in os.walk(newPath):
                for name in files:
                    if name.endswith("performanceSummaryPickle.pickle"):
                        performanceSummaryPD = pickle.load(open(os.path.join(root, name), 'rb'))

                        #Pull out average and max latency
                        avgLatency = performanceSummaryPD['L_bar']
                        avgLatencyIndex = np.argmin(avgLatency)
                        avgLatency = avgLatency.min()                    
                        maxLatency = performanceSummaryPD['L_max']
                        maxLatency = maxLatency[avgLatencyIndex]

                        #Save latency according to type (EE, ED, DD)                   
                        if measureType == 'EE':
                            EEAvgLatency[vehicle-1] = avgLatency                        
                            EEMaxLatency[vehicle-1] = maxLatency 

                        if measureType == 'ED':
                            EDAvgLatency[vehicle-1] = avgLatency                        
                            EDMaxLatency[vehicle-1] = maxLatency

                        if measureType == 'DD':
                            DDAvgLatency[vehicle-1] = avgLatency                        
                            DDMaxLatency[vehicle-1] = maxLatency 



    print 'Lbar for EE=', EEAvgLatency
    print 'Lmax for EE=', EEMaxLatency
    print 'Lbar for ED=', EDAvgLatency
    print 'Lmax for ED=', EDMaxLatency
    print 'Lbar for DD=', DDAvgLatency
    print 'Lmax for DD=', DDMaxLatency




    #Plot AVG latency versus num vehicles for EE, ED, DD
    ax = plt.figure().gca()
    plt.plot(range(1, maxVehicles+1, 1), EEAvgLatency, 'ko-', label='EE', markersize=8)
    plt.plot(range(1, maxVehicles+1, 1), EDAvgLatency, 'go-', label='ED', markersize=8)
    plt.plot(range(1, maxVehicles+1, 1), DDAvgLatency, 'ro-', label='DD', markersize=8)
    plt.title('\'{}\' Map'.format(mapType), fontweight='bold', fontsize=14)
    plt.xlabel('Number of Vehicles', fontsize=12)
    plt.ylabel(r'Average Latency, $\bar L$', fontsize=12)
    ax.get_xaxis().set_major_locator(MaxNLocator(integer=True))
    plt.margins(0.05)
    plt.legend(numpoints=1)
    # plt.show()
    plt.tight_layout()
    plt.savefig('{0}measureTypeTunedWithEE_{1}.png'.format(savePath, mapType))
    plt.close()

#     # #Plot MAX latency versus num vehicles for Stanford and MD2WRP
#     # ax = plt.figure().gca()
#     # plt.plot(numVehicles, stanfordMaxLatency, 'ko-', label='SRP/MRP', markersize=8)
#     # plt.plot(numVehicles, MD2WRPMaxLatency, 'go-', label='MD2WRP', markersize=8)
#     # plt.title('\'{}\' Map'.format(mapType), fontweight='bold', fontsize=14)
#     # plt.xlabel('Number of Vehicles', fontsize=12)
#     # plt.ylabel(r'Max Latency, $L_{\max}$', fontsize=12)
#     # ax.get_xaxis().set_major_locator(MaxNLocator(integer=True))
#     # plt.margins(0.05)
#     # plt.legend(numpoints=1)
#     # # plt.show()
#     # plt.tight_layout()
#     # plt.savefig('{0}Stanford_vs_MD2WRP-MAX_{1}.png'.format(savePath, mapType))
#     # plt.close()



################################################################################
if __name__ == '__main__':
    
    main()