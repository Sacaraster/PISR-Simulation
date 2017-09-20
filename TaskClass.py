class Task:

    """A class for PISR tasks"""

    def __init__(self, ID, x, y, pri):
        self.ID = ID
        self.location = [x,y]
        self.priority = pri