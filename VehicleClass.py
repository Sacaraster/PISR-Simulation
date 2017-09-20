from __future__ import division
import random
import itertools
import math
import numpy as np
import dubins

class Vehicle:

    """A class for PISR vehicles."""

    def __init__(self, _indexer, ID, initLoc, initHead, speed, turnRadius, numVehicles):              

        self.ID = ID
        self._indexer = _indexer   #since IDs are usually 100, 200, etc, this makes vehicle index referencing easier
        self.location = [initLoc, 0.0]   # [current_location, current_time]               
        self.targets = np.zeros((numVehicles, 3))  # [task, time_of_arrival, travel_time_to_task]
        self.targets[self._indexer, 0] = initLoc
        self.targets[self._indexer, 1] = 0.0
        self.targets[self._indexer, 2] = 0.0
        self.heading = initHead        
        self.speed = speed
        self.turnRadius = turnRadius
        self.normFactor = 0

    def calctraveleuc(self, taskVector):
        travelTimes = []
        for index, task in enumerate(taskVector):
            dist = math.sqrt(math.pow(task.location[0]-taskVector[int(self.location[0])-1].location[0], 2)+
                math.pow(task.location[1]-taskVector[int(self.location[0])-1].location[1], 2))
            time = dist/self.speed
            time = time/self.normFactor
            travelTimes.append(time)

        travelTimes = np.array(travelTimes)

        return travelTimes

    def calctraveldub(self, taskVector):
        travelTimes = []
        headings = []
        x0 = taskVector[int(self.location[0]-1)].location[0]
        y0 = taskVector[int(self.location[0]-1)].location[1]
        theta0 = self.heading
        for index, task, in enumerate(taskVector):
            pathLengthVector = []
            x1 = task.location[0]
            y1 = task.location[1]
            thetas = np.arange(0, 20, 1.25)*(math.pi/10)
            for theta1 in thetas:                                
                pathLength = dubins.path_length((x0, y0, theta0), (x1, y1, theta1), self.turnRadius)                
                if (int(self.location[0]-1) == index) & (theta0==theta1):
                    pathLength = float('inf')
                pathLengthVector.append(pathLength)
            dist = min(pathLengthVector)
            distindex = np.argmin(pathLengthVector)
            heading = thetas[distindex]*(180/math.pi)            
            time = dist/self.speed
            time = time/self.normFactor
            travelTimes.append(time)
            headings.append(heading)

        travelTimes = np.array(travelTimes)
        headings = np.array(headings)
        
        return travelTimes, headings

class VehicleMD2WRP(Vehicle):

    """A class for PISR vehicles using MD2WRP for task selection."""

    def __init__(self, _indexer, ID, initLoc, initHead, speed, bankAngle, numVehicles, beta, w):
        Vehicle.__init__(self, _indexer, ID, initLoc, initHead, speed, bankAngle, numVehicles)
        self.ageTracker = np.zeros(len(w))
        self.beta = beta
        self.w = w

    def selecttask(self, travelTimesFlight, travelTimesMeasure, headings):
        utilities = []               
        for index, tij in enumerate(travelTimesMeasure):
            ageModifier = 0 
            for otherArrivalIndex, otherArrival in enumerate(self.targets[:, 0]):
                if ((otherArrival == index+1) & (otherArrivalIndex != self._indexer)):                    
                    #If another vehicle arrives before me (or at the same time), reduce the age of the task under consideration
                    if self.targets[otherArrivalIndex, 1] <= (self.ageTracker[index]+tij):                        
                        ageModifier = self.targets[otherArrivalIndex, 1]
                        print '   *** Task {} age reduced!'.format(index+1)
                    #If another vehicle will be arriving after me, don't go to that task
                    if self.targets[otherArrivalIndex, 1] > (self.ageTracker[index]+tij):                        
                        ageModifier = (self.ageTracker[index]+tij)
                        print '   *** Task {} utility set to zero!'.format(index+1)

            utility = (math.exp(-self.beta*tij)*self.w[index]*(self.ageTracker[index]+tij-ageModifier))*100000
            utilities.append(utility)
        print '   Calculated utilities for each task:'
        for index, taskUtility in enumerate(utilities):
            print '      Task {} utility = {}'.format(index+1, taskUtility)
        maxUtility = max(utilities)
        selectedTask = [index+1 for index, utility in enumerate(utilities) if utility == maxUtility]
        print '   Task {} has the highest utility. ({})'.format(selectedTask[0], maxUtility)
        travelTimeFlight = travelTimesFlight[selectedTask[0]-1]
        self.targets[self._indexer, 0] = selectedTask[0]
        self.targets[self._indexer, 1] += travelTimeFlight
        self.targets[self._indexer, 2] = travelTimeFlight
        self.heading = headings[selectedTask[0]-1]

    def cxnone(self):
        pass

    def cxbc(self, vehicleVector):
        for vehicle in vehicleVector:            
            if vehicle.ID != self.ID:
                #Update sister vehicle age tracker's to account for the task just serviced by this vehicle
                #(This is how old the task will now be when the sister vehicle makes its next decision)
                vehicle.ageTracker[int(self.location[0]-1)] = vehicle.targets[vehicle._indexer, 1]-self.location[1]

    def cxbd(self, vehicleVector):
        for vehicle in vehicleVector:            
            if vehicle.ID != self.ID:
                #Let the sister vehicles know which task this vehicle has just selected and what time it will arrive
                vehicle.targets[self._indexer, 0] = self.targets[self._indexer, 0]
                vehicle.targets[self._indexer, 1] = self.targets[self._indexer, 1]

class VehicleManual(Vehicle):

    """A class for PISR vehicles performing a pre-defined tour."""

    def __init__(self, _indexer, ID, initLoc, initHead, speed, bankAngle, numVehicles, tour):
        Vehicle.__init__(self, ID, initLoc, initHead, speed, bankAngle)
        self.tour = tour

    def selecttask(self):
        pass