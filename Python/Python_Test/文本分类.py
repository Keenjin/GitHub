import numpy as np
import matplotlib.pyplot as plt
from IPython.display import Latex,display
from sklearn.naive_bayes import GaussianNB
from matplotlib.ticker import MultipleLocator
from sklearn.ensemble import RandomForestClassifier
from sklearn.datasets import fetch_20newsgroups
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.feature_extraction.text import TfidfTransformer
from pprint import pprint

# 显示中文
def conf_zh(font_name):
    from pylab import mpl
    mpl.rcParams['font.sans-serif'] = [font_name]
    mpl.rcParams['axes.unicode_minus'] = False 
    
conf_zh('SimHei')

np.set_printoptions(suppress=True)  # 去掉科学计数法显示

newsgroups_train = fetch_20newsgroups(subset='train')
pprint(list(newsgroups_train.target_names))

