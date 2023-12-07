import numpy as np

# Parameters
num_series = 200 # Total number of time series
time_points = 256
mean = 0
std_dev = 1
chunk_size = num_series // 100  # Number of series to process at a time

path_to_save = '../random_walk/random_walk_dataset.txt'

with open(path_to_save, 'w') as file:
    for _ in range(num_series // chunk_size):
        steps_chunk = np.random.normal(mean, std_dev, (chunk_size, time_points))

        # Cumulative sum to create the random walk for each series in the chunk
        dataset_chunk = np.cumsum(steps_chunk, axis=1)

        for series in dataset_chunk:
            series_str = ' '.join(map(str, series))
            file.write(series_str + '\n')

print(f"Dataset saved to {path_to_save}")

