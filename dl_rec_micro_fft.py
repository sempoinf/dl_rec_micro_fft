import sys
from scipy.fft import fft, rfft, rfftn

class fft_conver():
    def __init__(self) -> None:
        pass

def main(args: list):
    signal_mV = [1650] # need write a values
    sample_Hz = 60000  # 30 kHz - max

    hz = fft_conver()

if __name__ == '__main__':
    main(sys.argv)