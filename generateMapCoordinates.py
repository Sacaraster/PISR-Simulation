import math

def generateMapCoordinates(taskGeometry):

    #2 tasks 1000m apart
    if taskGeometry == 'twopt_1000':
        xTaskCoords = [0, 1000]
        yTaskCoords = [0,    0]

    #2 tasks 5000m apart
    if taskGeometry == 'twopt_5000':
        xTaskCoords = [0, 5000]
        yTaskCoords = [0,    0]

    #3 tasks in an equilateral triangle
    if taskGeometry == 'eqtri':
        xTaskCoords = [0,  250, 125]
        yTaskCoords = [0,    0, 216.5]

    #3 tasks in an isosceles triangle
    if taskGeometry == 'isotri':
        xTaskCoords = [0, 250, 125]
        yTaskCoords = [0,   0, 341.5]

    #10 tasks placed in a circle
    if taskGeometry == 'circle':        
        numTasks = 10
        xTaskCoords = []
        yTaskCoords = []
        radius = 5000
        theta = (2*math.pi)/numTasks     
        for task in xrange(1,numTasks+1,1):
            xTaskCoords.append(radius*math.cos(theta*task))
            yTaskCoords.append(radius*math.sin(theta*task))

    #16 tasks in a 4x4 grid
    if taskGeometry == 'grid':
        xTaskCoords = [0,    0,    0,    0, 1000, 1000, 1000, 1000, 2000, 2000, 2000, 2000, 3000, 3000, 3000, 3000]
        yTaskCoords = [3000, 2000, 1000, 0, 3000, 2000, 1000, 0   , 3000, 2000, 1000, 0   , 3000, 2000, 1000, 0]

    #10 "randomly" placed tasks (wide-area surveillance)
    if taskGeometry == 'random':        
        xTaskCoords = [-4500, -2000, -1000, -2500,  2000,  1000, 3000, 1000, 5000, 2000]
        yTaskCoords = [-1000,  1000,  1000,  3500, -2500, -1000, 1000, 2000, 3500, 5500]

    
    #3 clusters of tasks (10 total)
    if taskGeometry == 'clusters':
        xTaskCoords = [-2777, -1888, -2550, 2500, 3916, 2756,  1850,  2882,  2518,  3528]
        yTaskCoords = [2961,   2114,  927,  2500, 1505, 3552, -2800, -1915, -3833, -3274]


    return xTaskCoords, yTaskCoords