import numpy as np
import matplotlib.pyplot as plt
from IPython.display import Latex,display
from sklearn.naive_bayes import GaussianNB
from matplotlib.ticker import MultipleLocator
from sklearn.ensemble import RandomForestClassifier

# 显示中文
def conf_zh(font_name):
    from pylab import mpl
    mpl.rcParams['font.sans-serif'] = [font_name]
    mpl.rcParams['axes.unicode_minus'] = False 
    
conf_zh('SimHei')

np.set_printoptions(suppress=True)  # 去掉科学计数法显示

jin = ['近','斤','今','金','尽']
jin_per = [0.3, 0.2, 0.1, 0.06, 0.03]
jintian = ['天','填','田','甜','添']
jintian_per = [
    [0.001, 0.001, 0.001, 0.001, 0.001],
    [0.001, 0.001, 0.001, 0.001, 0.001],
    [0.990, 0.001, 0.001, 0.001, 0.001],
    [0.002, 0.001, 0.850, 0.001, 0.001],
    [0.001, 0.001, 0.001, 0.001, 0.001]
]

wo = ['我','窝','喔','握','卧']
wo_per = [0.400, 0.150, 0.090, 0.050, 0.030]
women = ['们','门','闷','焖','扪']
women_per = [
    [0.970, 0.001, 0.003, 0.001, 0.001],
    [0.001, 0.001, 0.001, 0.001, 0.001],
    [0.001, 0.001, 0.001, 0.001, 0.001],
    [0.001, 0.001, 0.001, 0.001, 0.001],
    [0.001, 0.001, 0.001, 0.001, 0.001]
]

N = 5

def found_from_oneword(oneword_per):
    index = []
    values = []
    a = np.array(oneword_per)
    for v in np.argsort(a)[::-1][:N]:
        index.append(v)
        values.append(oneword_per[v])
    return index, values

def found_from_twoword(oneword_per, twoword_per):
    last = 0
    for i in range(len(oneword_per)):
        current = np.multiply(oneword_per[i], twoword_per[i])
        if i == 0:
            last = current
        else:
            last = np.concatenate((last, current), axis = 0)
    index = []
    values = []
    for v in np.argsort(last)[::-1][:N]:
        index.append([int(v / 5), int(v % 5)])
        values.append(last[v])
    return index, values

def predict(word):
    if word == 'jin':
        for i in found_from_oneword(jin_per)[0]:
            print(jin[i])
    elif word == 'jintian':
        for i in found_from_twoword(jin_per, jintian_per)[0]:
            print(jin[i[0]] + jintian[i[1]])
    elif word == 'wo':
        for i in found_from_oneword(wo_per)[0]:
            print(wo[i])
    elif word == 'women':
        for i in found_from_twoword(wo_per, women_per)[0]:
            print(wo[i[0]] + women[i[1]])
    elif word == 'jintianwo':
        index1, value1 = found_from_oneword(wo_per)
        index2, value2 = found_from_twoword(jin_per, jintian_per)
        last = np.multiply(value1, value2)
        for i in np.argsort(last)[::-1][:N]:
            print(jin[index2[i][0]], jintian[index2[i][1]], wo[i])
    elif word == 'jintianwomen':
        index1, value1 = found_from_twoword(jin_per, jintian_per)
        index2, value2 = found_from_twoword(wo_per, women_per)
        last = np.multiply(value1, value2)
        for i in np.argsort(last)[::-1][:N]:
            print(jin[index1[i][0]], jintian[index1[i][1]], wo[index2[i][0]], women[index2[i][1]])
            
if __name__ == '__main__':
    predict('jin')
    predict('jintian')
    predict('wo')
    predict('women')
    predict('jintianwo')
    predict('jintianwomen')