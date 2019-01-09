import numpy as np
import matplotlib.pyplot as plt


def test1():
    a = np.array([[3,7,5],[8,4,3],[2,4,9]])
    print(np.amin(a, 0))
    print(np.amin(a, 1))

    print(np.amin(a))
    print(np.amax(a))
    print(np.amax(a, axis = 0))

    print(np.ptp(a))        # 最大值和最小值的差
    print(np.ptp(a, axis = 1))

def test2():
    a = np.array([[10,7,6],[3,2,1]])
    print(a)
    print(np.percentile(a,50))

    print(np.median(a))     # 这个方法的实现，py3好像有问题
    print(np.median(a, axis=0))

    print(np.mean(a))       # 算术平均值
    print(np.average(a))
    wts = np.array([[1,2,1],[3,7,2]])
    print(np.average(a, weights = wts,axis=0))

    print(np.var([1,2,3,4]))        # 求方差


def test3():
    # 计算正弦和余弦曲线上的点的 x 和 y 坐标 
    x = np.arange(0,  3  * np.pi,  0.1) 
    y_sin = np.sin(x) 
    y_cos = np.cos(x)  
    # 建立 subplot 网格，高为 2，宽为 1  
    # 激活第一个 subplot
    plt.subplot(2,  2,  1)  
    # 绘制第一个图像 
    plt.plot(x, y_sin) 
    plt.title('Sine')  
    plt.subplot(2,  2,  2)  
    # 绘制第一个图像 
    plt.plot(x, y_sin) 
    # 将第二个 subplot 激活，并绘制第二个图像
    plt.subplot(2,  1,  2) 
    plt.plot(x, y_cos) 
    plt.title('Cosine')  
    # 展示图像
    plt.show()

test3()