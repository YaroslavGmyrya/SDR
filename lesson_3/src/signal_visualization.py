import sys
import matplotlib.pyplot as plt
import numpy as np
import re

def main():
    #check command arguments
    if len(sys.argv) > 2:
        print(f"Programm expected one argumet, but received {len(sys.argv)}")
        sys.exit(1)
    #file name
    input_file = sys.argv[1]

    #template for paring
    #template = r'(-?\d+)\s*,\s*(-?\d+)'
    template = r'(\d+)'

    #lists for I/Q samples
    amps = []

    #open file
    with open(input_file, "r") as file:
        for line in file:
            #find numbers
            all_amps = re.findall(rf"{template}", line)

            for amp in all_amps:
                #get index and amp
                amps.append(amp)


    indexes = np.arange(0, len(amps), 1)
    
    plt.plot(indexes, amps)
    plt.title("S(t)")
    plt.xlabel("A")
    plt.ylabel("S(t)")
    plt.grid(True)
    plt.show()

if __name__ == '__main__':
    main()