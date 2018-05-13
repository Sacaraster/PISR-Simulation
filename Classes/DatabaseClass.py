import numpy as np

class Database:

    """A class for different information needs of PISR Vehicles."""

    def __init__(self, database_items, vehicle_vector, task_vector):
        for entry in database_items:
            if entry == 'Age_Tracker':
                # age_vector = []
                # for task in task_vector:
                #     age_vector.append(task.age)
                # age_vector = np.array(age_vector)
                self.age_tracker = np.zeros(len(task_vector))
                for task in task_vector:
                    self.age_tracker[task.ID-1] = task.age
            elif entry == 'Vehicle_Tracker':
                self.vehicle_tracker = np.zeros((len(vehicle_vector), 2))  # format: [destination_task, arrival_time]


