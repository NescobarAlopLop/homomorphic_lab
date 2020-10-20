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


run one example from server or debug container - done
run multiple - done
create personal cmake and main - done
get example model

run homomorhic computation on extracted features of a sound
    probably code generator since values may change between training and if model is improved
compare results

write sever/client application
client:
extracts features
get parameters from server
generates keys and sends data for computation on server
receives encrypted data from server and decrypts the results

server can train new models
generate homomorphic C++ code to run evaluations on encrypted queries
generate parameters
transfers parameters to client
receives encrypted data
performs calculation and return encrypted result



read all the examples:
- bfv basics:
    - only integers 
    - supports addition and multiplication. addition is much "cheaper" in noise
    - for all but simplest computations batching should be used
- ckks:
    - real numbers


probably most important part of the knowledge required to start working on SEAL code can be found [here](https://stats.stackexchange.com/questions/39243/how-does-one-interpret-svm-feature-weights/355043#355043?newreg=8dc6259a834646c28769dfa2bb564de0)
in short:
```python
import numpy as np
from sklearn.svm import SVC

X = np.array([[3, 4], [1, 4], [2, 3], [6, -1], [7, -1], [5, -3]])
y = np.array([-1, -1, -1, 1, 1, 1])

clf = SVC(C=1e5, kernel='linear')
clf.fit(X, y)

w = clf.coef_
b = clf.intercept_
print('w = ', clf.coef_)
print('b = ', clf.intercept_)
print('Indices of support vectors = ', clf.support_)
print('Support vectors = ', clf.support_vectors_)
print('Number of support vectors for each class = ', clf.n_support_)
print('Coefficients of the support vector in the decision function = ', np.abs(clf.dual_coef_))
``` 

after training the model 

no need to generate cpp code, model can be loaded from text file stored on server or passed as
argument by managing python script.

need to read up on serialization and transfer of public key and encrypted model

next steps as i see them now are:
client:
    - generate keys
    - encrypt query
    - send public key and encrypted query to server
    
server:
    - encrypt with public keys the model weights
    - run computation
    - return encrypted result and class definition function (class 0 if less than 0 class 1 otherwise)
    
client:
    - decrypt received results
    - evaluate according to classification function
    
 