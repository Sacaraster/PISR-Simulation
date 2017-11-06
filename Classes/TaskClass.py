class Task:

    """A class for PISR tasks"""

    def __init__(self, ID, x, y, pri, init_age, t_activate, t_terminate):
        self.ID = ID
        self.location = [x,y]
        self.priority = pri
        self.age = init_age
        self.t_activate = t_activate
        self.t_terminate = t_terminate
        

    