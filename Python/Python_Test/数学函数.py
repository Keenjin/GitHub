import numpy as np
import matplotlib.pyplot as plt

x = np.arange(1,9,0.1)
plt.plot(x,np.log2(x),color='red',label='$log(n)$')
plt.legend(loc='upper right',frameon=False)
plt.ylim(0, 15)

plt.plot(x,x,color='black',label='$n$')
plt.legend(loc='upper right',frameon=False)

plt.plot(x,x*np.log2(x),color='blue',label='$n*log(n)$')
plt.legend(loc='upper right',frameon=False)

plt.plot(x,x**2,color='green',label='$n^2$')
plt.legend(loc='upper right',frameon=False)

xn = np.arange(1,9,1)
yn = []
for i in xn:
    yn.append(np.math.factorial(i))
plt.plot(xn,yn,color='yellow',label='$n!$')
plt.legend(loc='upper right',frameon=False)

plt.show()
