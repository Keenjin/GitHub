import numpy as np

def exp1():
    a = np.array([1,2,3])
    print(a)
    a = np.array([[1,2],[3,4]])
    print(a)

def exp2():
    a = np.array([1,2,3], dtype = complex)
    print(a)

def exp3():
    a = np.array([[1],[4]])
    print(a.ndim)       # 数组的秩（维数）

def exp4():
    # int8, int16, int32, int64 四种数据类型可以使用字符串 'i1', 'i2','i4','i8' 代替
    student = np.dtype([('name','S20'),('age','i1'),('marks','f4')])    # 类似定义一个结构体，S20 name; int8 age; float marks;
    a = np.array([('keen',18,100.0),('lyc',20,99.5)], dtype=student)
    print(a)

def exp5():
    x = np.empty([3,2], dtype=int)      # 创建一个空数组，但是元素值是随机的，未初始化
    print(x)
    x = np.zeros((5,), dtype=int)
    print(x)
    x = np.ones((3,2),dtype=[('x','i4'),('y','f4')])
    print(x)

def exp6():
    x = [1,2,3]
    a = np.asarray(x)       # 普通数组转化为numpy数组
    print(a)
    y = (1,2,3)
    a = np.asarray(y)
    print(a)
    a = np.asarray(y, dtype=[('x','i4'),('y','f4'),('z',complex)])
    print(a)

def exp7():
    s = b'Hello Wold'       # 字符串转化为numpy数组。注意前面必须要有b，表示窄字符
    print(f'S{len(s)}')
    a = np.frombuffer(s, dtype=f'S{len(s)}')
    print(a)
    a = np.frombuffer(s, dtype='S1')        # 方便按固定长度分拆字符串，存储到数组中
    print(a)

def exp8():
    list = range(5)         # 迭代器中取出数组
    it = iter(list)
    x = np.fromiter(it, dtype=float)
    print(x)

def exp9():
    x = np.arange(5, dtype=int)
    print(x)
    x = np.arange(1,50,2,dtype=int)     # 取出所有奇数
    print(x)

def exp10():
    x = np.linspace(1,10,10)        # 等差数列
    print(x)
    x = np.logspace(1.0,2.0,num=10,base=10.0) # 对数log_base，输出base^1.0 ~ base^2.0，也即(10.0,100.0)
    print(x)

def exp11():
    x = np.asarray(range(10))
    a = np.reshape(x, (5,2))
    print(a)
    print(a[a>5])       # 取出满足条件的所有数据
    b = np.array([1,2+6j,5,3.5+5j])
    print(b[np.iscomplex(b)])   # 取出所有复数

def exp12():
    a = np.arange(8).reshape(2,4)
    print(a)
    print(a.ravel())    # 所有维度，全部转化为一维数组，遍历顺序方式由order控制
    print(np.transpose(a))  # a的转置
    print(a.T)              # a的转置

def exp13():
    a = np.arange(8).reshape(4,2)
    print(a)
    print(np.rollaxis(a,1))
    #print(np.rollaxis(a,2,1))

def exp14():
    a = np.array([[1,2],[3,4]])
    print(f"a=\n{a}")
    b = np.array([[5,6],[7,8]])
    print(f"b=\n{b}")
    print(np.concatenate((a,b)))        # axis=0，表示0轴不动，扩充1轴
    print(np.concatenate((a,b), axis = 1))  # axis=1，表示1轴不动，扩充0轴
    print(np.hstack((a,b)))     # 水平方向扩充数组
    print(np.vstack((a,b)))     # 垂直方向扩充数组

#exp1()
#exp2()
#exp3()
#exp4()
#exp5()
#exp6()
#exp7()
#exp8()
#exp9()
#exp10()
#exp11()
#exp12()
#exp13()
exp14()