import numpy as np
import matplotlib.pylab as plt
#import scipy.stats as stats
import pandas as pd

import sys
import statistics as stat

import useful_things as ut

d_gap      = 5.75;
num_folder = "";
case_name  = "balabolka";  # case
num_names  = ["01", "man2x"]; # names


#r_vals = np.arange(0.0, 20, 0.2);

for num_name in num_names:
    ### 5
    filename5 = 'C://Users//azer//OneDrive - University of Waterloo//dump_results_06//' + num_folder + "" + case_name + '_' + num_name +'//dist_4.csv';
    print filename5;
    csv_data5 = pd.read_csv(filename5, sep=',',header=None);
    neg_area_vals_5 = csv_data5.values[:,0];
    
    negative_list_wrong = [];
    for i in range(0, len(neg_area_vals_5)):
        negative_list_wrong.append(neg_area_vals_5[i]);
        
    str_vals4 = "scf_vals_" + case_name + "_" + num_name + "= negative_list_wrong";
    exec(str_vals4);
    

"""
###########################################################################
fig3 = plt.figure(1);
fig3.patch.set_facecolor('white')
plt.clf();
ax = plt.gca();

plt.ylabel("Q_s(r)");
plt.xlabel("radius");

green_array = np.asarray(scp_vals_boxes_layered_a);
red_array = np.asarray(scp_vals_boxes_layered_c); 

green_array = [x for x in green_array if x > 1e-6];
red_array = [x for x in red_array if x > 1e-6];

green_idx = r_vals[:len(green_array)];
red_idx = r_vals[:len(red_array)]; 


plt.plot(red_idx, red_array, 'r', linewidth=2);
plt.plot(green_idx, green_array, 'g', linewidth=1);

#plt.plot([d_gap, d_gap], [0, 0.5], 'k--')

#plt.plot([d_gap, d_gap], [70000, 140000], 'k--')

plt.title(r"SCP with offset");
plt.show();
"""


