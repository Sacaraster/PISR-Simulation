import os
import subprocess
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

    @abstractmethod
    def print_pathing_data(self):
        raise NotImplementedError("You must implement a print_pathing_data method for every Pathing type!")

class Euclidean_Pathing(Pathing):

    def __init__(self):
        self.type = 'Euclidean'
        self.trajectory = []

    def print_pathing_data(self):
        print '            Type:', self.type

    def get_path(self, vehicle):
        
        x0 = vehicle.location.location[0]
        y0 = vehicle.location.location[1]

        x1 = vehicle.routing.destination.location[0]
        y1 = vehicle.routing.destination.location[1]

        #Calculate length of Euclidean path
        path_length = math.sqrt(math.pow(x1-x0, 2)+math.pow(y1-y0, 2))
        #Calculate trajectory
        trajectory = np.array([[x0, y0], [x1, y1]])

        #update vehicle states
        self.trajectory = trajectory        
        vehicle.routing.arrival_time = vehicle.time + path_length/vehicle.speed
        vehicle.heading = 0

        print '      Travel time to Task {} = {}'.format(vehicle.routing.destination.ID, path_length/vehicle.speed)
        print '      Arriving @ {} secs.'.format(np.around(vehicle.routing.arrival_time, 3))
        print '      Arrival heading: {} degrees'.format(np.around(vehicle.heading*(180/math.pi),1))

    def get_best_paths(self, vehicle, task_vector):

        times_and_headings = []

        x0 = vehicle.location.location[0]
        y0 = vehicle.location.location[1]

        #For every candidate task...        
        for index, task in enumerate(task_vector): 
            
            #Coordinates of candidate task
            x1 = task.location[0]
            y1 = task.location[1]            

            #Calculate the distance between the current location and candidate task
            dist = math.sqrt(math.pow(x1-x0, 2)+math.pow(y1-y0, 2))

            #Convert distance to travel time
            time = dist/vehicle.speed
            
            #Save the travel time to each task
            times_and_headings.append([task.ID, time, 0])

        times_and_headings = np.array(times_and_headings)

        print '      Shortest measured times to each task:'
        print ''
        print times_and_headings[:,0:2]
        print ''

        return times_and_headings


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
        self.type = 'Dubins'
        self.trajectory = []

    def print_pathing_data(self):
        print '            Type:', self.type

    def get_path(self, vehicle):

        x0 = vehicle.location.location[0]
        y0 = vehicle.location.location[1]
        theta0 = vehicle.heading

        x1 = vehicle.routing.destination.location[0]
        y1 = vehicle.routing.destination.location[1]

        path_length_vector = []
        #Discretized arrival headings (try each of these and pick the one with the shortest travel distance)
        thetas = np.arange(0, 20, 1.25)*(math.pi/10)
        #If arriving at the current task at the current heading, slightly change arrival angle (prevents travel time of zero)
        for theta_index, theta in enumerate(thetas):                
            if (int(vehicle.location.ID-1) == vehicle.routing.destination.ID) & (theta0==theta):
                thetas[theta_index] = theta0 + 0.0174533   #add 1 degree to arrivalangle
        for theta1 in thetas:
            #Cacluate the path length for given arrival angle
            path_length = dubins.path_length((x0, y0, theta0), (x1, y1, theta1), vehicle.turn_radius)
            path_length_vector.append(path_length)

        #find the shortest travel distance for all calculated arrival heading options
        min_dist = min(path_length_vector)
        min_dist_index = np.argmin(path_length_vector)
        arrival_heading = thetas[min_dist_index]
        
        #Calculate trajectory to destination
        trajectory, _ = dubins.path_sample((x0, y0, theta0), (x1, y1, arrival_heading), vehicle.turn_radius, 20)

        #update vehicle states
        self.trajectory = trajectory        
        vehicle.routing.arrival_time = vehicle.time + min_dist/vehicle.speed
        vehicle.heading = arrival_heading

        print '      Travel time to Task {} = {}'.format(vehicle.routing.destination.ID, min_dist/vehicle.speed)
        print '      Arriving @ {} secs.'.format(np.around(vehicle.routing.arrival_time, 3))
        print '      Arrival heading: {} degrees'.format(np.around(vehicle.heading*(180/math.pi),1))

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

        print '      Shortest measured times to each task:'
        print ''
        print times_and_headings[:,0:2]
        print ''

        return times_and_headings


class Tripath_Pathing(Pathing):

    def __init__(self, task_geometry, nfz):
        self.type = 'Tripath'
        self.map = task_geometry   #tells Tripath which map is in use
        self.nfz = nfz             #tells Tripath which no-fly zone to use (an integer)
        self.trajectory = []        
        self.nfz_impact = 0      #ratio of average travel distance with nfz to w/out nfz
        

    def print_pathing_data(self):
        print '            Type:', self.type
        print '            Map:', self.map
        print '            NFZ:', self.nfz
        print '            NFZ Impact Rating:', self.nfz_impact 

    def get_path(self, vehicle):

        x0 = vehicle.location.location[0]
        y0 = vehicle.location.location[1]

        x1 = vehicle.routing.destination.location[0]
        y1 = vehicle.routing.destination.location[1]

        #Cacluate the path to the task
        FNULL = open(os.devnull, 'w')   #This prevents a terminal window from popping up each time Tripath is called
        subprocess.call('/home/chris/Research/PISR_Sim_NGpp/Tripath_custom/bin/./setut {} {} {} {} {} {}'.format(x0, y0, x1, y1, vehicle.pathing.map, vehicle.pathing.nfz),
            cwd='/home/chris/Research/PISR_Sim_NGpp/Tripath_custom/bin/', stdout=FNULL, shell=True)
        path_data = np.genfromtxt('/home/chris/Research/PISR_Sim_NGpp/Tripath_custom/bin/path.txt', delimiter = ",")  #path_data is the trajectory data
        xPath = path_data[:,0]
        yPath = path_data[:,1]

        #Calculate the length of the path
        path_length = 0
        for ind, entry in enumerate(xPath[0:-1]):
            path_length = path_length + math.sqrt(math.pow(xPath[ind+1]-xPath[ind], 2)+math.pow(yPath[ind+1]-yPath[ind], 2)) 

        #update vehicle states
        self.trajectory = path_data        
        vehicle.routing.arrival_time = vehicle.time + path_length/vehicle.speed
        vehicle.heading = 0

        print '      Travel time to Task {} = {}'.format(vehicle.routing.destination.ID, path_length/vehicle.speed)
        print '      Arriving @ {} secs.'.format(np.around(vehicle.routing.arrival_time, 3))
        print '      Arrival heading: {} degrees'.format(np.around(vehicle.heading*(180/math.pi),1))


    def get_best_paths(self, vehicle, task_vector):

        times_and_headings = []  #note...Euclidean, so heading is always '0'

        #Coordinates of current location
        x0 = vehicle.location.location[0]
        y0 = vehicle.location.location[1]

        #For every candidate task...        
        for index, task in enumerate(task_vector): 
            
            #Coordinates of candidate task
            x1 = task.location[0]
            y1 = task.location[1]

            #Cacluate the path to the task
            FNULL = open(os.devnull, 'w')   #This prevents a terminal window from popping up each time Tripath is called
            subprocess.call('/home/chris/Research/PISR_Sim_NGpp/Tripath_custom/bin/./setut {} {} {} {} {} {}'.format(x0, y0, x1, y1, vehicle.pathing.map, vehicle.pathing.nfz),
                cwd='/home/chris/Research/PISR_Sim_NGpp/Tripath_custom/bin/', stdout=FNULL, shell=True)
            path_data = np.genfromtxt('/home/chris/Research/PISR_Sim_NGpp/Tripath_custom/bin/path.txt', delimiter = ",")
            xPath = path_data[:,0]
            yPath = path_data[:,1]

            #Calculate the length of the path
            dist = 0
            for ind, entry in enumerate(xPath[0:-1]):
                dist = dist + math.sqrt(math.pow(xPath[ind+1]-xPath[ind], 2)+math.pow(yPath[ind+1]-yPath[ind], 2))

            #Convert distance to travel time
            time = dist/vehicle.speed
            
            #Save the travel time to each task
            times_and_headings.append([task.ID, time, 0])

        times_and_headings = np.array(times_and_headings)

        print '      Shortest measured times to each task:'
        print ''
        print times_and_headings[:,0:2]
        print ''

        return times_and_headings

    def calc_nfz_impact_rating(self, pathing_data, task_vector):
        #First, calculate the average distance between all tasks without the NFZ (Euclidean distances)
        cxyVector = []
        for task in task_vector:
            x = task.location[0]
            y = task.location[1]
            cxy = x+y*1j
            cxyVector.append(cxy)
        cxyVector = np.array([cxyVector], dtype=complex)
        distanceMatrix = abs(cxyVector.T-cxyVector)
        D_without_nfz = np.sum(distanceMatrix)/((distanceMatrix.shape[0]**2)-distanceMatrix.shape[0])   #don't divide by diaganol entries, which are zero

        #Second, calculate the average distance between all tasks taking into account the NFZ (Use Tripath)
        D_array = []
        for start_task in task_vector:  #for every task...
            for end_task in task_vector:  #to every task...
                #Coordinates of starting task
                x0 = start_task.location[0]
                y0 = start_task.location[1]

                #Coordinate of destination task
                x1 = end_task.location[0]
                y1 = end_task.location[1]

                #Caclulate distance between start and end task
                FNULL = open(os.devnull, 'w')   #This prevents a terminal window from popping up each time Tripath is called
                # subprocess.call('../../Tripath_custom/bin/./setut {} {} {} {} {} {}'.format(x0, y0, x1, y1, pathing_data[1], pathing_data[2]),
                #     cwd='../../Tripath_custom/bin/', stdout=FNULL, shell=True)
                subprocess.call('/home/chris/Research/PISR_Sim_NGpp/Tripath_custom/bin/./setut {} {} {} {} {} {}'.format(x0, y0, x1, y1, pathing_data[1], pathing_data[2]),
                    cwd='/home/chris/Research/PISR_Sim_NGpp/Tripath_custom/bin/', stdout=FNULL, shell=True)
                path_data = np.genfromtxt('/home/chris/Research/PISR_Sim_NGpp/Tripath_custom/bin/path.txt', delimiter = ",")
                xPath = path_data[:,0]
                yPath = path_data[:,1]
                dist = 0
                for ind, entry in enumerate(xPath[0:-1]):
                    dist = dist + math.sqrt(math.pow(xPath[ind+1]-xPath[ind], 2)+math.pow(yPath[ind+1]-yPath[ind], 2))
                D_array.append(dist)
        D_array = np.array(D_array)        
        D_with_nfz = np.sum(D_array)/(D_array.shape[0]-len(task_vector))   #don't divide by the zero entries of task x to task x
        
        #calculate nfz impact rating and save
        self.nfz_impact = D_with_nfz/D_without_nfz


class PathingFactory:
    def get_pathing_module(self, pathing_data):
        if pathing_data[0] == 'Euclidean':            
            return Euclidean_Pathing()
        elif pathing_data[0] == 'Dubins':
            return Dubins_Pathing()
        elif pathing_data[0] == 'Tripath':            
            return Tripath_Pathing(pathing_data[1], pathing_data[2])
        else:
            raise NotImplementedError("Unknown pathing type.")
