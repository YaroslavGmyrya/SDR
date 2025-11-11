import numpy as np
import matplotlib.pyplot as plt
import sys

def main():
    #check command line arguments
    if len(sys.argv) != 2:
        print(f"Programm wait 2 command-line argument, but have {len(sys.argv)}")

    # get filename
    file = sys.argv[1]

    #samples
    imag = []
    real = []

    count = []
    counter = 0

    #samples on symbol
    L = 10

    #open file
    with open(file, "rb") as f:
        index = 0
        #read 2L byte (2L bytes = 10 samples = 1 symbol)
        while (bytes := f.read(2 * L)):
            #if byte position is even, then it is I
            if(index % 2 == 0):
                for i in range(0,len(bytes), 2):
                    real.append(int.from_bytes(bytes[i:i+2], byteorder='little', signed=True))
                    counter += 1
                    count.append(counter)
            #if byte position is odd, then it is Q
            else:
                for i in range(0,len(bytes), 2):
                    imag.append(int.from_bytes(bytes[i:i+2], byteorder='little', signed=True))
            index += 1


    plt.subplot(2, 1, 1)
    plt.scatter((real)[5000:],(imag)[5000:],color='red')
    plt.xlabel("I")
    plt.ylabel("Q")
    plt.title("Signal constellation")

    plt.subplot(2, 1, 2)
    plt.plot(count, real, color='blue')
    plt.plot(count, imag, color='red')
    plt.xlabel("time")
    plt.ylabel("S(t)")
    plt.title("Signal")

    plt.show()

if __name__ == '__main__':
    main()
