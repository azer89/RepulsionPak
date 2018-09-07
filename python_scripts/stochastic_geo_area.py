
import numpy as np
import matplotlib.pylab as plt
import scipy.stats as stats
import pandas as pd

import useful_things as ut

# MASK
#C://Users//azer//workspace//Images//PhysicsPak_Snapshots_01//
#C://Users//azer//OneDrive//__results__//07_cat_fish_good//PhysicsPak_Snapshots_01//
filename1 = 'C://Users//azer//OneDrive//__results__//19_dump_10_01//PhysicsPak_Snapshots_01//dist_mask.csv';
csv_data1 = pd.read_csv(filename1, sep=',',header=None);
print filename1;
mask_vals = csv_data1.values[:,0];

#SDF
filename2 = 'C://Users//azer//OneDrive//__results__//19_dump_10_01//PhysicsPak_Snapshots_01//dist_all.csv';
csv_data2 = pd.read_csv(filename2, sep=',',header=None);
print filename2;
sdf_vals = csv_data2.values[:,0];


img_sz = np.sqrt(len(mask_vals));
img_sz_int = int(img_sz);            
            


fig1 = plt.figure(1);
fig1.patch.set_facecolor('white')
plt.clf();

# draw something here
ax = plt.gca();



for x_iter in range(0, img_sz_int, 10):
    for y_iter in range(0, img_sz_int, 10):
        if mask_vals[x_iter + y_iter * img_sz_int] > 0:
            circle1 = plt.Circle((x_iter, y_iter), 1, color='r');
            ax.add_artist(circle1);
            
plt.title(r"mask_vals");

plt.axes().set_aspect('equal');
plt.xlim(0, img_sz);
plt.ylim(0, img_sz);

plt.show();
            

fig2 = plt.figure(2);
fig2.patch.set_facecolor('white')
plt.clf();

# draw something here
ax = plt.gca();            

      
for x_iter in range(0, img_sz_int, 10):
    for y_iter in range(0, img_sz_int, 10):
        if sdf_vals[x_iter + y_iter * img_sz_int] > 0:
            circle1 = plt.Circle((x_iter, y_iter), 1, color='g');
            ax.add_artist(circle1);


plt.title(r"sdf_vals");

plt.axes().set_aspect('equal');
plt.xlim(0, img_sz);
plt.ylim(0, img_sz);

plt.show();

