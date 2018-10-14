import numpy as np
import matplotlib.pylab as plt
#import scipy.stats as stats
import pandas as pd

import sys
import statistics as stat

import useful_things as ut


case_name = "boxes";
num_names = ["a", "c", "e"];



import numpy as np
import matplotlib.pylab as plt
#import scipy.stats as stats
import pandas as pd

import sys
import statistics as stat

import useful_things as ut


case_name = "boxes";  # case
num_names = ["a", "c", "e"]; # names

for num_name in num_names:

    # MASK
    filename1 = 'C://Users//azer//OneDrive - University of Waterloo//__new_results__//10_' + case_name + '_' + num_name +'//dist_mask.csv';
    csv_data1 = pd.read_csv(filename1, sep=',',header=None);
    #print filename1;
    mask_vals = csv_data1.values[:,0];
    
    # SDF
    filename2 = 'C://Users//azer//OneDrive - University of Waterloo//__new_results__//10_' + case_name + '_' + num_name +'//dist_all.csv';
    csv_data2 = pd.read_csv(filename2, sep=',',header=None);
    #print filename2;
    sdf_vals = csv_data2.values[:,0];
    
    # size
    img_sz = np.sqrt(len(mask_vals));
    img_sz_int = int(img_sz);
    
    #########################
    # calculate statistics
    #########################    
    a_list = [];
    for x_iter in range(0, img_sz_int):
        for y_iter in range(0, img_sz_int):
            if sdf_vals[x_iter + y_iter * img_sz_int] >= 0:
                a_list.append(sdf_vals[x_iter + y_iter * img_sz_int]);
    
    # store non zero non negative sdf values
    str_vals_ggg = "non_neg_vals_" + case_name + "_" + num_name + " = a_list";
    exec(str_vals_ggg);
    
    print "min = ", min(a_list);
    print "max = ", max(a_list);
    print "mean = ", stat.mean(a_list);
    print "median = ", stat.median(a_list);
    
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
    r_gap = 0.1;
    r_vals = np.arange(0.2, 20 + 1, r_gap);
    
    ###################################
    # calculate sphere contact function
    ###################################
    str_vals = "scf_vals_" + case_name + "_" + num_name + " = []";
    exec(str_vals);
    
    
    # add area fraction
    str_vals_0 = "scf_vals_" + case_name + "_" + num_name + ".append(area_fraction)";
    exec(str_vals_0);
    #for r_iter in r_vals:
    #sys.stdout.write('.');
    
    scf_array = np.zeros(len(r_vals));
    #num_contact = 0;
    for x_iter in range(0, img_sz_int):
        for y_iter in range(0, img_sz_int):
            a_val = sdf_vals[x_iter + y_iter * img_sz_int];            
            idx = int(a_val / r_gap) + 1; # minus ???
            if idx <= 0 :
                continue;
            for z_iter in range(0, idx): #idx = idx - 1;
                scf_array[z_iter] = scf_array[z_iter] + 1;
            #if sdf_vals[x_iter + y_iter * img_sz_int] >= r_iter:
                #num_contact += 1.0;
    #scf = num_contact / num_sample;
    for i in range(0, len(scf_array)):
         scf = scf_array[i] / num_sample;
         str_vals = "scf_vals_" + case_name + "_" + num_name + ".append(scf)";
         exec(str_vals);
         
    #str_vals = "scf_vals_" + case_name + "_" + num_name + ".append(scf)";
    #exec(str_vals);
    
    print "done bro\n";