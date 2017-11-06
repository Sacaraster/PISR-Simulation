import numpy as np
from abc import ABCMeta, abstractmethod

class Communication(object):

    """A class for pathing of PISR Vehicles."""

    __metaclass__ = ABCMeta 

    @abstractmethod
    def talk(self):
        raise NotImplementedError("You must implement a talk method for every Communication type!")

class No_Communication(Communication):

    def __init__(self):        
        pass

    def talk(self):
        print "Say Nothing."

class Completion_Communication(Communication):

    def __init__(self):        
        pass

    def talk(self):
        #will need to pas in each vehicles' database information
        print "Tell other vehicle's which Task was just completed."

class Destination_Communication(Communication):

    def __init__(self):        
        pass

    def talk(self):
        #will need to pas in each vehicles' database information (and maybe more)
        print "Tell other vehicle's the destination task (and what was just completed)."

class CommunicationFactory:
    def get_comm_module(self, comm_data):
        if comm_data[0] == 'None':            
            return No_Communication()
        elif comm_data[0] == 'Completion':
            return Completion_Communication()
        elif comm_data[0] == 'Destination':
            return Destination_Communication()
        else:
            raise NotImplementedError("Unknown pathing type.")
