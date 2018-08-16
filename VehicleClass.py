from __future__ import division
import random
import itertools
import math
import numpy as np
import dubins

from RoutingClass import RoutingFactory
from PathingClass import PathingFactory
from CommunicationClass import CommunicationFactory
from DatabaseClass import Database

class Vehicle:

    """A class for PISR vehicles."""

    #The base vehicle class holds attributes of the physical vehicle only (e.g. speed and heading).
    #Vehicles implement "modules" that perform other functions. For example, every vehicle loads a specific
    #type of "Pathing" module, which is defined by the Pathing class. So a vehicle that flys Euclidean
    #paths implements the Euclidean sublcass of the Pathing module.
    def __init__(self, _indexer, ID, init_location, init_heading, speed, turn_radius, t_activate, t_terminate):              

        self._indexer = _indexer   #since IDs are usually 100, 200, etc, this makes referencing vehicles easier
        self.ID = ID
        self.location = init_location   #a task object (vehicle is located at a task)
        self.time = t_activate               #current time for vehicle    
        self.heading = init_heading
        self.speed = speed  
        self.turn_radius = turn_radius
        self.t_activate = t_activate
        self.t_terminate = t_terminate

    #Add the "Routing" module to the vehicle. This determines how the vehicle selects tasks.
    def add_routing(self, routing_data, task_vector):
        routing_factory = RoutingFactory()
        self.routing = routing_factory.get_routing_module(routing_data, self, task_vector)    

    #Add the "Pathing" module to the vehicle. This determines how the vehicle travels between tasks.
    def add_pathing(self, pathing_data):
        pathing_factory = PathingFactory()
        self.pathing = pathing_factory.get_pathing_module(pathing_data) 

    #Add the "Communication" module to the vehicle. This lets vehicles communicate.
    def add_comm(self, comm_data):
        comm_factory = CommunicationFactory()
        self.comm = comm_factory.get_comm_module(comm_data) 

    #Store task and sister vehicle information based on the vehicle's type of "Database" module
    def add_database(self, database_items, vehicle_vector, task_vector):
        self.database = Database(database_items, vehicle_vector, task_vector)
        