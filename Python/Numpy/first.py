import numpy as np

def test1():
    my_array = np.array([1,2,3,4,5,6])
    print(my_array.shape)

    my_new_array = np.zeros(5)
    print(my_new_array)

    my_random_array = np.random.random((5,2))
    print(my_random_array)
    print(my_random_array.shape)

    a = np.array([[1.0, 2.0],[3.0, 4.0]])
    b = np.array([[5.0, 6.0],[7.0, 8.0]])
    sum = a + b
    difference = a - b
    product = a * b
    quotient = a / b
    print("sum =", sum)
    print("difference =", difference)
    print("product =", product)
    print("quotient =", quotient)

    # 矩阵乘法
    matrix_product = a.dot(b)
    print("matrix_product =", matrix_product)

    a = np.array([0,1,2,3,4])
    print(a)
    print(a.shape)

    b = np.array((0,1,2,3,4))
    print(b)
    print(b.shape)

    c = np.arange(5)
    print(c)
    print(c.shape)

    d = np.linspace(0,2*np.pi,5)
    print(d)
    print(d.shape)

def test2():
    a = np.array([[11, 12, 13, 14, 15],
                [16, 17, 18, 19, 20],
                [21, 22, 23, 24, 25],
                [26, 27, 28 ,29, 30],
                [31, 32, 33, 34, 35]])

    print("a[0, 1] = \n", a[0, 1])
    print("a[0, 1:4] = \n", a[0, 1:4])
    print("a[1:4, 1] = \n", a[1:4, 1])
    print("a[::2, ::2] = \n", a[::2, ::2])
    print("a[:, 1] = \n", a[:, 1])
    print("-"*10)
    print(type(a))
    print(a.dtype)
    print(a.size)
    print(a.shape)
    print(a.itemsize)
    print(a.ndim)
    print(a.nbytes)

def test3():
    a = np.arange(25)
    print("a =\n", a)
    a = a.reshape((5,5))
    print("a =\n", a)

    b = np.array([10, 62, 1, 14, 2, 56, 79, 2, 1, 45,
                  4, 92, 5, 55, 63, 43, 35, 6, 53, 24,
                  56, 3, 56, 44, 78])
    b = b.reshape((5,5))
    print("b =\n", b)

    print(a + b)
    print(a - b)
    print(a * b)
    print(a / b)
    print(a ** 2)
    print(a < b)
    print(a > b)
    print(a.dot(b))

    print(a.sum())
    print(a.min())
    print(a.max())
    print(a.cumsum())

def test4():
    a = np.arange(0, 100, 10)
    print(a)
    indices = [1, 5, -1]
    b = a[indices]
    print(b)

#test1()
#test2()
#test3()
test4()