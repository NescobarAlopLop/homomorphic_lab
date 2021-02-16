import copy
import os
import sys

import numpy as np
import pandas as pd
import python_speech_features as psf
import scipy.io.wavfile as sw

from sklearn import svm
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler


PROJ_ROOT = '.'
dataset_directory = os.path.join(PROJ_ROOT, 'dataset')
sys.path.append(dataset_directory)

audio_files_directory = os.path.join(dataset_directory, 'audio_files')

final_dataset = pd.DataFrame()  # Blank initial dataset

number_of_filters = 26
for file_name in os.listdir(audio_files_directory):
    rate, signal = sw.read(os.path.join(audio_files_directory, file_name))
    features = psf.base.mfcc(signal=signal, samplerate=rate, preemph=1.1, nfilt=number_of_filters, numcep=17)
    features = psf.base.fbank(features)[1]
    features = psf.base.logfbank(features)
    features_df = pd.DataFrame(features)

    if 'dog' in file_name:
        features_df['label'] = '0'
    elif 'cat' in file_name:
        features_df['label'] = '1'
    else:
        raise ValueError(f'Unsupported animal class {file_name}')

    final_dataset = final_dataset.append(features_df, ignore_index=True)  # rbind(final_dataset,features)

print(final_dataset.shape)

# Finalize dataset with the attributes and target
X = final_dataset.iloc[:, 0:-1]
y = final_dataset.iloc[:, -1]

# Splitting into test and train
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.24, random_state=1)

# Feature Scaling
sc = StandardScaler()
X_train = sc.fit_transform(X_train)
X_test = sc.fit_transform(X_test)

X_train = pd.DataFrame(X_train)
X_test = pd.DataFrame(X_test)


def one(X, Y):
    return (np.dot(X, Y.T) + 1) ** 3


def two(X, Y):
    return (np.dot(X, Y.T) + 1) ** 2


def three(X, Y):
    return X.dot(Y.T) ** 3


def four(X, Y):
    return X.dot(Y.T) ** 4


def five(X, Y):
    res = np.dot(X, Y.T) + 1
    return res ** 5


trained_models = {}
for kernel in [
    one,
    two,
    three,
    four,
    five,
    'linear',
    'poly',
]:
    model = svm.SVC(
        kernel=kernel,
    )
    model.fit(X_train, y_train)
    acc_score = model.score(X_test, y_test)

    try:
        print(f'\n{str(kernel.__name__)} - acc_score: {acc_score}')
    except Exception:
        print(f'\n{str(kernel)} - acc_score: {acc_score}')

    trained_models[kernel] = {
        'model': copy.deepcopy(model),
        'acc_score': acc_score,
    }
