import numpy as np
from abc import ABCMeta, abstractmethod

class Pathing(object):

    """A class for pathing of PISR Vehicles."""

    __metaclass__ = ABCMeta 

    @abstractmethod
    def get_path(self):
        raise NotImplementedError("You must implement a get_path method for every Pathing type!")

class Euclidean_Pathing(Pathing):

    def __init__(self, task_vector):        
        self.task_vector = task_vector  

    def get_path(self):
        #Will need to pass in a vehicle object when called, for current location        
        return "Straight line distance between current location and all other Tasks."

class Dubins_Pathing(Pathing):

    def __init__(self, task_vector):
        self.task_vector = task_vector

    def get_path(self):
        #Will need to pass in a vehicle object when called, for current location    
        return "Dubins path from current location to all Tasks."

class Tripath_Pathing(Pathing):

    def __init__(self, task_vector):
        self.task_vector = task_vector

    def get_path(self):
        #Will need to pass in a vehicle object when called, for current location   
        return "Call the Tripath program to return locally optimal Euclidean distance to all tasks, subject to path constraints."

class PathingFactory:
    def get_pathing_module(self, pathing_data):
        if pathing_data[0] == 'Euclidean':            
            return Euclidean_Pathing(pathing_data[1])
        elif pathing_data[0] == 'Dubins':
            return Dubins_Pathing(pathing_data[1])
        elif pathing_data[0] == 'Tripath':
            return Tripath_Pathing(pathing_data[1])
        else:
            raise NotImplementedError("Unknown pathing type.")
