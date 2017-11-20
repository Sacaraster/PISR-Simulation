import matplotlib.pyplot as plt
import numpy as np

pathData = np.genfromtxt('path.txt',delimiter = ",")
xPath = pathData[:,0]
yPath = pathData[:,1]

cEdges = np.genfromtxt('cedges.txt',delimiter=",")
xCEdges = cEdges[:,0]
yCEdges = cEdges[:,1]

ucEdges = np.genfromtxt('ucedges.txt',delimiter=",")
xUCEdges = ucEdges[:,0]
yUCEdges = ucEdges[:,1]


plt.plot(xUCEdges,yUCEdges,color='gray')
plt.plot(xCEdges,yCEdges,color='black',linewidth=3.0)
plt.plot(xPath,yPath,color='green',linewidth=3.0)

plt.xlabel('X (units)')
plt.ylabel('Y (units)')
plt.title('Simple Tripath Example')
#plt.grid(True)
plt.savefig("path_test.eps")
plt.show()
