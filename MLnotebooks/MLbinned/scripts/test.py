import tensorflow as tf
import numpy as np
import pandas as pd
from tensorflow.python.keras.layers import Dense
from tensorflow.python.keras.models import Sequential
from tensorflow.python.keras.losses import binary_crossentropy
from tensorflow.python.keras import optimizers
from tensorflow.python.keras import initializers
import matplotlib.pyplot as plt
from tensorflow.python.keras.callbacks import ModelCheckpoint
import os
from tensorflow.python import keras
import tempfile
import matplotlib as mpl
from sklearn.model_selection import train_test_split
import seaborn as sns
import sys
import math
import random as rd
from sklearn.model_selection import KFold
import statistics
from math import sqrt

x=[0,1,2,3,4]
y=[2,3,4,5,6]

mpl.rcParams['figure.figsize'] = (12, 10)
print(1)
colors = plt.rcParams['axes.prop_cycle'].by_key()['color']
print(2)
fig=plt.figure(figsize=(12,10))
print(3)
ax=fig.add_subplot(1,1,1)
print(4)
ax.plot(x,y, color=colors[0], label='test')
print(5)
fig.savefig("test.png")
fig.savefig("test.pdf")
