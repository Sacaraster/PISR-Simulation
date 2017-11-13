import math
import dubins
import numpy as np
from abc import ABCMeta, abstractmethod

class Pathing(object):

    """A class for pathing of PISR Vehicles."""

    __metaclass__ = ABCMeta 

    @abstractmethod
    def get_path(self):
        raise NotImplementedError("You must implement a get_path method for every Pathing type!")

    @abstractmethod
    def get_best_paths(self):
        raise NotImplementedError("You must implement a get_best_paths method for every Pathing type!")

class Euclidean_Pathing(Pathing):

    def __init__(self):
        self.trajectory = [] 

    def get_path(self):
        #Will need to pass in a vehicle object when called, for current location        
        return "Straight line distance between current location and target task"
        # def calctraveleuc(self, taskVector):
#         travelTimes = []
#         for index, task in enumerate(taskVector):
#             dist = math.sqrt(math.pow(task.location[0]-taskVector[int(self.location[0])-1].location[0], 2)+
#                 math.pow(task.location[1]-taskVector[int(self.location[0])-1].location[1], 2))
#             time = dist/self.speed
#             time = time/self.normFactor
#             travelTimes.append(time)

#         travelTimes = np.array(travelTimes)

#         return travelTimes

    def get_best_paths(self, vehicle, task_vector):
        return "Straight line distance between current location and all other Tasks."

    def calcDistanceMatrixData(self, task_vector):
        cxyVector = []
        for task in task_vector:
            x = task.location[0]
            y = task.location[1]
            cxy = x+y*1j
            cxyVector.append(cxy)
        cxyVector = np.array([cxyVector], dtype=complex)

        distanceMatrix = abs(cxyVector.T-cxyVector)

        longestDistance = np.max(distanceMatrix)
        avgDistance = np.sum(distanceMatrix)/((distanceMatrix.shape[0]**2)-distanceMatrix.shape[0])   #don't divide by diaganol entries, which are zero

        return avgDistance, longestDistance

class Dubins_Pathing(Pathing):

    def __init__(self):
        self.trajectory = []

    def get_path(self, vehicle, arrival_heading):

        x0 = vehicle.location.location[0]
        y0 = vehicle.location.location[1]
        theta0 = vehicle.heading

        x1 = vehicle.routing.destination.location[0]
        y1 = vehicle.routing.destination.location[1]
        theta1 = arrival_heading

        #Calculate length of dubins path
        path_length = dubins.path_length((x0, y0, theta0), (x1, y1, theta1), vehicle.turn_radius)
        #Calculate trajectory to destination
        trajectory, _ = dubins.path_sample((x0, y0, theta0), (x1, y1, theta1), vehicle.turn_radius, 20)

        #update vehicle states
        self.trajectory = trajectory        
        vehicle.routing.arrival_time = vehicle.time + path_length/vehicle.speed
        vehicle.heading = arrival_heading

    def get_best_paths(self, vehicle, task_vector):
        
        times_and_headings = []

        #Coordinates of current location and current heading
        x0 = vehicle.location.location[0]
        y0 = vehicle.location.location[1]
        theta0 = vehicle.heading
        #For every candidate task...        
        for index, task in enumerate(task_vector):            
            path_length_vector = []
            #Coordinates of candidate task
            x1 = task.location[0]
            y1 = task.location[1]
            #Discretized arrival headings (try each of these and pick the one with the shortest travel distance)
            thetas = np.arange(0, 20, 1.25)*(math.pi/10)
            #If arriving at the current task at the current heading, slightly change arrival angle (prevents travel time of zero)
            for theta_index, theta in enumerate(thetas):                
                if (int(vehicle.location.ID-1) == index) & (theta0==theta):
                    thetas[theta_index] = theta0 + 0.0174533   #add 1 degree to arrivalangle
            for theta1 in thetas:
                #Cacluate the path length for given arrival angle
                path_length = dubins.path_length((x0, y0, theta0), (x1, y1, theta1), vehicle.turn_radius)
                path_length_vector.append(path_length)
            #find the shortest travel distance for all arrival heading options
            min_dist = min(path_length_vector)
            min_dist_index = np.argmin(path_length_vector)
            heading = thetas[min_dist_index]         
            #calculate travel time based on vehicle speed
            time = min_dist/vehicle.speed
            times_and_headings.append([task.ID, time, heading])

        times_and_headings = np.array(times_and_headings)

        return times_and_headings


class Tripath_Pathing(Pathing):

    def __init__(self):
        self.trajectory = [] 

    def get_path(self, vehicle, task_vector):
        #Will need to pass in a vehicle object when called, for current location  
        print "Call the Tripath program to return locally optimal paths from current location to destination."

    def get_best_paths(self, vehicle, task_vector):
        print "Call the Tripath program to return locally optimal paths from current location to all other tasks."

class PathingFactory:
    def get_pathing_module(self, pathing_data):
        if pathing_data[0] == 'Euclidean':            
            return Euclidean_Pathing()
        elif pathing_data[0] == 'Dubins':
            return Dubins_Pathing()
        elif pathing_data[0] == 'Tripath':
            return Tripath_Pathing()
        else:
            raise NotImplementedError("Unknown pathing type.")
