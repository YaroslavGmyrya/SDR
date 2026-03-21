import numpy as np
from matplotlib import pyplot as plt 



filename = "build/rx_samples.pcm"
samples = np.fromfile(filename, dtype=np.int16)

iq = samples.astype(np.float32).view(np.complex64)

plt.rcParams["agg.path.chunksize"] = 10000
plt.plot(np.real(iq))
plt.plot(np.imag(iq))
plt.show()