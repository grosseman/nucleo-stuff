import matplotlib.pyplot as plt
import numpy as np
import wave
import struct


file = '96.301_2023-10-09_22-50-29.wav'
output_file = 'output.txt'

with wave.open(file, 'rb') as wav:
    samplefreq = wav.getframerate()
    nof_samples = wav.getnframes()
    print(f"Nof samples: {nof_samples}")
    timevect = np.arange(0, nof_samples, 1)
    signal = np.zeros(nof_samples)

    for i in range(nof_samples):
        signal[i] = (struct.unpack('h', wav.readframes(1)))[0]/65535

fig = plt.plot(timevect, signal)
plt.show()

#with open(output_file, 'w') as f:
    
