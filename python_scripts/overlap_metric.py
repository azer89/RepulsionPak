import numpy as np
import matplotlib.pylab as plt
#import scipy.stats as stats
import pandas as pd

import sys
import statistics as stat

import useful_things as ut

#d_gap = 5.75;
num_folder = "";
case_name = "pad";  # case
num_names = ["02", "man2x"];
container_area = 146721.0;

r_vals = np.arange(0.0, 20, 0.1);

for num_name in num_names:

    # 2
    filename2 = 'C://Users//azer//OneDrive - University of Waterloo//dump_results_06//' + num_folder + "" + case_name + '_' + num_name +'//dist_2.csv';
    csv_data2 = pd.read_csv(filename2, sep=',',header=None);
    area_vals_2 = csv_data2.values[:,0];
    
    # 3
    filename3 = 'C://Users//azer//OneDrive - University of Waterloo//dump_results_06//' + num_folder + "" + case_name + '_' + num_name +'//dist_3.csv';
    csv_data3 = pd.read_csv(filename3, sep=',',header=None);
    area_vals_3 = csv_data3.values[:,0];
    scp_vals = [];
    for i in range(0, len(area_vals_3)):
        scp_vals.append(1.0 - (area_vals_3[i] / container_area) );
        
    str_vals = "scf_vals_" + case_name + "_" + num_name + " = scp_vals";
    exec(str_vals);
    
    # 4
    #filename4 = 'C://Users//azer//OneDrive - University of Waterloo//__new_results__//' + num_folder + "_" + case_name + '_' + num_name +'//dist_4.csv';
    #csv_data4 = pd.read_csv(filename4, sep=',',header=None);
    #area_vals_4 = csv_data4.values[:,0];
    
     # 5
    #filename5 = 'C://Users//azer//OneDrive - University of Waterloo//__new_results__//' + num_folder + "_" + case_name + '_' + num_name +'//dist_5_offset_container_d_gap.csv';
    #print filename5;
    #csv_data5 = pd.read_csv(filename5, sep=',',header=None);
    #neg_area_vals_5 = csv_data5.values[:,0];
    
    overlap_list = [];
    for i in range(0, len(area_vals_2)):
        overlap_list.append(area_vals_2[i] - area_vals_3[i]);
        
    #negative_list = [];
    #negative_list_wrong = [];
    #for i in range(0, len(area_vals_2)):
        #negative_list_wrong.append((container_area - area_vals_3[i]) / container_area );
    #    negative_list_wrong.append(neg_area_vals_5[i]);
    #    negative_list.append(area_vals_4[i] / container_area);
        
    
    
    #str_vals4 = "negative_wrong_vals_" + case_name + "_" + num_name + " = negative_list_wrong";
    #exec(str_vals4);
    
    #str_vals3 = "negative_vals_" + case_name + "_" + num_name + " = negative_list";
    #exec(str_vals3);
        
    #str_vals2 = "area_vals_" + case_name + "_" + num_name + " = area_vals_3";
    #exec(str_vals2);
    
    str_vals = "overlap_vals_" + case_name + "_" + num_name + " = overlap_list";
    exec(str_vals);

###########################################################################


fig3 = plt.figure(1);
fig3.patch.set_facecolor('white')
plt.clf();
ax = plt.gca();
ax.set_xlim(0, 15.5);
ax.set_ylim(-0.01, 0.7);

plt.ylabel("overlap area");
plt.xlabel("polygon offset");

#input
green_array = np.asarray(overlap_vals_pad_02) / container_area;
red_array = np.asarray(overlap_vals_pad_man2x) / container_area;  

plt.plot(r_vals, np.asarray(green_array), 'g', linewidth=1);
plt.plot(r_vals, np.asarray(red_array), 'r', linewidth=1);

#plt.plot([d_gap, d_gap], [0, 0.7], 'k')

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
"""
fig3 = plt.figure(3);
fig3.patch.set_facecolor('white')
plt.clf();
ax = plt.gca();

plt.ylabel("Q_s(r)");
plt.xlabel("radius");

################

green_array = np.asarray(negative_wrong_vals_pad_01);
red_array = np.asarray(negative_wrong_vals_pad_man2x); 

green_array = [x for x in green_array if x > 1e-6];
red_array = [x for x in red_array if x > 1e-6];

green_idx = r_vals[:len(green_array)];
red_idx = r_vals[:len(red_array)]; 


plt.plot(red_idx, red_array, '#FFAAAA', linewidth=2);
plt.plot(green_idx, green_array, '#AAFFAA', linewidth=2);

################

green_array2 = np.asarray(negative_vals_pad_01);
red_array2 = np.asarray(negative_vals_pad_man2x); 

green_array2 = [x for x in green_array2 if x > 1e-6];
red_array2   = [x for x in red_array2 if x > 1e-6];

green_idx2 = r_vals[:len(green_array2)];
red_idx2   = r_vals[:len(red_array2)]; 


plt.plot(red_idx2, red_array2, 'r', linewidth=1);
plt.plot(green_idx2, green_array2, 'g', linewidth=1);

plt.plot([d_gap, d_gap], [0, 0.5], 'k--')




#plt.plot([d_gap, d_gap], [70000, 140000], 'k--')

plt.title(r"SCP");
plt.show();

"""
###########################################################################
"""
fig3 = plt.figure(4);
fig3.patch.set_facecolor('white')
plt.clf();
ax = plt.gca();

plt.ylabel("Q_s(r)");
plt.xlabel("radius");

green_array = np.asarray(negative_wrong_vals_pad_01);
red_array = np.asarray(negative_wrong_vals_pad_man2x); 

green_array = [x for x in green_array if x > 1e-6];
red_array = [x for x in red_array if x > 1e-6];

green_idx = r_vals[:len(green_array)];
red_idx = r_vals[:len(red_array)]; 


plt.plot(red_idx, red_array, 'r', linewidth=2);
plt.plot(green_idx, green_array, 'g', linewidth=1);

plt.plot([d_gap, d_gap], [0, 0.5], 'k--')

#plt.plot([d_gap, d_gap], [70000, 140000], 'k--')

plt.title(r"SCP experiment");
plt.show();
"""


