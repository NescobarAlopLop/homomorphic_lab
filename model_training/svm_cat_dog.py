import codecs
import copy
import json
import os
import sys

import numpy as np
import pandas as pd
import python_speech_features as psf
import scipy.io.wavfile as sw
import tenseal as ts
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
    if not os.path.isfile(os.path.join(audio_files_directory, file_name)):
        continue

    rate, signal = sw.read(os.path.join(audio_files_directory, file_name))
    features = psf.base.mfcc(signal=signal, samplerate=rate, preemph=1.1, nfilt=number_of_filters, numcep=17)
    features = psf.base.fbank(
        signal=features,
        samplerate=rate,
    )[1]
    features = psf.base.logfbank(features)
    features_df = pd.DataFrame(features)

    if 'dog' in file_name:
        features_df['label'] = '-1'
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
u = np.array(X_train.mean())
s = np.array(X_train.std())

# Feature Scaling
sc = StandardScaler()
X_train = sc.fit_transform(X_train)
X_train = pd.DataFrame(X_train)

X_test = sc.fit_transform(X_test)
X_test = pd.DataFrame(X_test)


trained_models = {}
for kernel in [
    # 'linear',
    'poly',
]:
    model = svm.SVC(
        kernel=kernel,
        C=20,
        gamma=10,
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


print(f'prediction {y_train.iloc[0]} {model.predict(np.array(X_train.iloc[0,:]).reshape((1,26)))}')
print(f'prediction {y_train.iloc[1]} {model.predict(np.array(X_train.iloc[1,:]).reshape((1,26)))}')


# Setup TenSEAL context
context = ts.context(
    ts.SCHEME_TYPE.CKKS,
    poly_modulus_degree=8192 * 2,
    coeff_mod_bit_sizes=[60, 40, 40, 40, 40, 40, 60]
)
context.generate_galois_keys()
context.global_scale = 2**40

model = trained_models['poly']['model']
for file_name in os.listdir(audio_files_directory)[:12]:
    if not os.path.isfile(os.path.join(audio_files_directory, file_name)):
        continue

    rate, signal = sw.read(os.path.join(audio_files_directory, file_name))
    features = psf.base.mfcc(signal=signal, samplerate=rate, preemph=1.1, nfilt=number_of_filters, numcep=17)
    features = psf.base.fbank(features)[1]
    features = psf.base.logfbank(features)
    features = np.array(features)
    features = np.array((features[0] - u) / s).reshape((1, 26))

    prediction = model.dual_coef_.dot(np.power(model.gamma * model.support_vectors_.dot(features.T), model.degree)) + model.intercept_

    print(f'\t{file_name}  {model.predict(features)[0]} {model.decision_function(features)[0]}')
    print(f'\t\t\t\t{np.sign(prediction)}  {prediction}\n')

    bias = model.intercept_[0]
    degree = model.degree
    support_vectors = model.support_vectors_
    query = features
    gamma = model.gamma
    dual_coefficients = model.dual_coef_

    enc_query = ts.ckks_vector(context, query.tolist()[0])

    inside_kernel = enc_query.matmul(support_vectors.T.tolist()) * gamma
    kernel_result = inside_kernel.square() * inside_kernel

    prediction_enc = kernel_result.dot(dual_coefficients[0].tolist()) + bias

    print(f'expected: {prediction}')
    print(f'result: {prediction_enc.decrypt()}')
    print('-' * 30, '\n')


result_model = {
    'dual_coef': model.dual_coef_.tolist(),
    'gamma': model.gamma,
    'support_vectors': model.support_vectors_.tolist(),
    'degree': model.degree,
    'bias': model.intercept_[0],
}

file_path = 'example_inputs/model.json'
json.dump(result_model, codecs.open(file_path, 'w', encoding='utf-8'), separators=(',', ':'), sort_keys=True, indent=4)
