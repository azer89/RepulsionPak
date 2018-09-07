
import numpy as np
import matplotlib.pylab as plt
import scipy.stats as stats
import pandas as pd

import useful_things as ut

case_name = "balabolka";
num_name = "09";

# MASK
filename1 = 'C://Users//azer//OneDrive//__results__//32_' + case_name + '_' + num_name +'//dist_mask.csv';
csv_data1 = pd.read_csv(filename1, sep=',',header=None);
print filename1;
mask_vals = csv_data1.values[:,0];

# SDF
filename2 = 'C://Users//azer//OneDrive//__results__//32_' + case_name + '_' + num_name +'//dist_all.csv';
csv_data2 = pd.read_csv(filename2, sep=',',header=None);
print filename2;
sdf_vals = csv_data2.values[:,0];

# size
img_sz = np.sqrt(len(mask_vals));
img_sz_int = int(img_sz);


#########################
# calculate area_fraction
#########################
area_counter = 0;
num_sample = 0;
for x_iter in range(0, img_sz_int):
    for y_iter in range(0, img_sz_int):
        if mask_vals[x_iter + y_iter * img_sz_int] > 0:
            num_sample += 1.0;
        if sdf_vals[x_iter + y_iter * img_sz_int] > 0:
            area_counter += 1.0;
            
area_fraction = area_counter / num_sample;
print "area_fraction = ", area_fraction;

#######
# stuff
#######
r_vals = np.arange(1, 30 + 1, 0.2);

###################################
# calculate sphere contact function
###################################
"""
foo = "lol";
str_val = foo + " = 1";
exec(str_val);
print lol;
"""
str_vals = "scf_vals_" + case_name + "_" + num_name + " = []";
exec(str_vals);
#scf_vals = [];
for r_iter in r_vals:
    print r_iter;
    num_contact = 0;
    for x_iter in range(0, img_sz_int):
        for y_iter in range(0, img_sz_int):
            if sdf_vals[x_iter + y_iter * img_sz_int] >= r_iter:
                num_contact += 1.0;
    scf = num_contact / num_sample;
    str_vals = "scf_vals_" + case_name + "_" + num_name + ".append(scf)";
    exec(str_vals);
    #scf_vals.append(scf);

            
##### fig1 = plt.figure(1);
##### fig1.patch.set_facecolor('white')
##### plt.clf();

# draw something here
##### ax = plt.gca();

"""
for x_iter in range(0, img_sz, 10):
    for y_iter in range(0, img_sz, 10):
        if mask_vals[x_iter + y_iter * img_sz] > 0:
            circle1 = plt.Circle((x_iter, y_iter), 1, color='r');
            ax.add_artist(circle1);
"""

"""            
for x_iter in range(0, img_sz, 10):
    for y_iter in range(0, img_sz, 10):
        if sdf_vals[x_iter + y_iter * img_sz] > 0:
            circle1 = plt.Circle((x_iter, y_iter), 1, color='g');
            ax.add_artist(circle1);
"""

#plt.xlim(0, img_sz);
#plt.ylim(0, img_sz);

#str_vals = "scf_vals_" + case_name + "_" + num_name + " = []";
#func_vals = "plt.plot(r_vals, np.asarray(" + str_vals + "), 'r')";
#exec(func_vals);

#plt.plot(r_vals, np.asarray(scf_vals), 'r');
#plt.plot(r_vals, np.asarray(c_vals), 'b');
#plt.plot(r_vals, np.asarray(scf_vals_uni), 'y');
#plt.plot(r_vals, np.asarray(scf_vals_good), 'b');
#plt.plot(r_vals, np.asarray(scf_vals_bad), 'r');

##### plt.title(r"Spherical contact");

#plt.axes().set_aspect('equal');
##### plt.show();

