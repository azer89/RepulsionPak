import numpy as np
import matplotlib.pylab as plt
#import scipy.stats as stats
import pandas as pd

import sys
import statistics as stat

import useful_things as ut


num_folder = "13";
case_name = "pad";  # case
num_names = ["12"]; # names

for num_name in num_names:

    # MASK
    filename1 = 'C://Users//azer//OneDrive - University of Waterloo//__new_results__//' + num_folder + "_" + case_name + '_' + num_name +'//dist_mask.csv';
    csv_data1 = pd.read_csv(filename1, sep=',',header=None);
    #print filename1;
    mask_vals = csv_data1.values[:,0];
    
    # SDF
    filename2 = 'C://Users//azer//OneDrive - University of Waterloo//__new_results__//' + num_folder + "_" + case_name + '_' + num_name +'//dist_all.csv';
    csv_data2 = pd.read_csv(filename2, sep=',',header=None);
    #print filename2;
    sdf_vals = csv_data2.values[:,0];
    
    # size
    img_sz = np.sqrt(len(mask_vals));
    img_sz_int = int(img_sz);
    
    #########################
    # calculate statistics
    #########################       
    iter_step = 1; # be extra careful
    a_list = [];
    for x_iter in range(0, img_sz_int, iter_step):
        for y_iter in range(0, img_sz_int, iter_step):
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
    
    # mask val
    # <= 0 inside an element
    # > 0 negative space
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
    h_gap = 0.2;
    r_vals = np.arange(h_gap, 20, h_gap);
    
    ###################################
    # calculate sphere contact function
    ###################################
    str_vals = "scf_vals_" + case_name + "_" + num_name + " = []";
    exec(str_vals);
    
    
    
    # add area fraction
    str_vals_0 = "scf_vals_" + case_name + "_" + num_name + ".append(area_fraction)";
    exec(str_vals_0);
    for r_iter in r_vals:
        sys.stdout.write('.');
        num_contact = 0;
        for x_iter in range(0, img_sz_int):
            for y_iter in range(0, img_sz_int):
                if sdf_vals[x_iter + y_iter * img_sz_int] >= r_iter:
                    num_contact += 1.0;
        scf = num_contact / num_sample;
        str_vals = "scf_vals_" + case_name + "_" + num_name + ".append(scf)";
        exec(str_vals);
    
    print "";