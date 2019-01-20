import numpy as np
from sklearn.naive_bayes import GaussianNB
import pprint

data_table = [['date','weather'],
              [1,0],
              [2,1],
              [3,2],
              [4,1],
              [5,2],
              [6,0],
              [7,0],
              [8,3],
              [9,1],
              [10,1]]

# 当天的天气
print(data_table[1:-1])
X = []
for e in data_table[1:-1]:
    X.append(e[1])

X = np.array(X).reshape(9,1)
print(X)

y = [1,2,1,2,0,0,3,1,1]
clf = GaussianNB().fit(X, y)
p = [[1]]
print(np.array(clf.predict_proba(X)))
print(clf.predict(p))
print(clf.predict_proba(p))