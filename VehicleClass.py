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
        self.location = [initLoc, 0.0]   # [current_task_location, current_time]               
        self.targets = np.zeros((numVehicles, 3))  # table of what other vehicles are doing; [task, time_of_arrival, travel_time_to_task]
        self.targets[self._indexer, 0] = initLoc
        self.targets[self._indexer, 1] = 0.0
        self.targets[self._indexer, 2] = 0.0
        self.heading = initHead        
        self.speed = speed
        self.turnRadius = turnRadius
        self.trajectory = []
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
        trajectories = []
        #Coordinates of current location and current heading
        x0 = taskVector[int(self.location[0]-1)].location[0]
        y0 = taskVector[int(self.location[0]-1)].location[1]
        theta0 = self.heading
        #For every candidate task...        
        for index, task, in enumerate(taskVector):            
            pathLengthVector = []
            #Coordinates of candidate task
            x1 = task.location[0]
            y1 = task.location[1]
            #Discretized arrival headings (try each of these and pick the one with the shortest travel distance)
            thetas = np.arange(0, 20, 1.25)*(math.pi/10)
            #If arriving at the current task at the current heading, slightly change arrival angle (prevents travel time of zero)
            for thetaIndex, theta in enumerate(thetas):                
                if (int(self.location[0]-1) == index) & (theta0==theta):
                    thetas[thetaIndex] = theta0 + 0.0174533   #add 1 degree to arrivalangle
            for theta1 in thetas:
                #Cacluate the path length for given arrival angle
                pathLength = dubins.path_length((x0, y0, theta0), (x1, y1, theta1), self.turnRadius)
                pathLengthVector.append(pathLength)
            #find the shortest travel distance for all arrival heading options
            dist = min(pathLengthVector)
            distindex = np.argmin(pathLengthVector)
            heading = thetas[distindex]         
            #calculate travel time based on vehicle speed, then normalize the travel time
            time = dist/self.speed
            time = time/self.normFactor
            travelTimes.append(time)
            headings.append(heading)

        travelTimes = np.array(travelTimes)
        headings = np.array(headings)
        
        return travelTimes, headings

    def calctrajectory(self, selectedTask, taskVector, headings):
       
        #current location and heading
        x0 = taskVector[int(self.location[0]-1)].location[0]
        y0 = taskVector[int(self.location[0]-1)].location[1]
        theta0 = self.heading
        
        #destination and arrival heading
        x1 = taskVector[selectedTask-1].location[0]
        y1 = taskVector[selectedTask-1].location[1]
        theta1 = headings[selectedTask-1]
        
        #calc dubins trajectory to destination
        trajectory, _ = dubins.path_sample((x0, y0, theta0), (x1, y1, theta1), self.turnRadius, 20)

        return trajectory


class VehicleMD2WRP(Vehicle):

    """A class for PISR vehicles using MD2WRP for task selection."""

    def __init__(self, _indexer, ID, initLoc, initHead, speed, bankAngle, numVehicles, beta, w):
        Vehicle.__init__(self, _indexer, ID, initLoc, initHead, speed, bankAngle, numVehicles)
        # self.ageTracker = np.zeros(len(w))      #Start with zero age vector
        self.ageTracker = np.array([ 0, 0.711, 0.806, 0.468, 2.359, 2.53, 2.013, 1.019, 1.708, 1.365])  #start with "periodic" age state
        # self.ageTracker = np.random.rand(1,10)    #TEST non-zero AGE VECTOR
        # self.ageTracker = self.ageTracker[0]
        # self.ageTracker[0] = 0
        self.beta = beta
        self.w = w

    def selecttask(self, travelTimesMeasure):
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
        selectedTask = int(selectedTask[0])
        print '   Task {} has the highest utility. ({})'.format(selectedTask, maxUtility)          

        return selectedTask      

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