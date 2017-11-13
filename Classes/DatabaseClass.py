import numpy as np

class Database:

    """A class for different information needs of PISR Vehicles."""

    def __init__(self, database_items, vehicle_vector, task_vector):
        for entry in database_items:
            if entry == 'Age_Tracker':
                self.age_tracker = np.zeros(len(task_vector))
            elif entry == 'Vehicle_Tracker':
                self.vehicle_tracker = np.zeros((len(vehicle_vector), 2))  # format: [destination_task, arrival_time]

