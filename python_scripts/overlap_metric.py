import numpy as np
import matplotlib.pylab as plt
#import scipy.stats as stats
import pandas as pd

import sys
import statistics as stat

import useful_things as ut

d_gap = 5.75;
num_folder = "13";
case_name = "pad";  # case
num_names = ["man2x", "01"]; # names
container_area = 146721;

r_vals = np.arange(0.0, 20, 0.1);

for num_name in num_names:

    # MASK
    filename2 = 'C://Users//azer//OneDrive - University of Waterloo//__new_results__//' + num_folder + "_" + case_name + '_' + num_name +'//dist_2.csv';
    csv_data2 = pd.read_csv(filename2, sep=',',header=None);
    #print filename1;
    area_vals_2 = csv_data2.values[:,0];
    
    # SDF
    filename3 = 'C://Users//azer//OneDrive - University of Waterloo//__new_results__//' + num_folder + "_" + case_name + '_' + num_name +'//dist_3.csv';
    csv_data3 = pd.read_csv(filename3, sep=',',header=None);
    #print filename2;
    area_vals_3 = csv_data3.values[:,0];
    
    overlap_list = [];
    for i in range(0, len(area_vals_2)):
        overlap_list.append(area_vals_2[i] - area_vals_3[i]);
        
    negative_list = [];
    for i in range(0, len(area_vals_2)):
        negative_list.append((container_area - area_vals_3[i]) / container_area );
        
    str_vals3 = "negative_vals_" + case_name + "_" + num_name + " = negative_list";
    exec(str_vals3);
        
    str_vals2 = "area_vals_" + case_name + "_" + num_name + " = area_vals_3";
    exec(str_vals2);
    
    str_vals = "overlap_vals_" + case_name + "_" + num_name + " = overlap_list";
    exec(str_vals);
    
###########################################################################
fig3 = plt.figure(1);
fig3.patch.set_facecolor('white')
plt.clf();
ax = plt.gca();

plt.ylabel("overlap area");
plt.xlabel("polygon offset");

#input
green_array = np.asarray(overlap_vals_pad_01);
red_array = np.asarray(overlap_vals_pad_man2x);  

plt.plot(r_vals, np.asarray(green_array), 'g', linewidth=1);
plt.plot(r_vals, np.asarray(red_array), 'r', linewidth=1);

plt.plot([d_gap, d_gap], [0, 140000], 'k--')

plt.title(r"Overlap. PAD (red) vs RepulsionPak (green)");
plt.show();


'''
###########################################################################
fig2 = plt.figure(2);
fig2.patch.set_facecolor('white')
plt.clf();
ax = plt.gca();

plt.ylabel("Offset area of positive space");
plt.xlabel("polygon offset");

green_array = np.asarray(area_vals_pad_01);
red_array = np.asarray(area_vals_pad_man2x);  

green_array = [x for x in green_array if x != 0];
red_array = [x for x in red_array if x != 0];

green_idx = r_vals[:len(green_array)];
red_idx = r_vals[:len(red_array)]; 

plt.plot(red_idx, np.asarray(red_array), 'r', linewidth=1);
plt.plot(green_idx, np.asarray(green_array), 'g', linewidth=1);

plt.plot([d_gap, d_gap], [70000, 140000], 'k--')

plt.title(r"Offset area of positive space. PAD (red) vs RepulsionPak (green)");
plt.show();
'''


###########################################################################
fig3 = plt.figure(3);
fig3.patch.set_facecolor('white')
plt.clf();
ax = plt.gca();

plt.ylabel("Q_s(r)");
plt.xlabel("radius");

green_array = np.asarray(negative_vals_pad_01);
red_array = np.asarray(negative_vals_pad_man2x); 

green_array = [x for x in green_array if x > 1e-6];
red_array = [x for x in red_array if x > 1e-6];

green_idx = r_vals[:len(green_array)];
red_idx = r_vals[:len(red_array)]; 


plt.plot(red_idx, red_array, 'r', linewidth=1);
plt.plot(green_idx, green_array, 'g', linewidth=1);

plt.plot([d_gap, d_gap], [0, 0.5], 'k--')

#plt.plot([d_gap, d_gap], [70000, 140000], 'k--')

plt.title(r"SCP");
plt.show();


