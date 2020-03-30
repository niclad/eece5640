# 1) read data
# 2) store data
# 3) plot line chart

import numpy as np
import matplotlib.pyplot as plt
from matplotlib import rcParams
import csv
rcParams['font.sans-serif'] = ['Arial']

author_data = []
csv.register_dialect('txt_data', delimiter=' ')
with open('author_data-cuda.txt', newline='') as f:
    reader = csv.reader(f, 'txt_data')
    for row in reader:
        author_data.append(row)
        
author_data = np.array(author_data, dtype='uint32')
#test = np.reshape(author_data, (author_data.shape[1], author_data.shape[0])

fig, ax = plt.subplots()
ax.set_aspect(0.002)

ax.plot(author_data[:, 0], author_data[:, 1], color='green', linestyle='-', linewidth=1)
ax.set_xlabel("Co-authors published with")
ax.set_ylabel("Number of authors")
ax.set_title("Number of authors with an amount of co-authors")
ax.set_xlim(0, 350)
ax.set_ylim(-2000, 60000)
ax.axhline(0, color='k', linewidth=1, linestyle=':')
ax.spines['top'].set_visible(False)         # remove top border from plot
ax.spines['right'].set_visible(False)       # remove right border from plot

plt.show()
fig.savefig('cuda_dist.pdf', bbox_inches='tight')