import numpy as np
import matplotlib.pyplot as plt

# Открываем файл для чтения
name = "../../build/rxdata1.pcm"
data = []
imag = []
real = []
count = []
counter = 0

L = 10
with open(name, "rb") as f:
    index = 0
    while (bytes := f.read(2 * L)):
        if(index % 2 == 0):
            for i in range(0,len(bytes), 2):
                real.append(int.from_bytes(bytes[i:i+2], byteorder='little', signed=True))
                counter += 1
                count.append(counter)
        else:
            for i in range(0,len(bytes), 2):
                imag.append(int.from_bytes(bytes[i:i+2], byteorder='little', signed=True))
        index += 1

        

plt.figure(1)
plt.scatter((real)[5000:],(imag)[5000:],color='red')  # Используем scatter для диаграммы созвездия
plt.show()