import random
import math
import numpy as np

# 染色体长度
CHROMOSOME_SIZE = 15

# 收敛条件、判断退出
def is_finished(last_three):
    s = sorted(last_three)
    if s[0] and s[2] - s[0] < 0.001 * s[0]:
        return True
    return False

# 初始染色体样态
def init():
    chromosome_state1 = ['000000100101001', '101010101010101']    
    chromosome_state2 = ['011000100101100', '001100110011001']
    chromosome_state3 = ['001000100100101', '101010101010101']
    chromosome_state4 = ['000110100100100', '110011001100110']
    chromosome_state5 = ['100000100100101', '101010101010101']
    chromosome_state6 = ['101000100100100', '111100001111000']
    chromosome_state7 = ['101010100110100', '101010101010101']
    chromosome_state8 = ['100110101101000', '000011110000111']
    chromosome_states = [chromosome_state1,
                     chromosome_state2,
                     chromosome_state3,
                     chromosome_state4,
                     chromosome_state5,
                     chromosome_state6,
                     chromosome_state7,                     
                     chromosome_state8]    
    return chromosome_states

# 计算适应度
def fitness(chromosome_states):
    fitnesses = []
    for chromosome_state in chromosome_states:
        if chromosome_state[0][0] == '1':
            x = 10 * (-float(int(chromosome_state[0][1:],2)-1)/16384)
        else:
            x = 10 * (float(int(chromosome_state[0],2)+1)/16384)
        if chromosome_state[1][0] == '1':
            y = 10 * (-float(int(chromosome_state[1][1:],2)-1)/16384)
        else:
            y = 10 * (float(int(chromosome_state[1],2)+1)/16384)
        z = y * math.sin(x) + x * math.cos(y)
        fitnesses.append(z)
    return fitnesses

# 筛选
def filter(chromosome_states, fitnesses):
    # top 8 对应的索引值
    chromosome_state_new = []
    top1_fitness_index = 0
    for i in np.argsort(fitnesses)[::-1][:8].tolist():
        chromosome_state_new.append(chromosome_states[i])
        top1_fitness_index = i
    return chromosome_state_new, top1_fitness_index

# 生产下一代
def crossover(chromosome_states):
    chromosome_states_new = []
    while chromosome_states:
        chromosome_state = chromosome_states.pop(0)
        for v in chromosome_states:
            pos = random.choice(range(8, CHROMOSOME_SIZE - 1))
            chromosome_states_new.append([chromosome_state[0][:pos] + v[0][pos:], chromosome_state[1][:pos] + v[1][pos:]])
            chromosome_states_new.append([v[0][:pos] + chromosome_state[1][pos:], v[0][:pos] + chromosome_state[1][pos:]])
    return chromosome_states_new

# 基因突变
def mutation(chromosome_states):
    n = int(5.0 / 100 * len(chromosome_states))
    while n > 0:
        n -= 1
        chromosome_state = random.choice(chromosome_states)
        index = chromosome_states.index(chromosome_state)
        pos = random.choice(range(len(chromosome_state)))
        x = chromosome_state[0][:pos] + str(int(not int(chromosome_state[0][pos]))) +chromosome_state[0][pos+1:]
        y = chromosome_state[1][:pos] + str(int(not int(chromosome_state[1][pos]))) +chromosome_state[1][pos+1:]
        chromosome_states[index] = [x,y]

if __name__ == '__main__':
    # 初始群体
    chromosome_states = init()
    last_three = [0] * 3
    last_num = 0
    n = 100
    while n > 0:
        n -= 1
        # 产生下一代
        chromosome_states = crossover(chromosome_states)
        # 基因突变
        mutation(chromosome_states)
        fitnesses = fitness(chromosome_states)
        chromosome_states, top1_fitness_index = filter(chromosome_states, fitnesses)
        last_three[last_num] = fitnesses[top1_fitness_index]
        if is_finished(last_three):
            break
        if last_num >= 2:
            last_num = 0
        else:
            last_num += 1
    print(chromosome_states)