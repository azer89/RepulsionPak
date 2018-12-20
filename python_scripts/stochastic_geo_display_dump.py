import numpy as np
import matplotlib.pylab as plt
#import scipy.stats as stats
#import pandas as pd

import useful_things as ut

##################
# h_gap and r_vals
##################
h_gap = 0.2;
r_vals = np.arange(h_gap, 40, h_gap);
r_vals = np.insert(r_vals, 0, 0);

########
# figure
########
fig1 = plt.figure(1);
fig1.patch.set_facecolor('white');
plt.clf();
ax = plt.gca();
ax.set_xlim(0, 24);


# 1000
num_folder = "16";
case_name = "dump_1000";  # case
num_names = ["1", "2", "3", "4", "5", "6", "7", "8", "9", "10"];

for num_name in num_names:
    str_cmd = "data_array = " "scf_vals_" + case_name + "_" + num_name;
    print str_cmd;
    exec(str_cmd);
    
    data_array = [x for x in data_array if x != 0];
    idx_array = r_vals[:len(data_array)];
    
    plt.plot(idx_array, np.asarray(data_array), 'r', linewidth=1);


# 250
case_name = "dump_250";  # case
for num_name in num_names:
    str_cmd = "data_array = " "scf_vals_" + case_name + "_" + num_name;
    print str_cmd;
    exec(str_cmd);
    
    data_array = [x for x in data_array if x != 0];
    idx_array = r_vals[:len(data_array)];
    
    plt.plot(idx_array, np.asarray(data_array), 'g', linewidth=1);


case_name = "dump_10";  # case
for num_name in num_names:
    str_cmd = "data_array = " "scf_vals_" + case_name + "_" + num_name;
    print str_cmd;
    exec(str_cmd);
    
    data_array = [x for x in data_array if x != 0];
    idx_array = r_vals[:len(data_array)];
    
    plt.plot(idx_array, np.asarray(data_array), 'b', linewidth=1);


plt.title(r"SCPs of all");
plt.show();
