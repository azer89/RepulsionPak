
import numpy as np
import matplotlib.pylab as plt
#import scipy.stats as stats
import pandas as pd

import useful_things as ut

# MASK
#C://Users//azer//workspace//Images//PhysicsPak_Snapshots_01//
#C://Users//azer//OneDrive//__results__//07_cat_fish_good//PhysicsPak_Snapshots_01//
filename1 = 'C://Users//azer//OneDrive//__results__//08_star_2//PhysicsPak_Snapshots_01//dist_mask.csv';
csv_data1 = pd.read_csv(filename1, sep=',',header=None);
print filename1;
mask_vals = csv_data1.values[:,0];

#SDF
filename2 = 'C://Users//azer//OneDrive//__results__//08_star_2//PhysicsPak_Snapshots_01//dist_all.csv';
csv_data2 = pd.read_csv(filename2, sep=',',header=None);
print filename2;
sdf_vals = csv_data2.values[:,0];

img_sz = np.sqrt(len(mask_vals));
img_sz_int = int(img_sz);

r_vals = range(1, 201);
###r_vals_odd = range(1, 21, 2);
###r_vals_even = range(2, 21, 2);

### Calculate
c_vals = [];
###n_array = [];
###for r_iter in r_vals:
###    c_vals.append(0);
###    n_array.append(0);

### 
#num_sample = img_sz * img_sz;
#len_mask = len(mask_vals);
#num_sample = len_mask * (len_mask - 1) / 2.0 ;

for r_iter in r_vals:
    print r_iter;
    num_contact = 0;
    num_sample = 0;
    for x_iter in range(r_iter, img_sz_int - r_iter):       
        for y_iter in range(r_iter, img_sz_int - r_iter):
            #num_sample += 2.0;
            
            #horizontal   
            x1 = x_iter - r_iter;
            x2 = x_iter + r_iter;
            if mask_vals[x1 + y_iter * img_sz_int] > 0 and mask_vals[x2 + y_iter * img_sz_int] > 0:
                num_sample += 1.0;
                is_empty_1 = sdf_vals[x1 + y_iter * img_sz_int] >= 0;
                is_empty_2 = sdf_vals[x2 + y_iter * img_sz_int] >= 0;
                if  is_empty_1 and is_empty_2:
                    num_contact += 1.0;
            
            #vertical
            y1 = y_iter - r_iter;
            y2 = y_iter + r_iter; 
            if mask_vals[x_iter + y1 * img_sz_int] > 0 and mask_vals[x_iter + y2 * img_sz_int] > 0:
                num_sample += 1.0;
                is_empty_1 =  sdf_vals[x_iter + y1 * img_sz_int] >= 0;
                is_empty_2 =  sdf_vals[x_iter + y2 * img_sz_int] >= 0;
                if  is_empty_1 and is_empty_2:
                    num_contact += 1.0;
        
    c = num_contact / num_sample;
    c_vals.append(c);


### even


fig1 = plt.figure(1);
fig1.patch.set_facecolor('white')
plt.clf();
ax = plt.gca();

plt.plot(r_vals, np.asarray(c_vals), 'r');
#plt.plot(r_vals, np.asarray(c_vals_1), 'b');

plt.title(r"Covariance");
plt.show();

plt.savefig("cov.svg")