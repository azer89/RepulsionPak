import numpy as np
import matplotlib.pylab as plt
#import scipy.stats as stats
import pandas as pd

import sys
import statistics as stat

import useful_things as ut

case_name = "balabolka";
num_names = ["01", "man2x"];
#num_names = ["01", "02", "03", "04", "05", "06", "07", "08", "09", "10"];
#num_names = ["a", "b", "c", "d", "e"];

for num_name in num_names:

    # MASK
    filename1 = 'C://Users//azer//OneDrive - University of Waterloo//__new_results__//09_' + case_name + '_' + num_name +'//dist_mask.csv';
    csv_data1 = pd.read_csv(filename1, sep=',',header=None);
    #print filename1;
    mask_vals = csv_data1.values[:,0];
    
    # SDF
    filename2 = 'C://Users//azer//OneDrive - University of Waterloo//__new_results__//09_' + case_name + '_' + num_name +'//dist_all.csv';
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
            if sdf_vals[x_iter + y_iter * img_sz_int] > 0:
                a_list.append(sdf_vals[x_iter + y_iter * img_sz_int]);
    
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
    r_vals = np.arange(0.2, 20 + 1, 0.2);
    
    ###################################
    # calculate sphere contact function
    ###################################
    str_vals = "scf_vals_" + case_name + "_" + num_name + " = []";
    exec(str_vals);
    
    # add area fraction
    str_vals_0 = "scf_vals_" + case_name + "_" + num_name + ".append(area_fraction)";
    exec(str_vals_0);
    #scf_vals = [];
    for r_iter in r_vals:
        sys.stdout.write('.');
        #print r_iter;
        num_contact = 0;
        for x_iter in range(0, img_sz_int):
            for y_iter in range(0, img_sz_int):
                if sdf_vals[x_iter + y_iter * img_sz_int] >= r_iter:
                    num_contact += 1.0;
        scf = num_contact / num_sample;
        str_vals = "scf_vals_" + case_name + "_" + num_name + ".append(scf)";
        exec(str_vals);
    
    print "";