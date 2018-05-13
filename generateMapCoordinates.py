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

    #3 tasks in a slightly perturbed equilateral triangle
    if taskGeometry == 'eqtri_perturb':
        xTaskCoords = [0,  255, 125]
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

    #3 tasks placed in a circle, with 1 in the center
    if taskGeometry == 'circle_rtb_3':        
        numTasks = 4
        xTaskCoords = []
        yTaskCoords = []
        radius = 5000
        theta = (2*math.pi)/(numTasks-1) 
        xTaskCoords.append(0)   #put the base in the center of the circle
        yTaskCoords.append(0)    
        for task in xrange(2,numTasks+1,1):
            xTaskCoords.append(radius*math.cos(theta*task))  #evenly space rest of the tasks around the circle
            yTaskCoords.append(radius*math.sin(theta*task))

    #5 tasks placed in a circle, with 1 in the center
    if taskGeometry == 'circle_rtb_5':        
        numTasks = 6
        xTaskCoords = []
        yTaskCoords = []
        radius = 5000
        theta = (2*math.pi)/(numTasks-1) 
        xTaskCoords.append(0)   #put the base in the center of the circle
        yTaskCoords.append(0)    
        for task in xrange(2,numTasks+1,1):
            xTaskCoords.append(radius*math.cos(theta*task))  #evenly space rest of the tasks around the circle
            yTaskCoords.append(radius*math.sin(theta*task))

    #6 tasks placed in a circle, with 1 in the center
    if taskGeometry == 'circle_rtb_6':        
        numTasks = 7
        xTaskCoords = []
        yTaskCoords = []
        radius = 5000
        theta = (2*math.pi)/(numTasks-1) 
        xTaskCoords.append(0)   #put the base in the center of the circle
        yTaskCoords.append(0)    
        for task in xrange(2,numTasks+1,1):
            xTaskCoords.append(radius*math.cos(theta*task))  #evenly space rest of the tasks around the circle
            yTaskCoords.append(radius*math.sin(theta*task))

    #7 tasks placed in a circle, with 1 in the center
    if taskGeometry == 'circle_rtb_7':        
        numTasks = 8
        xTaskCoords = []
        yTaskCoords = []
        radius = 5000
        theta = (2*math.pi)/(numTasks-1) 
        xTaskCoords.append(0)   #put the base in the center of the circle
        yTaskCoords.append(0)    
        for task in xrange(2,numTasks+1,1):
            xTaskCoords.append(radius*math.cos(theta*task))  #evenly space rest of the tasks around the circle
            yTaskCoords.append(radius*math.sin(theta*task))

    #8 tasks placed in a circle, with 1 in the center
    if taskGeometry == 'circle_rtb_8':        
        numTasks = 9
        xTaskCoords = []
        yTaskCoords = []
        radius = 5000
        theta = (2*math.pi)/(numTasks-1) 
        xTaskCoords.append(0)   #put the base in the center of the circle
        yTaskCoords.append(0)    
        for task in xrange(2,numTasks+1,1):
            xTaskCoords.append(radius*math.cos(theta*task))  #evenly space rest of the tasks around the circle
            yTaskCoords.append(radius*math.sin(theta*task))

    #9 tasks placed in a circle, with 1 in the center
    if taskGeometry == 'circle_rtb_9':        
        numTasks = 10
        xTaskCoords = []
        yTaskCoords = []
        radius = 5000
        theta = (2*math.pi)/(numTasks-1) 
        xTaskCoords.append(0)   #put the base in the center of the circle
        yTaskCoords.append(0)    
        for task in xrange(2,numTasks+1,1):
            xTaskCoords.append(radius*math.cos(theta*task))  #evenly space rest of the tasks around the circle
            yTaskCoords.append(radius*math.sin(theta*task))

    #10 tasks placed in a circle, with 1 in the center (also functions as centroid offset = 0)
    if taskGeometry == 'circle_rtb_10':        
        numTasks = 11
        xTaskCoords = []
        yTaskCoords = []
        radius = 5000
        theta = (2*math.pi)/(numTasks-1) 
        xTaskCoords.append(0)   #put the base in the center of the circle
        yTaskCoords.append(0)    
        for task in xrange(2,numTasks+1,1):
            xTaskCoords.append(radius*math.cos(theta*task))  #evenly space rest of the tasks around the circle
            yTaskCoords.append(radius*math.sin(theta*task))

    #11 tasks placed in a circle, with 1 in the center
    if taskGeometry == 'circle_rtb_11':        
        numTasks = 12
        xTaskCoords = []
        yTaskCoords = []
        radius = 5000
        theta = (2*math.pi)/(numTasks-1) 
        xTaskCoords.append(0)   #put the base in the center of the circle
        yTaskCoords.append(0)    
        for task in xrange(2,numTasks+1,1):
            xTaskCoords.append(radius*math.cos(theta*task))  #evenly space rest of the tasks around the circle
            yTaskCoords.append(radius*math.sin(theta*task))

    #12 tasks placed in a circle, with 1 in the center
    if taskGeometry == 'circle_rtb_12':        
        numTasks = 13
        xTaskCoords = []
        yTaskCoords = []
        radius = 5000
        theta = (2*math.pi)/(numTasks-1) 
        xTaskCoords.append(0)   #put the base in the center of the circle
        yTaskCoords.append(0)    
        for task in xrange(2,numTasks+1,1):
            xTaskCoords.append(radius*math.cos(theta*task))  #evenly space rest of the tasks around the circle
            yTaskCoords.append(radius*math.sin(theta*task))
         
    #10 tasks placed in a circle, with the base offset from the centroid by 10% of d/d_max (task 1 is the base)
    if taskGeometry == 'circle_rtb_10_offset10':        
        numTasks = 11
        xTaskCoords = []
        yTaskCoords = []
        radius = 5000
        theta = (2*math.pi)/(numTasks-1) 
        xTaskCoords.append(0)   #the base
        yTaskCoords.append(500)    
        for task in xrange(2,numTasks+1,1):
            xTaskCoords.append(radius*math.cos(theta*task))  #evenly space rest of the tasks around the circle
            yTaskCoords.append(radius*math.sin(theta*task))

    #10 tasks placed in a circle, with the base offset from the centroid by 20% of d/d_max (task 1 is the base)
    if taskGeometry == 'circle_rtb_10_offset20':        
        numTasks = 11
        xTaskCoords = []
        yTaskCoords = []
        radius = 5000
        theta = (2*math.pi)/(numTasks-1) 
        xTaskCoords.append(0)   #the base
        yTaskCoords.append(1000)    
        for task in xrange(2,numTasks+1,1):
            xTaskCoords.append(radius*math.cos(theta*task))  #evenly space rest of the tasks around the circle
            yTaskCoords.append(radius*math.sin(theta*task))

    #10 tasks placed in a circle, with the base offset from the centroid by 30% of d/d_max (task 1 is the base)
    if taskGeometry == 'circle_rtb_10_offset30':        
        numTasks = 11
        xTaskCoords = []
        yTaskCoords = []
        radius = 5000
        theta = (2*math.pi)/(numTasks-1) 
        xTaskCoords.append(0)   #the base
        yTaskCoords.append(1500)    
        for task in xrange(2,numTasks+1,1):
            xTaskCoords.append(radius*math.cos(theta*task))  #evenly space rest of the tasks around the circle
            yTaskCoords.append(radius*math.sin(theta*task))

    #10 tasks placed in a circle, with the base offset from the centroid by 40% of d/d_max (task 1 is the base)
    if taskGeometry == 'circle_rtb_10_offset40':        
        numTasks = 11
        xTaskCoords = []
        yTaskCoords = []
        radius = 5000
        theta = (2*math.pi)/(numTasks-1) 
        xTaskCoords.append(0)   #the base
        yTaskCoords.append(2000)    
        for task in xrange(2,numTasks+1,1):
            xTaskCoords.append(radius*math.cos(theta*task))  #evenly space rest of the tasks around the circle
            yTaskCoords.append(radius*math.sin(theta*task))

    #10 tasks placed in a circle, with the base offset from the centroid by 50% of d/d_max (task 1 is the base)
    if taskGeometry == 'circle_rtb_10_offset50':        
        numTasks = 11
        xTaskCoords = []
        yTaskCoords = []
        radius = 5000
        theta = (2*math.pi)/(numTasks-1) 
        xTaskCoords.append(0)   #the base
        yTaskCoords.append(2500)    
        for task in xrange(2,numTasks+1,1):
            xTaskCoords.append(radius*math.cos(theta*task))  #evenly space rest of the tasks around the circle
            yTaskCoords.append(radius*math.sin(theta*task))

    #10 tasks placed in a circle, with the base offset from the centroid by 60% of d/d_max (task 1 is the base)
    if taskGeometry == 'circle_rtb_10_offset60':        
        numTasks = 11
        xTaskCoords = []
        yTaskCoords = []
        radius = 5000
        theta = (2*math.pi)/(numTasks-1) 
        xTaskCoords.append(0)   #the base
        yTaskCoords.append(3000)    
        for task in xrange(2,numTasks+1,1):
            xTaskCoords.append(radius*math.cos(theta*task))  #evenly space rest of the tasks around the circle
            yTaskCoords.append(radius*math.sin(theta*task))

    #10 tasks placed in a circle, with the base offset from the centroid by 70% of d/d_max (task 1 is the base)
    if taskGeometry == 'circle_rtb_10_offset70':        
        numTasks = 11
        xTaskCoords = []
        yTaskCoords = []
        radius = 5000
        theta = (2*math.pi)/(numTasks-1) 
        xTaskCoords.append(0)   #the base
        yTaskCoords.append(3500)    
        for task in xrange(2,numTasks+1,1):
            xTaskCoords.append(radius*math.cos(theta*task))  #evenly space rest of the tasks around the circle
            yTaskCoords.append(radius*math.sin(theta*task))

    #10 tasks placed in a circle, with the base offset from the centroid by 80% of d/d_max (task 1 is the base)
    if taskGeometry == 'circle_rtb_10_offset80':        
        numTasks = 11
        xTaskCoords = []
        yTaskCoords = []
        radius = 5000
        theta = (2*math.pi)/(numTasks-1) 
        xTaskCoords.append(0)   #the base
        yTaskCoords.append(4000)    
        for task in xrange(2,numTasks+1,1):
            xTaskCoords.append(radius*math.cos(theta*task))  #evenly space rest of the tasks around the circle
            yTaskCoords.append(radius*math.sin(theta*task))

    #10 tasks placed in a circle, with the base offset from the centroid by 90% of d/d_max (task 1 is the base)
    if taskGeometry == 'circle_rtb_10_offset90':        
        numTasks = 11
        xTaskCoords = []
        yTaskCoords = []
        radius = 5000
        theta = (2*math.pi)/(numTasks-1) 
        xTaskCoords.append(0)   #the base
        yTaskCoords.append(4500)    
        for task in xrange(2,numTasks+1,1):
            xTaskCoords.append(radius*math.cos(theta*task))  #evenly space rest of the tasks around the circle
            yTaskCoords.append(radius*math.sin(theta*task))

    #10 tasks placed in a circle, with the base offset from the centroid by 100% of d/d_max (task 1 is the base)
    if taskGeometry == 'circle_rtb_10_offset100':        
        numTasks = 11
        xTaskCoords = []
        yTaskCoords = []
        radius = 5000
        theta = (2*math.pi)/(numTasks-1) 
        xTaskCoords.append(0)   #the base
        yTaskCoords.append(5000)    
        for task in xrange(2,numTasks+1,1):
            xTaskCoords.append(radius*math.cos(theta*task))  #evenly space rest of the tasks around the circle
            yTaskCoords.append(radius*math.sin(theta*task))

    #16 tasks in a 4x4 grid
    if taskGeometry == 'grid':
        xTaskCoords = [0,    0,    0,    0, 1000, 1000, 1000, 1000, 2000, 2000, 2000, 2000, 3000, 3000, 3000, 3000]
        yTaskCoords = [3000, 2000, 1000, 0, 3000, 2000, 1000, 0   , 3000, 2000, 1000, 0   , 3000, 2000, 1000, 0]

    #10 "randomly" placed tasks (wide-area surveillance)
    if taskGeometry == 'random':        
        xTaskCoords = [-4500, -2000, -1000, -2500,  2000,  1000, 3000, 1000, 5000, 2000]
        yTaskCoords = [-1000,  1000,  1000,  3500, -2500, -1000, 1000, 2000, 3500, 5500]

    #10 "randomly" placed tasks plus a task at the centroid (the first task)
    if taskGeometry == 'random_centroid_0':        
        xTaskCoords = [400,  -4500, -2000, -1000, -2500,  2000,  1000, 3000, 1000, 5000, 2000]
        yTaskCoords = [1300, -1000,  1000,  1000,  3500, -2500, -1000, 1000, 2000, 3500, 5500]

    #10 "randomly" placed tasks plus a task offset from the centroid by a distance equal to 10% of the furthest point from the centroid
    #(the first task)
    if taskGeometry == 'random_centroid_10':        
        xTaskCoords = [400,  -4500, -2000, -1000, -2500,  2000,  1000, 3000, 1000, 5000, 2000]
        yTaskCoords = [1300+541, -1000,  1000,  1000,  3500, -2500, -1000, 1000, 2000, 3500, 5500]

    #10 "randomly" placed tasks plus a task offset from the centroid by a distance equal to 20% of the furthest point from the centroid
    #(the first task)
    if taskGeometry == 'random_centroid_20':        
        xTaskCoords = [400,  -4500, -2000, -1000, -2500,  2000,  1000, 3000, 1000, 5000, 2000]
        yTaskCoords = [1300+1083, -1000,  1000,  1000,  3500, -2500, -1000, 1000, 2000, 3500, 5500]

    #10 "randomly" placed tasks plus a task offset from the centroid by a distance equal to 30% of the furthest point from the centroid
    #(the first task)
    if taskGeometry == 'random_centroid_30':        
        xTaskCoords = [400,  -4500, -2000, -1000, -2500,  2000,  1000, 3000, 1000, 5000, 2000]
        yTaskCoords = [1300+1624, -1000,  1000,  1000,  3500, -2500, -1000, 1000, 2000, 3500, 5500]

    #10 "randomly" placed tasks plus a task offset from the centroid by a distance equal to 40% of the furthest point from the centroid
    #(the first task)
    if taskGeometry == 'random_centroid_40':        
        xTaskCoords = [400,  -4500, -2000, -1000, -2500,  2000,  1000, 3000, 1000, 5000, 2000]
        yTaskCoords = [1300+2165, -1000,  1000,  1000,  3500, -2500, -1000, 1000, 2000, 3500, 5500]

    #10 "randomly" placed tasks plus a task offset from the centroid by a distance equal to 50% of the furthest point from the centroid
    #(the first task)
    if taskGeometry == 'random_centroid_50':        
        xTaskCoords = [400,  -4500, -2000, -1000, -2500,  2000,  1000, 3000, 1000, 5000, 2000]
        yTaskCoords = [1300+2707, -1000,  1000,  1000,  3500, -2500, -1000, 1000, 2000, 3500, 5500]

    #10 "randomly" placed tasks plus a task offset from the centroid by a distance equal to 60% of the furthest point from the centroid
    #(the first task)
    if taskGeometry == 'random_centroid_60':        
        xTaskCoords = [400,  -4500, -2000, -1000, -2500,  2000,  1000, 3000, 1000, 5000, 2000]
        yTaskCoords = [1300+3248, -1000,  1000,  1000,  3500, -2500, -1000, 1000, 2000, 3500, 5500]

    #10 "randomly" placed tasks plus a task offset from the centroid by a distance equal to 70% of the furthest point from the centroid
    #(the first task)
    if taskGeometry == 'random_centroid_70':        
        xTaskCoords = [400,  -4500, -2000, -1000, -2500,  2000,  1000, 3000, 1000, 5000, 2000]
        yTaskCoords = [1300+3789, -1000,  1000,  1000,  3500, -2500, -1000, 1000, 2000, 3500, 5500]

    #10 "randomly" placed tasks plus a task offset from the centroid by a distance equal to 80% of the furthest point from the centroid
    #(the first task)
    if taskGeometry == 'random_centroid_80':        
        xTaskCoords = [400,  -4500, -2000, -1000, -2500,  2000,  1000, 3000, 1000, 5000, 2000]
        yTaskCoords = [1300+4330, -1000,  1000,  1000,  3500, -2500, -1000, 1000, 2000, 3500, 5500]

    #10 "randomly" placed tasks plus a task offset from the centroid by a distance equal to 90% of the furthest point from the centroid
    #(the first task)
    if taskGeometry == 'random_centroid_90':        
        xTaskCoords = [400,  -4500, -2000, -1000, -2500,  2000,  1000, 3000, 1000, 5000, 2000]
        yTaskCoords = [1300+4871, -1000,  1000,  1000,  3500, -2500, -1000, 1000, 2000, 3500, 5500]

    #10 "randomly" placed tasks plus a task offset from the centroid by a distance equal to the furthest point from the centroid
    #(the first task)
    if taskGeometry == 'random_centroid_100':        
        xTaskCoords = [400,  -4500, -2000, -1000, -2500,  2000,  1000, 3000, 1000, 5000, 2000]
        yTaskCoords = [1300+5413, -1000,  1000,  1000,  3500, -2500, -1000, 1000, 2000, 3500, 5500]
    
    #3 clusters of tasks (10 total)
    if taskGeometry == 'clusters':
        xTaskCoords = [-2777, -1888, -2550, 2500, 3916, 2756,  1850,  2882,  2518,  3528]
        yTaskCoords = [2961,   2114,  927,  2500, 1505, 3552, -2800, -1915, -3833, -3274]


    return xTaskCoords, yTaskCoords