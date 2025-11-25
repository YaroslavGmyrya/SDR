import numpy as np
import matplotlib.pyplot as plt
import sys

def main():
    #check command line arguments
    if len(sys.argv) != 2:
        print(f"Programm wait 2 command-line argument, but have {len(sys.argv)}")

    # get filename
    file = sys.argv[1]

    # count = []
    # counter = 0

    # #samples on symbol
    # L = 10

    # #open file
    # with open(file, "rb") as f:
    #     index = 0
    #     #read 2L byte (2L bytes = 10 samples = 1 symbol)
    #     while (bytes := f.read(2 * L)):
    #         #if byte position is even, then it is I
    #         if(index % 2 == 0):
    #             for i in range(0,len(bytes), 2):
    #                 real.append(int.from_bytes(bytes[i:i+2], byteorder='little', signed=True))
    #                 counter += 1
    #                 count.append(counter)
    #         #if byte position is odd, then it is Q
    #         else:
    #             for i in range(0,len(bytes), 2):
    #                 imag.append(int.from_bytes(bytes[i:i+2], byteorder='little', signed=True))
    #         index += 1

    rx = np.fromfile(f"{file}", dtype=np.int16)

    print(len(rx))

    real = [rx[x] for x in range(0,len(rx), 2)]
    imag = [rx[x] for x in range(1,len(rx), 2)]
    count = [x for x in range(len(rx) // 2)]
    imag = (imag / max(np.abs(imag)))[4000:]
    real = (real / max(np.abs(real)))[4000:]

    print(len(real))
    print(len((imag)))
    print(len(count))

    # plt.subplot(2, 1, 1)
    # plt.scatter((real)[5000:],(imag)[5000:],color='red')
    # plt.xlabel("I")
    # plt.ylabel("Q")
    # plt.title("Signal constellation")

    plt.plot([x for x in range(len(real))], real, color='blue')
    plt.plot([x for x in range(len(real))], imag, color='red')
    plt.xlabel("time")
    plt.ylabel("S(t)")
    plt.title("Signal")
    plt.show()
    
    h = [1] * 10

    conv_real = np.convolve(real, h, mode="same")
    conv_imag = np.convolve(imag, h, mode="same")

    plt.plot([x for x in range(len(real))], conv_real, color='blue')
    plt.plot([x for x in range(len(real))], conv_imag, color='red')
    plt.xlabel("time")
    plt.ylabel("S(t)")
    plt.title("Convolve")
    plt.show()

    # # compute e(k)
    e = []

    p1, p2 = 0, 0
    BnTs = 0.01
    Nsps = 10
    Kp = 0.002
    c = np.sqrt(2) / 2   
    theta = (BnTs / Nsps) / (c + 1/(4*c)) 
    K1 = (-4*c*theta) / ((1 + 2*c*theta+theta**2) * Kp)
    K2 = (-4*theta**2) / ((1+2*c*theta+theta**2) * Kp)
    offset = [0]
    offset2 = [] 
    Nsp = Nsps
    Ns = Nsps
    i = 0
    symbols_I = []
    symbols_Q = []
    for n in range(0, len(conv_imag) - (Nsp + Ns + Nsp//2) - 1):

        # Текущее смещение n + offset
        idx = n + offset[-1]

        # -----------------------------
        # 1. Вычисляем ошибку e
        # -----------------------------
        e.append(((conv_real[idx + Nsp + Ns] - conv_real[idx + Ns]) * conv_real[idx + Nsp//2 + Ns] +
            (conv_imag[idx + Nsp + Ns] - conv_imag[idx + Ns]) * conv_imag[idx + Nsp//2 + Ns]))

        # -----------------------------
        # 2. Фильтр петли
        # -----------------------------

        # Ветвь K1
        p1 = e[-1] * K1

        # Ветвь K2
        p2 = p2 + p1 + e[-1] * K2

        # Сброс p2, если вышла за границу
        p2 = p2 % 1.0

        # -----------------------------
        # 3. Обновление смещения offset
        # -----------------------------
        offset.append(int(round(p2 * Nsps)))
        symbols_I.append(conv_real[idx])
        symbols_Q.append(conv_imag[idx])

    # здесь можно обрабатывать найденный символ с индексом (n + offset)
    # например:
    # symbol_sample = I[n + offset] + 1j*Q[n + offset]

    # Смещение для следующего символа будет offset + Nsps

    plt.plot([x for x in range(len(e))], e)
    plt.show()

    plt.plot([x for x in range(len(offset))], offset)
    plt.show()

    print(offset)

    # I_symbs = real[abs(3)::10]
    # Q_symbs = imag[abs(3)::10]

    plt.scatter(symbols_I, symbols_Q)
    plt.show()


if __name__ == '__main__':
    main()
