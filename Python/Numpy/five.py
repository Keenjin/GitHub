import numpy as np
import matplotlib.pyplot as plt

a = np.arange(180, step=1)
sin_a = np.sin(a * np.pi / 30)      # 正弦曲线，还有其他cos，tan，cot，arcsin等
print(sin_a)
plt.plot(a, sin_a)
plt.show()

def test1():
    a = np.array([1.0,3.2,5.55,12,0,1,-0.3,-3.7])
    print(a)
    print(np.around(a))     # 四舍五入
    print(np.around(a, decimals = 1))
    print(np.floor(a))      # 向下取整数
    print(np.ceil(a))       # 向上取整数

def test2():
    pass            # 加减乘除，add、subtract、multiply、divide
                    # 逐元素求倒数，reciprocal
                    # 次方运算，power
                    # 求余数，mod

test1()