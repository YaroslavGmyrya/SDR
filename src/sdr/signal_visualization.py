import numpy as np
import matplotlib.pyplot as plt


file1 = "../TX/pcm/tx_samples.pcm"
file2 = "txdata1.pcm"
file3 = "rxdata1.pcm"

rx = np.fromfile(f"{file3}", dtype=np.int16)

print(rx)

complex_IQ = (rx[0::2] + 1j * rx[1::2])

complex_IQ = complex_IQ[20000:25000]

plt.plot(np.real(complex_IQ))
plt.plot(np.imag(complex_IQ))
plt.show()


plt.scatter(np.real(complex_IQ), np.imag(complex_IQ))
plt.show()

conv = np.convolve(complex_IQ, [1,1,1,1,1,1,1,1,1,1])

plt.plot(conv)
plt.show()

plt.scatter(np.real(conv), np.imag(conv))
plt.show()