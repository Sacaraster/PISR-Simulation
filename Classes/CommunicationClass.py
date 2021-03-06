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
        self.type = 'None'

    def talk(self, decider, vehicle_vector):
        pass

class Completion_Communication(Communication):

    def __init__(self):        
        self.type = 'Completion'

    def talk(self, decider, vehicle_vector):
        for vehicle in vehicle_vector:            
            if vehicle.ID != decider.ID:
                #Update sister vehicle age tracker's to account for the task just serviced by this vehicle
                #(This is how old the task will now be when the sister vehicle makes its next decision)
                vehicle.database.age_tracker[int(decider.location.ID-1)] = vehicle.routing.arrival_time-decider.time
                print ''
                print '      Broadcasted completion of Task {} @ {} secs.'.format(decider.location.ID, decider.time)

class Destination_Communication(Communication):

    def __init__(self):        
        self.type = 'Destination'

    def talk(self, decider, vehicle_vector):
        for vehicle in vehicle_vector:            
            if vehicle.ID != decider.ID:
                #Update sister vehicle age tracker's to account for the task just serviced by this vehicle
                #(This is how old the task will now be when the sister vehicle makes its next decision)
                vehicle.database.age_tracker[int(decider.location.ID-1)] = vehicle.routing.arrival_time-decider.time
                #Let the sister vehicles know which task this vehicle has just selected and what time it will arrive
                vehicle.database.vehicle_tracker[decider._indexer, 0] = decider.routing.destination.ID
                vehicle.database.vehicle_tracker[decider._indexer, 1] = decider.routing.arrival_time
        
        print ''
        print '      Broadcasted completion of Task {} @ {} secs.'.format(decider.location.ID, decider.time)
        print '      Broadcasted destination as Task {} @ {} secs.'.format(decider.routing.destination.ID, decider.routing.arrival_time)

class CommunicationFactory:
    def get_comm_module(self, comm_data):
        if comm_data == 'None':            
            return No_Communication()
        elif comm_data == 'Completion':
            return Completion_Communication()
        elif comm_data == 'Destination':
            return Destination_Communication()
        else:
            raise NotImplementedError("Unknown communication type.")
