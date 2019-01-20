import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

x = [1,2,3,4,5,6,7,8,9]
y = [0.199,0.389,0.5,0.783,0.980,1.777,1.38,1.575,1.771]
y1 = []
for xi in x:
    y1.append(xi**2 + xi*3 + np.random.randint(0,10))

print(y1)

W = np.vstack([x, np.ones(len(x))]).T
a, b = np.linalg.lstsq(W, y)[0]

f = 3

p = np.polyfit(x, y1, f)

def func(x, p1, p2, p3):
    return p1 * x ** f + p2 * x + p3

popt, pcov = curve_fit(func, x, y1)

np

x = np.array(x)
y = np.array(y)
y1 = np.array(y1)

#plt.plot(x, y, 'o', label='original data',markersize=10)
#plt.plot(x, a * x + b, 'r', label='fitted line')

plt.plot(x, y1, 'o', label='original data')
plt.plot(x, p[0] * x ** f + p[1] * x + p[2], 'r', label='fitted line')
plt.plot(x, func(x, *popt), 'b', label='fitted line2')

plt.show()