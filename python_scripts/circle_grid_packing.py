"""
Circle-Grid packing
"""

import numpy as np
import matplotlib.pylab as plt
import scipy.stats as stats

import useful_things as ut

    

####################    
if __name__ == "__main__":
    print "CIRCLE-GRID"
    #variables
    num_side = 4;
    num_sample = 2000;
    center_pt = (0,0);
    skin_thickness = 10;

    circle_radius = 100;
    sq_radius = np.sqrt(2.0) * (circle_radius + skin_thickness);
    
    poly_2 = ut.CreatePoly(num_side, center_pt, sq_radius);      
    sample_pts_2 = ut.InterpolateLine(poly_2[0], poly_2[1], num_sample);
    
    ### calculating df
    df_vals = [];
    for a in range(len(sample_pts_2)):
        d = ut.Dist(center_pt, sample_pts_2[a]) - circle_radius;        
        #d = ut.Dist(sample_pts_1[a], sample_pts_2[a]) - skin_thickness; #dist to center
        df_vals.append(d);
    
    ### multiplication
    mul_val = 10; # 1 2 4 5 7 10
    df_all = np.zeros(len(df_vals) * mul_val);
    for a in range(len(df_all)):
        i = a % len(df_vals);
        df_all[a] = df_vals[i];
    
    ### drawing
    plt.figure(1);
    plt.clf();
    axes = plt.gca()
    axes.set_xlim([-250, 250])
    axes.set_ylim([-250, 250])    
    
    ### drawing
    poly_len = len(poly_2);
    for a in range(poly_len):
        b = a + 1;
        if(b == poly_len):
            b = 0;
        plt.plot([ poly_2[a][0], poly_2[b][0] ], [ poly_2[a][1], poly_2[b][1] ]); 
        
    
    plt.axes().set_aspect('equal')
    plt.show();

    #######################
    # HISTOGRAM
    #######################
    plt.figure(2);
    plt.clf();
    max_df = np.max(df_all);
    plt.hist(df_all, 100);
    plt.show(); 
    
    ###
    mean_df = np.mean(df_all);
    max_df = np.max(df_all);
    median_df = np.median(df_all);
    stdev_df = np.std(df_all);
    #mode_df = np.mode(df_all);
    kurt_df = stats.kurtosis(df_all, fisher=False, bias=False);
    skew_df = stats.skew(df_all, bias=False);
    
    ###
    ###
    num_std_1 = 0.0;
    num_std_2 = 0.0;  
    num_std_3 = 0.0; 
    mean_std_1 = mean_df + stdev_df;
    mean_std_2 = mean_df + stdev_df * 2.0;
    mean_std_3 = mean_df + stdev_df * 3.0;
    for a in range(len(df_all)):
        if(df_all[a] < mean_std_1):
            num_std_1 += 1.0;
        if(df_all[a] < mean_std_2):
            num_std_2 += 1.0;
        if(df_all[a] < mean_std_3):
            num_std_3 += 1.0;
    print "ratio mean + std * 1 is ", (num_std_1 / len(df_all));
    print "ratio mean + std * 2 is ", (num_std_2 / len(df_all));
    print "ratio mean + std * 3 is ", (num_std_3 / len(df_all));
    
    ### SORT
    df_sort = np.sort(df_all);
    idx_ten = len(df_sort) * 10 / 100;
    idx_fifty = len(df_sort) * 50 / 100;
    idx_ninety = len(df_sort) * 90 / 100;
    span_val = (df_sort[idx_ninety] - df_sort[idx_ten]) / df_sort[idx_fifty];
    print "d10 is ", df_sort[idx_ten]
    print "d50 is ", df_sort[idx_fifty]
    print "d90 is ", df_sort[idx_ninety]
    print "mean is ", mean_df
    print "max is ", max_df
    print "span is ", span_val    
    print "kurtosis is ", kurt_df    
    
    