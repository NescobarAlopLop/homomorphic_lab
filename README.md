## under construction
This project will run machine learning models on encrypted data.
It is still in its early stages of development.

In the meanwhile please have a look at python implementation of said project:

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/NescobarAlopLop/homomorphic_lab/blob/master/notebook/homomorphic_encription_lab.ipynb)


# To only run and get the feel of it:
1. clone the repository: `git clone git@github.com:NescobarAlopLop/homomorhpic_lab.git`
2. cd into cloned repository `cd homomorhpic_lab`
3. run `run_executables_in_logical_order.sh` **from the root directory**. This script will
   compile the executables and run them in the correct order with pretrained
   model and example input.
   
# Prerequisites:
- install Microsoft Seal

# Getting started:
### Download dataset:
First we need some labeled data, I've used Googles [AudioSet](https://research.google.com/audioset/dataset/index.html).

In [`dataset`](./dataset) folder in project root directory you will find 2 csv files. They are both from
the original dataset website and as the names suggest:
- [`balanced_train_segments.csv`](./dataset/balanced_train_segments.csv) -  contains training data labels.
- [`eval_segments.csv`](./dataset/eval_segments.csv) -  contains testing data labels.
- [`class_labels_indices.csv`](./dataset/class_labels_indices.csv) -  contains testing data labels.

But that's not all, we are missing the audio files themselves, to download them please run the following terminal
commands:
```shell
cd ./dataset
chmod +x download.sh
cat eval_segments.csv | ./download.sh  # to download all audio files listed in csv file

# or
cat eval_segments.csv | grep /m/05tny_ | ./download.sh # to download files with specific tag 
```
For our needs a dog bark and a cats meow which are `/m/05tny_` and `/m/07qrkrw` respectively will be sufficient.

Source of download script is [here](https://github.com/unixpickle/audioset/blob/master/download/download.sh)

### Unpack audio files
If you have used the above way of downloading the audio files you'll discover that they are compressed
in .gz format, so our next step will be un-compressing them, since I don't see the need in keeping compressed files
I went with:
```shell
cd ./dataset
gunzip *.gz
```
in case you do want to keep the zipped files:
```shell
cd ./dataset
for f in *.gz ; do gunzip -c "$f" > /home/$USER/"${f%.*}" ; done
```
will do the trick.


### Train your model:
I've used scikit learn library for SVM model training, so you'll have to install
the requirements needed to run the `train.py` script
I assume you do have python version 3 and above.
To install python requirements for the project run:
```shell
pip3 install -r requirements.txt
```
_Note:_ I would suggest to use virtual environment to keep the project contained. 
But the details on how exactly to do that I'll leave to the reader.


to run debug server run from project root:
```shell script
docker-compose -f docker/docker-compose.yml up -d gdbserver
```

python snippet from stack overflow explaining where to get weights and biases resulting 
from SVM training.

```python
import numpy as np
from sklearn.svm import SVC

X = np.array([[3, 4], [1, 4], [2, 3], [6, -1], [7, -1], [5, -3]])
y = np.array([-1, -1, -1, 1, 1, 1])

clf = SVC(kernel='poly', gamma=1)
clf.fit(X, y)

print('b = ', clf.intercept_)
print('Indices of support vectors = ', clf.support_)
print('Support vectors = ', clf.support_vectors_)
print('Number of support vectors for each class = ', clf.n_support_)
print('Coefficients of the support vector in the decision function = ', np.abs(clf.dual_coef_))

negative_prediction = clf.dual_coef_.dot(np.power(clf.gamma * clf.support_vectors_.dot(X[0]), clf.degree)) + clf.intercept_
positive_prediction = clf.dual_coef_.dot(np.power(clf.gamma * clf.support_vectors_.dot(X[4]), clf.degree)) + clf.intercept_

print('Compare both results')
print(negative_prediction, clf.decision_function(X[0].reshape((1, 2))))
print(positive_prediction, clf.decision_function(X[4].reshape((1, 2))))

print('-' * 8)
print(f'clf.dual_coef_ {clf.dual_coef_[0]}')
print(f'clf.gamma {clf.gamma}')
print(f'clf.support_vectors_ {clf.support_vectors_}')
print(f'clf.degree {clf.degree}')
print(f'clf.intercept_ {clf.intercept_}')
assert np.sign(negative_prediction) == clf.predict(X[0].reshape((1, 2)))
assert np.sign(positive_prediction) == clf.predict(X[4].reshape((1, 2)))
``` 

after training the model 

no need to generate cpp code, model can be loaded from text file stored on a server or passed as
argument by managing python script.

need to read up on serialization and transfer of public key and encrypted model

next steps as i see them now are:

results first approach, first only create client server as different executables on same machine
save all the relevant data into files and w/r them locally

so in the following algorithm:
- send - save as a local file
- receive - read file

server:
    - generate encryption parameters and store them
    
client:
    - read encryption parameters
    - generate secret/public keys
    - encrypt with secret key the model weights and biases
    
    - run computation
    - return encrypted result and class definition function (class 0 if less than 0 class 1 otherwise)
    
client:
    - decrypt received results
    - evaluate according to classification function
    
 add masking for result vector:
 https://github.com/microsoft/SEAL/issues/64
 
 test performance and try to make optimizations
 
 i have used public key encryption because I assume that client should not have the access to the model
 and the server should not see the clients data.
 In case server is used for computation only and client is in charge of generating all the data then symmetric
 key encryption might be of benefit, mainly because of reduced size of transferred bytes.


cat meow:
`83,/m/07qrkrw,"Meow"`

dog bark:
`75,/m/05tny_,"Bark"`
