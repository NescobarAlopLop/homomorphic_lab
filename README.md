## under construction
This project will run machine learning models on encrypted data.
It is still in its early stages of development.


to run debug server run from project root:
```shell script
docker-compose -f docker/docker-compose.yml up -d gdbserver
```


#### Sources for audio
project for downloading dataset:
https://github.com/marl/audiosetdl
https://github.com/google/youtube-8m#download-dataset-locally


python snippet from stack overflow explaining where to get weights and biases resulting 
from SVM training.

```python
import numpy as np
from sklearn.svm import SVC

X = np.array([[3, 4], [1, 4], [2, 3], [6, -1], [7, -1], [5, -3]])
y = np.array([-1, -1, -1, 1, 1, 1])

clf = SVC(C=1e5, kernel='linear')
clf.fit(X, y    )

w = clf.coef_
b = clf.intercept_
print('w = ', clf.coef_)
print('b = ', clf.intercept_)
print('Indices of support vectors = ', clf.support_)
print('Support vectors = ', clf.support_vectors_)
print('Number of support vectors for each class = ', clf.n_support_)
print('Coefficients of the support vector in the decision function = ', np.abs(clf.dual_coef_))

inference1 = np.sign(w.dot(X[0]))[0]
print(f'{inference1} vs {y[0]}')
inference2 = np.sign(w.dot(X[4]))[0]
print(f'{inference2} vs {y[4]}')
``` 

after training the model 

no need to generate cpp code, model can be loaded from text file stored on server or passed as
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
 key encryption migh be of benefit, mainly because of reduced size of transferred bytes.