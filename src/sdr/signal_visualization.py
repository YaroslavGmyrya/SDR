import numpy as np
import matplotlib.pyplot as plt


file1 = "../TX/pcm/tx_samples.pcm"
file2 = "txdata1.pcm"
file3 = "rxdata1.pcm"

rx = np.fromfile(f"{file3}", dtype=np.int16)

print(rx)

complex_IQ = (rx[0::2] + 1j * rx[1::2])

plt.plot(np.real(complex_IQ))
plt.plot(np.imag(complex_IQ))
plt.show()