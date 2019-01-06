import numpy as np
import matplotlib.pyplot as plt

def test1():
    a = np.linspace(0,2*np.pi,50)
    #a = np.arange(25)
    #plt.plot(a)
    b = np.sin(a)
    plt.plot(a,b)
    mask = b >= 0
    plt.plot(a[mask], b[mask], 'bo')
    mask = (b >= 0) & (a <= np.pi / 2)
    plt.plot(a[mask], b[mask], 'go')
    plt.show()

def test2():
    a = np.arange(0, 100, 10)
    b = a[:5]
    c = a[a >= 50]
    print(b)
    print(c)

    print("-" * 10)
    b = np.where(a < 50)
    c = np.where(a >= 50)[0]
    print(b)
    print(c)

def test3():
    v1 = np.array([[2],[1],[3]])
    print(v1)
    print("-" * 10)
    v2 = np.array([[2,1,3]])
    print(v2)
    print("-" * 10)
    v3 = np.transpose(v2)
    print(v3)
    print("-" * 10)

def test4():
    A = np.array([[2,1,-2],[3,0,1],[1,1,-1]])
    b = np.transpose(np.array([[-3,4,-2]]))
    x = np.linalg.solve(A, b)
    print("Ax=b, x=", x)

#test1()
#test2()
#test3()
test4()
