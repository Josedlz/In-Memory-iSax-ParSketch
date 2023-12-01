import numpy as np
import scipy.fftpack

# Parameters
num_series = 30  # Total number of time series
time_points = 20  # Length of each time series
x = 10  # Number of high amplitude frequency components
y = 0  # Starting frequency component for high amplitude
high_amplitude = 10  # Arbitrary value for high amplitude
low_amplitude = 1  # Arbitrary value for low amplitude
chunk_size = num_series // 30  # Number of time series to process at a time

path_to_save = '../fourier_slice/fourier_slice_dataset.txt'

# Open a file to save the dataset
with open(path_to_save, 'w') as file:
    for _ in range(num_series // chunk_size):
        # Initialize frequency spectra
        spectra = np.zeros((chunk_size, time_points), dtype=complex)

        # Assign amplitudes and random phase shifts
        for i in range(chunk_size):
            high_freq_range = slice(y, y + x)
            low_freq_range1 = slice(0, y)
            low_freq_range2 = slice(y + x, time_points)
            spectra[i, high_freq_range] = high_amplitude * np.exp(1j * np.random.uniform(0, 2*np.pi, x))
            spectra[i, low_freq_range1] = low_amplitude * np.exp(1j * np.random.uniform(0, 2*np.pi, y))
            spectra[i, low_freq_range2] = low_amplitude * np.exp(1j * np.random.uniform(0, 2*np.pi, time_points - y - x))

        # Inverse Fourier transform to generate time series
        time_series_batch = scipy.fftpack.ifft(spectra).real

        # Add white noise
        time_series_batch += np.random.normal(0, 1, time_series_batch.shape)

        # Write the batch to the file
        for series in time_series_batch:
            series_str = ' '.join(map(str, series))
            file.write(series_str + '\n')

print(f"Dataset saved to {path_to_save}")

