
import matplotlib.pyplot as plt
import numpy as np

# Data for plotting
methods = ['Random Walk', 'Random F 5,0', 'Random F 5,2', 'Random F 5,4', 'Random F 5,8', 'Random F 50']
pruning = [80, 90, 85, 75, 70, 60]
precision_top1 = [100, 95, 97, 90, 85, 80]
query_time_top1 = [100, 400, 600, 800, 1000, 1200]

x = np.arange(len(methods))  # the label locations

# Plotting the bars
fig, ax1 = plt.subplots()

# Twin the axes for two different y-axes
ax2 = ax1.twinx()

# Bars for Pruning and Precision top1
ax1.bar(x - 0.2, pruning, width=0.4, label='Pruning', color='green')
ax1.bar(x + 0.2, precision_top1, width=0.4, label='Precision top1', color='blue')

# Line for Query response time top1
ax2.plot(x, query_time_top1, label='Query response time top1 (sec)', color='orange', marker='o')

# Labels, title and custom x-axis tick labels, etc.
ax1.set_xlabel('Method')
ax1.set_ylabel('Pruning / Precision %', color='g')
ax2.set_ylabel('Execution Time (sec)', color='b')
ax1.set_title('Pruning and Precision vs. Execution Time')
ax1.set_xticks(x)
ax1.set_xticklabels(methods, rotation=45, ha='right')
ax1.legend(loc='upper left')
ax2.legend(loc='upper right')

# Show grid
ax1.grid(True)

# Show plot
plt.show()

