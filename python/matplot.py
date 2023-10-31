import matplotlib.pyplot as plt
import numpy as np


kf = 4
sample_step = 0.001
fc = 10
time_vector = np.arange(0.0, 10.0, sample_step)
ampl = 1
signal = ampl*np.sin(2*np.pi*time_vector)
fm = np.zeros(time_vector.shape[0])

area_under_signal = 0

for i,x in enumerate(time_vector):
    area_under_signal += signal[i] * sample_step
    fm[i] = ampl*np.cos(2*np.pi*fc*x + 2*np.pi*kf*area_under_signal) 

fig, ax = plt.subplots()
ax.plot(time_vector, signal, 'g--')
ax.plot(time_vector, fm, color='blue')

plt.show()