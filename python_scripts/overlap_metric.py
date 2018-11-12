import numpy as np
import matplotlib.pylab as plt
#import scipy.stats as stats
import pandas as pd

import sys
import statistics as stat

import useful_things as ut

num_folder = "13";
case_name = "pad";  # case
num_names = ["man2x"]; # names

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
