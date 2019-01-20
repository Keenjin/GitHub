import numpy as np
import matplotlib.pyplot as plt
from IPython.display import Latex,display
from sklearn.cluster import KMeans,AgglomerativeClustering,DBSCAN
from sklearn.ensemble import RandomForestClassifier

x = []
f = open('city.txt',encoding='utf-8')
for v in f:
    #print(v)
    if not v.startswith('城'):
        continue
    else:
        x.append([float(v.split(':')[2].split(' ')[0]), float(v.split(':')[3].split(';')[0])])
        
print(f'''
x_y={x_y}
''')

numpy.array([1,2,3]).round

def manhattan_distance(x,y):
    return np.sum(abs(x-y))
    
def cluster_distance_sum(n_clusters):
    cls = KMeans(n_clusters).fit(x_y)
    print(f'cluster_centers={cls.cluster_centers_}')
    distance_sum = 0
    for i in range(n_clusters):
        group = cls.labels_ ==i
        members = x_y[group,:]
        for v in members:
            distance_sum += manhattan_distance(np.array(v),cls.cluster_centers_[i])
    print(f'distance_sum={distance_sum}')
    return distance_sum
    
d = []
m = np.arange(1,5,1)
for i in m:
    print(f'clusters={i}')
    d.append(cluster_distance_sum(i))

d = np.array(d)
plt.plot(m, d, 'r')
plt.show()