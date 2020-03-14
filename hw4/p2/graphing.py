# -*- coding: utf-8 -*-
"""
Created on Tue Feb 18 15:12:34 2020

@author: Nicolas
"""
import numpy as np
from matplotlib import pyplot as plt
from matplotlib import ticker as mtick
from matplotlib import rcParams
from matplotlib.patches import Patch
rcParams['font.family'] = 'sans-serif'
rcParams['font.sans-serif'] = ['Arial']
rcParams['hatch.linewidth'] = 0.75
department = ["1000", "1,000,000", "10,000,000"]
quarter = ["1", "2", "4"]

budgets = np.array([[0.00005781, 0.0396,  0.386],\
                    [0.00003254, 0.0196,  0.196],\
                    [0.00002466, 0.00984, 0.0979]])

# P2-B 50 bins
# =============================================================================
# budgets = np.array([[0.0001648,  0.165,  1.645],\
#                     [0.0001227,  0.0881, 0.780],\
#                     [0.00006448, 0.0403, 0.405]])
# =============================================================================
# P2-B 10 bins
# =============================================================================
# budgets = np.array([[0.00004128, 0.0381, 0.384],\
#                     [0.00003361, 0.0306, 0.238],\
#                     [0.00002049, 0.0216, 0.160]])
# =============================================================================
    
# set up barchart
x = np.arange(len(department)) # label locations
width = 0.8    # width of all the bars

fig, ax = plt.subplots()
rects = []
color = ["tomato", "royalblue", "limegreen"]
n = len(quarter)
for i in range(n):
    bar_x = x - width/2.0 + i/float(n)*width + width/(n*2)

    m = len(budgets[i,:])
    for j in range(m):
        bar_x = x[j%3] - width/2.0 + i/float(n)*width + width/(n*2)
        e = budgets[i,j]
        if (j >= 3):
            rects.append(ax.bar(bar_x, e, width=width/float(n), \
                    label=quarter[i], color=color[i], hatch='////'))
        else:
            #bar_x = x - width/2.0 + i/float(n)*width + width/(n*2)
            rects.append(ax.bar(bar_x, e, width=width/float(n), \
                    label=quarter[i], color=color[i]))
                        

#plt.xticks(rotation=-45)
# rotate and realign x-axis labels
plt.setp(ax.xaxis.get_majorticklabels(), rotation=-15, horizontalalignment='left')
ax.set_aspect("auto")

# format y-axis 
#ax.yaxis.set_major_formatter(mtick.PercentFormatter())  # make y-ticks be %
ax.set_ylabel("Execution time (s)")
ax.yaxis.grid(which="major", color="white", lw=0.75)
#ax.yaxis.set_minor_locator(mtick.MultipleLocator(10))
ax.yaxis.grid(which="minor", color="white", linestyle=":", lw=0.75)
plt.ylim(bottom=0, top=0.42)

ax.set_title("Execution time for varying number amounts and 10 bins")   # title


# format x-axis
ax.set_xticks(x)
ax.set_xlabel("Amount of numbers to bin")
ax.xaxis.set_label_coords(0.5, -0.15)
ax.set_xticklabels(department)
ax.xaxis.set_minor_locator(mtick.MultipleLocator(0.5))

# format plot
#ax.legend(loc="lower left", fancybox=False, edgecolor='k')  # format legend
#ax.axhline(100, color="black", lw=0.75, ls="-", zorder=2)  # format 0-axis
for i in range(len(department)-1):
    v_loc = (x[i] + x[i+1]) / 2
    #ax.axvline(v_loc, color="white")
ax.spines['top'].set_visible(False)         # remove top border from plot
ax.spines['right'].set_visible(False)       # remove right border from plot

# set custom legend
legend_elements = [Patch(facecolor='tomato', label='1 node'),
                   Patch(facecolor='royalblue', label='2 nodes'),
                   Patch(facecolor='limegreen', label='4 nodes')]
                   #Patch(facecolor='white', label='10 bins', edgecolor='k'),
                   #Patch(facecolor='white', hatch="////", label='50 bins', edgecolor='k')]
ax.legend(handles=legend_elements, loc="upper center", fancybox=False, edgecolor='k', ncol=3, bbox_to_anchor=(0.5, -0.2))

for rect in rects:
    for bar in rect:
        height = bar.get_height()
        if (height < 0.001):
            str = '%0.2e' % height
        else:
            str = '%0.3f' % height
        plt.text(bar.get_x() + bar.get_width()/2.0, height, str, ha='center', va='bottom', rotation=25)


plt.show()
fig.savefig('p2_nilbase_a1.pdf', bbox_inches='tight')