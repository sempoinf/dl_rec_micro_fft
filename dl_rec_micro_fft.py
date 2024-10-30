import sys
from scipy.fft import fft, fftfreq
import numpy as np

class fft_conver():
    def __init__(self, signal, sample, n_sample) -> None:
        self.signal = signal,
        self.sample = sample,
        self.n_sample = n_sample

    def calculate(self):
        # Преобразование Фурье
        fft_result = fft(self.signal)
        print(f"fft covert - {fft_result}")
        fft_magnitudes = np.abs(fft_result)
        print(f"fft magnitudes - {fft_magnitudes}")
        # Частоты
        frequencies = fftfreq(self.n_sample, d=1/self.sample)
        print(f"frequency - {frequencies}")
        # Ищем пиковую частоту
        positive_frequencies = frequencies[:self.n_sample // 2]
        positive_magnitudes = fft_magnitudes[:self.n_sample // 2]
        peak_frequency = positive_frequencies[np.argmax(positive_magnitudes)]
        print(f"Основная частота сигнала: {peak_frequency} Гц")

def main(args: list):
    adc_mV = [1650] # need write a values
    sample_rate = 60000  # 30 kHz - max
    num_samples = len(adc_mV)

    hz = fft_conver(adc_mV, sample_rate, num_samples)
    hz.calculate()

if __name__ == '__main__':
    main(sys.argv)