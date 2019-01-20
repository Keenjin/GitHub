import os
import numpy as np
import matplotlib.pyplot as plt

from sklearn.cluster import KMeans
from sklearn.cluster import AgglomerativeClustering
from sklearn.cluster import DBSCAN

x=[]

base_dir = os.path.dirname(__file__)

print(os.path.join(base_dir, 'city.txt'))

f=open(os.path.join(base_dir, 'city.txt'),encoding='utf-8')
for v in f:
    #print(v)
    if not v.startswith('城'):
        continue
    else:
        x.append([float(v.split(':')[2].split(' ')[0]), float(v.split(':')[3].split(';')[0])])

#print(x)
x = np.array(x)

n_clusters = 8

#cls = KMeans(n_clusters).fit(x)
#cls = AgglomerativeClustering(linkage='ward',n_clusters=n_clusters).fit(x)
cls = DBSCAN(eps=3,min_samples=1).fit(x)
cls.labels_
n_clusters = len(set(cls.labels_))

markers = ['^','x','o','*','+','s','d','v']

for i in range(n_clusters):
    members = cls.labels_ == i
    plt.scatter(x[members,0],x[members,1],s=60,marker=markers[i], c='b', alpha=0.5)

plt.show()