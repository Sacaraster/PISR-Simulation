import numpy as np

class Database:

    """A class for different information needs of PISR Vehicles."""

    def __init__(self, database_items):
        for entry in database_items:
            if entry[0] == 'Age_Tracker':
                self.age_tracker = np.ones(entry[1])
            if entry[0] == 'Vehicle_Tracker':
                self.vehicle_tracker = np.zeros((entry[1], 3))

    def task_completed(self):        
        self.age_tracker[0] = 0
        print "Updated age of completed task."

