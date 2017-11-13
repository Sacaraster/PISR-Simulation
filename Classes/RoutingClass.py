import math
import numpy as np
from abc import ABCMeta, abstractmethod

import PathingClass

class Routing(object):

    """A class for routing of PISR Vehicles."""

    __metaclass__ = ABCMeta 

    @abstractmethod
    def get_next_task(self):
        raise NotImplementedError("You must implement a get_next_task method for this routing type!")

class MD2WRP_Routing(Routing):

    def __init__(self, vehicle, task_vector, beta, w, distance_measure):        
        self.destination = vehicle.location   # destination task (a task object)
        self.arrival_time = vehicle.time  
        self.beta = beta
        self.w = w
        #generate a pathing object for calculating longest distance between tasks
        pathing_object = PathingClass.Euclidean_Pathing()
        avgDistance, longestDistance = pathing_object.calcDistanceMatrixData(task_vector)
        self.norm_factor = longestDistance/vehicle.speed
        if distance_measure == 1:
            self.distance_measure = ['Euclidean']
        if distance_measure == 2:
            self.distance_measure = ['Dubins']
        if distance_measure == 3:
            self.distance_measure = ['Tripath']


    def get_next_task(self, vehicle, task_vector):        

        #Calculate travel times to every task based on the type of distance measurement
        #  (This is not necessarily the physical pathing of the vehicle)
        pathing_factory = PathingClass.PathingFactory()
        pathing_object = pathing_factory.get_pathing_module(vehicle.routing.distance_measure)
        measured_times_and_headings = pathing_object.get_best_paths(vehicle, task_vector)
        measured_times = measured_times_and_headings[:,1]

        print '      Shortest measured times to each task:'
        print ''
        print measured_times_and_headings[:,0:2]
        print ''

        utilities = []               
        for index, tij in enumerate(measured_times):
            age_modifier = 0   #used to adjust the age of a task due to visits from other vehicles
            for other_arrival_index, other_arrival in enumerate(vehicle.database.vehicle_tracker[:, 0]):
                if ((other_arrival == index+1) & (other_arrival_index != vehicle._indexer)):                    
                    #If another vehicle will arrive before me (or at the same time), reduce the age of the task under consideration
                    if vehicle.database.vehicle_tracker[other_arrival_index, 1] <= (vehicle.database.age_tracker[index]+tij):                        
                        age_modifier = vehicle.database.vehicle_tracker[other_arrival_index, 1]
                        print '      *** Task {} age reduced! (Other visit)'.format(index+1)
                    #If another vehicle will be arriving after me, don't go to that task
                    ### UNLESS!!!...that vehicle has been terminated. Then you can go there...CODE THIS UP
                    if vehicle.database.vehicle_tracker[other_arrival_index, 1] > (vehicle.database.age_tracker[index]+tij):                        
                        age_modifier = (vehicle.database.age_tracker[index]+tij)
                        print '      *** Task {} utility set to zero! (Conflict)'.format(index+1)

            utility = (math.exp(-vehicle.routing.beta*(tij/vehicle.routing.norm_factor))*vehicle.routing.w[index]*
                ((vehicle.database.age_tracker[index]+tij-age_modifier)/vehicle.routing.norm_factor))*100000
            utilities.append(utility)
        for index, utility in enumerate(utilities):
            if vehicle.time < task_vector[index].t_activate:
                utilities[index] = 0
                print '      *** Task {} utility set to zero! (Not yet active)'.format(index+1)
            if vehicle.time >= task_vector[index].t_terminate:
                utilities[index] = 0
                print '      *** Task {} utility set to zero! (Terminated)'.format(index+1)             

        print '      Calculated utilities for each task:'
        for index, task_utility in enumerate(utilities):
            print '         Task {} utility = {}'.format(index+1, task_utility)
        max_utility = max(utilities)  

        selected_task = [index for index, utility in enumerate(utilities) if (utility == max_utility)]  #ID of selected task
        selected_task = task_vector[selected_task[0]]   #the actual task object
        print ''
        print '      Task {} has the highest utility. ({})'.format(selected_task.ID, max_utility)          

        vehicle.routing.destination = selected_task    

class Manual_Routing(Routing):

    def __init__(self, vehicle, seq_vector):
        self.current_stop = 0
        self.destination = [vehicle.location[0], 0.0]   # [destination_task, arrival_time]  
        self.sequence_vector = seq_vector

    def get_next_task(self):
        print "Next task in the sequence,", self.sequence_vector

class RoutingFactory:
    def get_routing_module(self, routing_data, vehicle, task_vector):
        if routing_data[0] == 'MD2WRP':            
            return MD2WRP_Routing(vehicle, task_vector, routing_data[1], routing_data[2], routing_data[3])
        elif routing_data[0] == 'Manual':
            return Manual_Routing(vehicle, routing_data[1])
        else:
            raise NotImplementedError("Unknown routing type.")
