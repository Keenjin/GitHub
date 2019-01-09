import numpy as np

def exp1():
    a,b = 13,17
    print(bin(a), bin(b))                # 打印出整数的二进制
    print(bin(np.bitwise_and(a, b)))     # 打印出按位与的结果
    print(bin(np.bitwise_or(a,b)))       # 打印出按位或的结果
    print(bin(np.invert(np.array([13], dtype=np.uint8)[0])))             # 无符号按位取反；有符号取补码+1
    print(np.binary_repr(a, width = 8))  # 按照一定规则的二进制展示
    print(np.binary_repr(np.invert(np.array([13], dtype=np.uint8)[0]), width = 8))  # 按照一定规则的二进制展示
    print(np.binary_repr(np.left_shift(a,2), width = 8))        # 左移2位，不足补0
    print(np.binary_repr(np.right_shift(a,2), width = 8))       # 右移2位，不足补0

exp1()