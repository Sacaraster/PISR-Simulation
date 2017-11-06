import numpy as np
from abc import ABCMeta, abstractmethod

class Routing(object):

    """A class for routing of PISR Vehicles."""

    __metaclass__ = ABCMeta 

    @abstractmethod
    def get_next_task(self):
        raise NotImplementedError("You must implement a get_next_task method for this routing type!")

class MD2WRP_Routing(Routing):

    def __init__(self, beta, w):        
        self.beta = beta
        self.w = w       

    def get_next_task(self):        
        return "An MD2WRP-selected Task"

class Manual_Routing(Routing):

    def __init__(self, seq_vector):
        self.current_stop = 0
        self.sequence_vector = seq_vector

    def get_next_task(self):
        return "Next task in the sequence"

class RoutingFactory:
    def get_routing_module(self, routing_data):
        if routing_data[0] == 'MD2WRP':            
            return MD2WRP_Routing(routing_data[1], routing_data[2])
        elif routing_data[0] == 'Manual':
            return Manual_Routing(routing_data[1])
        else:
            raise NotImplementedError("Unknown routing type.")
