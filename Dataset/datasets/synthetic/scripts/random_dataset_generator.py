import numpy as np

# Parameters
num_series = 2000  # 200 million time series
time_points = 256
mean = 0
std_dev = 1
chunk_size = num_series // 100  # Process 1 chunk at a time, adjust based on available memory

path_to_save = '../random_dataset/white_noise_dataset.txt'

# Open a file to save the dataset
with open(path_to_save, 'w') as file:
    for _ in range(num_series // chunk_size):
        # Generate a chunk of the dataset
        chunk = np.random.normal(mean, std_dev, (chunk_size, time_points))
        
        # Write the chunk to the file
        for series in chunk:
            series_str = ' '.join(map(str, series))
            file.write(series_str + '\n')

print(f"Dataset saved to {path_to_save}")

