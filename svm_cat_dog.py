import os
import re
from subprocess import call
from time import sleep
import pandas as pd
import python_speech_features as psf
import scipy.io.wavfile as sw
from sklearn import svm
from sklearn.metrics import accuracy_score
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
from sklearn_porter import Porter
import numpy as np

file_names = [f for f in os.listdir('./cats_dogs') if os.path.isfile(os.path.join('./cats_dogs', f))]
final_dataset = pd.DataFrame()  # Blank initial dataset

for i in file_names:
    rate, signal = sw.read(os.path.join('./cats_dogs', i))
    features = psf.base.mfcc(signal=signal, samplerate=rate, preemph=1.1, nfilt=26, numcep=13)

    features = psf.base.fbank(features)[1]
    features = psf.base.logfbank(features)
    features = psf.base.lifter(features, L=22)
    # features = psf.base.lifter(features, L=10)
    # features = psf.base.delta(features, N=13) # worsen results
    features = pd.DataFrame(features)
    features["Target"] = i
    final_dataset = final_dataset.append(features)  # rbind(final_dataset,features)


# Correcting indexing
index = 26
for i in range(0, len(final_dataset)):
    final_dataset.iloc[i, index] = final_dataset.iloc[i, index].replace('.wav', '')
    final_dataset.iloc[i, index] = re.sub(r'[0-9]+', '', final_dataset.iloc[i, index])
    final_dataset.iloc[i, index] = final_dataset.iloc[i, index].replace('_', '')
    final_dataset.iloc[i, index] = final_dataset.iloc[i, index].replace('barking', '0')
    final_dataset.iloc[i, index] = final_dataset.iloc[i, index].replace('cat', '1')
    final_dataset.iloc[i, index] = final_dataset.iloc[i, index].replace('dog', '0')
    final_dataset.iloc[i, index] = final_dataset.iloc[i, index].replace('00', '0')

# Finalize dataset with the attributes and target
# final_dataset.rename(columns={'y': 'target'})
X = final_dataset.iloc[:, 0:26]
y = final_dataset.iloc[:, -1]

# Spliting into test and train
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.24, random_state=1)
# del y_train

# Feature Scaling
sc = StandardScaler()
X_train = sc.fit_transform(X_train)
# X_test = sc.transform(X_test)
X_test = sc.fit_transform(X_test)

# type(y_train)
X_train = pd.DataFrame(X_train)
# y_train = pd.DataFrame(y_train)
X_test = pd.DataFrame(X_test)

print(y_test.values[0:18])


for kernel in [
    'linear',
    'poly',
    'rbf',
    'sigmoid',
]:
    model = svm.SVC(C=1.0, kernel=kernel,
                    degree=3,
                    gamma=1.0/26,
                    # gamma='auto',
                    coef0=0.0, shrinking=True, probability=False,
                    tol=0.001, cache_size=200, class_weight=None, verbose=False, max_iter=-1,
                    decision_function_shape='ovr', break_ties=False, random_state=None)

    model.fit(X_train, y_train)
    model.support_vectors_ = np.around(model.support_vectors_, decimals=2)
    model.intercept_ = np.around(model.intercept_, decimals=2)
    model.dual_coef_ = np.around(model.dual_coef_, decimals=2)
    model.score(X_train, y_train)
    predicted = model.predict(X_test)
    acc_score = accuracy_score(y_test, predicted)
    print(f'\nacc_score: {acc_score}')

    # Export:
    # c
    porter = Porter(model, language='c')
    output = porter.export(embed_data=True)
    # print(output)
    print(f'saving C code')
    with open(f'./porter_out/svc_{kernel}.c', 'w+') as f:
        f.write(output)
    sleep(1)

    # js
    porter = Porter(model, language='js')
    output = porter.export(embed_data=True)
    # print(output)
    print(f'saving js code')
    with open(f'./porter_out/svc_{kernel}.js', 'w+') as f:
        f.write(output)
    sleep(1)

    print('compiling svc')
    os.system(f'gcc ./porter_out/svc_{kernel}.c -std=c99 -lm -o ./porter_out/svc_{kernel}.o')
    sleep(1)
    print('run svc for all test inputs, kernel: {}'.format(kernel))
    print('c: result\t\t', end='')
    for test in X_test.values:
        call_array = [f'./porter_out/svc_{kernel}.o'] + list(map(str, np.around(test, decimals=2)))
        call(call_array)

    print('\npython result:\t', end='')
    print("".join([f'{x}' for x in predicted]))

    print('\ntrue value:\t\t', end='')
    print("".join([f'{x}' for x in y_test]))

    print(f'\n./porter_out/svc_{kernel}.o {call_array}')

print(model)
