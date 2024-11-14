import os
import re
import sys
from scipy.fft import fft, fftfreq
import numpy as np
import matplotlib.pyplot as plt

class ParserResult:
    def __init__(self, filename: str) -> None:
        """
        Initializes the class with the specified filename.
        :param filename: Path to the file to be parsed.
        """
        self.filename = filename
        self.data = self._load_file()

    def _load_file(self) -> str:
        """
        Loads the file content as a string.
        :return: The content of the file as a single string.
        """
        if not os.path.isfile(self.filename):
            raise FileNotFoundError(f"File '{self.filename}' not found.")
        
        with open(self.filename, 'r') as file:
            return file.read()
            

    def parse_section(self, section_number: int) -> list[int]:
        """
        Extracts mV values from the specified section.
        :param section_number: Index of the section to extract (starting from 1).
        :return: A list of mV values from the specified section.
        """
        # Split the data into sections based on "END OF DATA"
        sections = self.data.split("END OF DATA")
        
        # Remove any empty sections or unnecessary whitespace
        sections = [section.strip() for section in sections if section.strip()]

        print(f"Count of sections: {section_number} of {len(sections)}")
        
        # Ensure the specified section number is within range
        if section_number < 1 or section_number > len(sections):
            raise IndexError("The specified section number is out of range.")
        
        # Select the specified section
        selected_section = sections[section_number - 1]
        
        # Extract all values preceding 'mV' in the section
        mv_values = re.findall(r"[-]?\d+(?= mV)", selected_section)

        # print(f"Origin: {mv_values}")
        mv_values.pop()
        # print(f"Poped: {mv_values}")

        # Convert the extracted values to a list of integers
        return [int(value) for value in mv_values]

class SetZero():
    def __init__(self, adc_signal:tuple) -> None:
        self.adc_signal = adc_signal

    def transform_analog(self)-> list[int]:
        avg = np.average(self.adc_signal)
        return [int(elem - avg) for elem in self.adc_signal]

class Plottering():
    def __init__(self, analog_signal:tuple, flag_interval = False) -> None:
        self.ys = analog_signal

        interval_size = 2 if flag_interval else 1.0
        self.xs = [x * interval_size for x in range(len(self.ys))]


    def graph(self):
        plt.plot(self.xs, self.ys)
        plt.show()
        # Make sure to close the plt object once done
        plt.close()

class FFT_convert():
    def __init__(self, analog_signal:tuple, sample: int , n_sample: int) -> None:
        self.analog_signal = analog_signal
        self.sample = sample
        self.n_sample = n_sample

    def calculate(self):
        # Преобразование Фурье
        fft_result = fft(self.analog_signal)
        # print(f"fft covert - {fft_result}")
        fft_magnitudes = np.abs(fft_result)
        # print(f"fft magnitudes - {fft_magnitudes}")
        # Частоты
        frequencies = fftfreq(self.n_sample, d=1/self.sample)
        # print(f"frequency - {frequencies}")
        # Ищем пиковую частоту
        positive_frequencies = frequencies[:self.n_sample // 2]
        positive_magnitudes = fft_magnitudes[:self.n_sample // 2]
        peak_frequency = round(positive_frequencies[np.argmax(positive_magnitudes)])
        print(f"Main frequency: {peak_frequency} Hz")

def main(args: list):
    adc_mV_still = () # need write a values
    adc_mV_1000 = () # need write a values
    sample_rate = 5000  # for 1500 kHz - max

    data = ParserResult(filename='../dl_rec_read_data_py/res/results_500Hz.txt')

    adc_mV_still = data.parse_section(3)

    analog_signal = SetZero(adc_mV_still)
    transform_sig = analog_signal.transform_analog()
    # input(transform_sig)

    num_samples = len(transform_sig)
    hz = FFT_convert(adc_mV_still, sample_rate, num_samples)
    hz.calculate()

    sine = Plottering(transform_sig)
    sine.graph()

if __name__ == '__main__':
    main(sys.argv)