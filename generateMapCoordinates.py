import numpy as np

def generateMapCoordinates(taskGeometry):

    #3 tasks in an equilateral triangle
    if taskGeometry == 'eqtri':
        rawTaskCoords = np.array([[-250.-250.j, 250.-250.j, 0.+183.j]])

    #3 tasks in an isosceles triangle
    if taskGeometry == 'isotri':
        xTaskCoords = [-200, 200, 0]
        yTaskCoords = [0,    0,   552]

    #10 tasks placed in a circle
    if taskGeometry == 'circle':        
        numTasks = 10
        rawTaskCoords = np.empty([1, numTasks], dtype=complex)
        coord = 0+0.j
        radius = 5000
        theta = (math.pi*2)/numTasks     
        for task in range(1,11,1):
            coord = radius*math.cos(theta*task)+radius*math.sin(theta*task)*1j
            # coord.imag = radius*math.sin(theta*task)            
            rawTaskCoords[0, task-1] = coord
            # print rawTaskCoords

    #16 tasks in a 4x4 grid
    if taskGeometry == 'grid':
        rawTaskCoords = np.array([[0+3000.j, 0+2000.j, 0+1000.j, 0+0.j, 1000+3000.j, 1000+2000.j, 1000+1000.j, 1000+0.j,
            2000+3000.j, 2000+2000.j, 2000+1000.j, 2000+0.j, 3000+3000.j, 3000+2000.j, 3000+1000.j, 3000+0.j]])

    #10 "randomly" placed tasks (wide-area surveillance)
    if taskGeometry == 'random':
        rawTaskCoords = np.array([[-4500-1000.j, -2000+1000.j, -1000+1000.j, -2500+3500.j, 2000-2500.j,
            1000-1000.j, 3000+1000.j, 1000+2000.j, 5000+3500.j, 2000+5500.j]])
    #3 clusters of tasks (10 total)
    if taskGeometry == 'clusters':
        # rawTaskCoords = np.array([[-2777+2961.j, -1888+2114.j, -2550+927.j,
        #     2500+2500.j, 3916+1505.j, 2756+3552.j, 1850-2800.j, 2882-1915.j, 2518-3833.j, 3528-3274.j]])
        xTaskCoords = [-2777, -1888, -2550, 2500, 3916, 2756,  1850,  2882,  2518,  3528]
        yTaskCoords = [2961,   2114,  927,  2500, 1505, 3552, -2800, -1915, -3833, -3274]


    return xTaskCoords, yTaskCoords