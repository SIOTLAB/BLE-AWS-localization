import numpy as np 
import pandas as pd 
import matplotlib.pyplot as plt 
from pandas import DataFrame 
from sklearn import datasets  
from sklearn import mixture
import seaborn
from numpy import genfromtxt
from google.colab import files
drive.mount('/content/drive')

my_data = genfromtxt('/content/drive/My Drive/Data/CleanData.txt', delimiter=',')

fig = plt.figure(figsize=(9, 10))
plt.rcParams.update({'font.size': 14})
ax1 = fig.add_subplot(211)   
ax2 = fig.add_subplot(212)


my_data = np.delete(my_data, (0), axis=0)
d = pd.DataFrame(my_data) 
ax1.scatter(d[0], d[1], facecolors='none', edgecolors='b', label="Postion of user") 

ax1.set_xlabel("X coordinate [m]")
ax1.set_ylabel("Y coordinate [m]")
ax1.set_title("(a)", y=-0.2)
plt.legend(loc = "upper left", prop={'size': 15})

gmm = mixture.GaussianMixture(n_components = 3).fit(d) 
labels = gmm.predict(d) 
d['labels']= labels 
d0 = d[d['labels']== 0] 
d1 = d[d['labels']== 1] 
d2 = d[d['labels']== 2] 

seaborn.set_style(style='white')

ax2.scatter(d0[0], d0[1], c ='r', edgecolors='black', label="Sitting area", marker='+') 
ax2.scatter(d1[0], d1[1], c ='g', edgecolors='black', label="Lecture Hall", marker='x') 
ax2.scatter(d2[0], d2[1], label="Corridor", facecolors='none', edgecolors='b') 


ax2.set_xlabel("X coordinate [m]")
ax2.set_ylabel("Y coordinate [m]")
ax2.set_title("(b)", y=-0.2)
ax2.legend(loc = "upper left", prop={'size': 15})

plt.savefig('ClusteredData.pdf')
files.download("ClusteredData.pdf") 
plt.show()