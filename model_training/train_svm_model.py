import pandas as pd
import sys
import os
import scipy.io.wavfile as sw
import python_speech_features as psf


PROJ_ROOT = '.'

print(os.path.abspath(PROJ_ROOT))
dataset_directory = os.path.join(PROJ_ROOT, 'dataset')
sys.path.append(dataset_directory)

print(os.path.abspath(dataset_directory))

labels = pd.read_csv(
    filepath_or_buffer=os.path.join(
        dataset_directory,
        'class_labels_indices.csv',
    ),
)

print(labels.columns)

cat_label = labels[labels['display_name'] == "Meow"]
dog_label = labels[labels['display_name'] == "Bark"]

balanced_train_segments = pd.read_csv(
    filepath_or_buffer=os.path.join(
        dataset_directory,
        'balanced_train_segments.csv',
    ),
    skiprows=[0, 1],
    sep=', ',
    engine='python',
)

print(cat_label.mid)
print(dog_label.mid)

print(balanced_train_segments.head(3))
print(balanced_train_segments.columns)
cat_sound_files = balanced_train_segments[balanced_train_segments.positive_labels.str.contains(cat_label.iloc[0].mid)]
dog_sound_files = balanced_train_segments[balanced_train_segments.positive_labels.str.contains(dog_label.iloc[0].mid)]

final_dataset = []
audio_files_directory = os.path.join(
    dataset_directory,
    'audio_files',
)
# for file_name in cat_sound_files['# YTID']:
#
# for file in os.path.join(audio_files_directory, 'audio_chunks', '*.wav'):
#     # rate, signal = sw.read(os.path.join(audio_files_directory, file_name))
#     features = psf.base.mfcc(signal=signal, samplerate=rate, preemph=1.1, nfilt=26, numcep=13)
#
#     features = psf.base.fbank(features)[1]
#     features = psf.base.logfbank(features)
#     features = psf.base.lifter(features, L=22)
#     # features = psf.base.lifter(features, L=10)
#     # features = psf.base.delta(features, N=13) # worsen results
#     features = pd.DataFrame(features)
#     features["Target"] = file_name
#     final_dataset.append(features)  # rbind(final_dataset,features)

