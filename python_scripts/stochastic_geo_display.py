import numpy as np
import matplotlib.pylab as plt
#import scipy.stats as stats
import pandas as pd

import useful_things as ut

h_gap = 0.2;
r_vals = np.arange(h_gap, 20, h_gap);
r_vals = np.insert(r_vals, 0, 0);


fig3 = plt.figure(1);
fig3.patch.set_facecolor('white');
plt.clf();
ax = plt.gca();

green_array = scf_vals_pad_03;
red_array   = scf_vals_pad_man2x;

#blue_array = [x for x in blue_array if x != 0]
green_array = [x for x in green_array if x != 0];
red_array = [x for x in red_array if x != 0];

green_idx = r_vals[:len(green_array)];
red_idx = r_vals[:len(red_array)];

plt.plot(red_idx, np.asarray(red_array), 'r', linewidth=1);
plt.plot(green_idx, np.asarray(green_array), 'g', linewidth=1);

plt.plot([5.746, 5.746], [0, 0.5], 'k--')

#plt.plot(r_vals, np.asarray(scf_vals_boxes_e), 'b', linewidth=1);
#plt.plot(r_vals, np.asarray(scf_vals_balabolka_man8x), 'r', linewidth=1.0);
#plt.plot(r_vals, np.asarray(scf_vals_pad_03), 'g', linewidth=1.0);

# logarithmic y-axis
#plt.yscale('log');
#plt.gca().minorticks_off();
         

plt.title(r"Our result SCP");
plt.show();
#plt.plot(r_vals, np.asarray(scf_vals_pad_01), '#56D65F', linewidth=1);         
#plt.plot(r_vals, np.asarray(scf_vals_pad_07_7), '#6755D8', linewidth=1);
#plt.plot(r_vals, np.asarray(scf_vals_pad_03), '#6755D8', linewidth=1);
#plt.plot(r_vals, np.asarray(scf_vals_pad_04), '#FF6700', linewidth=1);
#plt.plot(r_vals, np.asarray(scf_vals_pad_05), '#178C86', linewidth=1);
#plt.plot(r_vals, np.asarray(scf_vals_balabolka_06), 'g', linewidth=0.5);
#plt.plot(r_vals, np.asarray(scf_vals_balabolka_07), 'g', linewidth=0.5);
#plt.plot(r_vals, np.asarray(scf_vals_balabolka_08), 'g', linewidth=0.5);
#plt.plot(r_vals, np.asarray(scf_vals_balabolka_09), 'g', linewidth=0.5);
#plt.plot(r_vals, np.asarray(scf_vals_balabolka_10), 'g', linewidth=0.5);
#plt.plot(r_vals, np.asarray(scf_vals_balabolka_11), 'g', linewidth=0.5);
#scf_vals_pad_11
#plt.plot(r_vals, np.asarray(scf_vals_pad_11), 'b', linewidth=1.0);




'''
plt.plot(r_vals, np.asarray(scf_vals_boxes_a), 'r', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_boxes_b), 'g', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_boxes_c), 'b', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_boxes_d), 'k', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_boxes_e), '#db82ff', linewidth=1.0);
'''

'''
plt.plot(r_vals, np.asarray(scf_vals_dump_10_01), 'b', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_10_02), 'b', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_10_03), 'b', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_10_04), 'b', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_10_05), 'b', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_10_06), 'b', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_10_07), 'b', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_10_08), 'b', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_10_09), 'b', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_10_10), 'b', linewidth=1.0);

plt.plot(r_vals, np.asarray(scf_vals_dump_250_01), 'g', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_250_02), 'g', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_250_03), 'g', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_250_04), 'g', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_250_05), 'g', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_250_06), 'g', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_250_07), 'g', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_250_08), 'g', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_250_09), 'g', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_250_10), 'g', linewidth=1.0);
'''

'''
plt.plot(r_vals, np.asarray(scf_vals_animal_20_1), 'g');
plt.plot(r_vals, np.asarray(scf_vals_animal_20_2), 'g');
plt.plot(r_vals, np.asarray(scf_vals_animal_20_3), 'g');
plt.plot(r_vals, np.asarray(scf_vals_animal_20_4), 'g');
plt.plot(r_vals, np.asarray(scf_vals_animal_20_5), 'g');
'''

"""
plt.plot(r_vals, np.asarray(scf_vals_animal_30_1), 'b');
plt.plot(r_vals, np.asarray(scf_vals_animal_30_2), 'b');
plt.plot(r_vals, np.asarray(scf_vals_animal_30_3), 'b');
plt.plot(r_vals, np.asarray(scf_vals_animal_30_4), 'b');
plt.plot(r_vals, np.asarray(scf_vals_animal_30_5), 'b');
"""

'''
plt.plot(r_vals, np.asarray(scf_vals_animal_40_1), 'c');
plt.plot(r_vals, np.asarray(scf_vals_animal_40_2), 'c');
plt.plot(r_vals, np.asarray(scf_vals_animal_40_3), 'c');
plt.plot(r_vals, np.asarray(scf_vals_animal_40_4), 'c');
plt.plot(r_vals, np.asarray(scf_vals_animal_40_5), 'c');
'''

"""
plt.plot(r_vals, np.asarray(scf_vals_animal_50_1), 'm');
plt.plot(r_vals, np.asarray(scf_vals_animal_50_2), 'm');
plt.plot(r_vals, np.asarray(scf_vals_animal_50_3), 'm');
plt.plot(r_vals, np.asarray(scf_vals_animal_50_4), 'm');
plt.plot(r_vals, np.asarray(scf_vals_animal_50_5), 'm');
"""

'''
##ff8000
plt.plot(r_vals, np.asarray(scf_vals_animal_60_1), '#ff8000');
plt.plot(r_vals, np.asarray(scf_vals_animal_60_2), '#ff8000');
plt.plot(r_vals, np.asarray(scf_vals_animal_60_3), '#ff8000');
plt.plot(r_vals, np.asarray(scf_vals_animal_60_4), '#ff8000');
plt.plot(r_vals, np.asarray(scf_vals_animal_60_5), '#ff8000');
'''

"""
#90ee90
plt.plot(r_vals, np.asarray(scf_vals_animal_70_1), '#90ee90');
plt.plot(r_vals, np.asarray(scf_vals_animal_70_2), '#90ee90');
plt.plot(r_vals, np.asarray(scf_vals_animal_70_3), '#90ee90');
plt.plot(r_vals, np.asarray(scf_vals_animal_70_4), '#90ee90');
plt.plot(r_vals, np.asarray(scf_vals_animal_70_5), '#90ee90');
"""

#804000
'''
plt.plot(r_vals, np.asarray(scf_vals_animal_80_1), '#90ee90');
plt.plot(r_vals, np.asarray(scf_vals_animal_80_2), '#90ee90');
plt.plot(r_vals, np.asarray(scf_vals_animal_80_3), '#90ee90');
plt.plot(r_vals, np.asarray(scf_vals_animal_80_4), '#90ee90');
plt.plot(r_vals, np.asarray(scf_vals_animal_80_5), '#90ee90');
#plt.plot(r_vals, np.asarray(scf_vals_star_2), 'r');
#plt.plot(r_vals, np.asarray(scf_vals_star_1), 'b');
'''

"""
plt.plot(r_vals, np.asarray(scf_vals_animal_90_1), 'c');
plt.plot(r_vals, np.asarray(scf_vals_animal_90_2), 'c');
plt.plot(r_vals, np.asarray(scf_vals_animal_90_3), 'c');
plt.plot(r_vals, np.asarray(scf_vals_animal_90_4), 'c');
plt.plot(r_vals, np.asarray(scf_vals_animal_90_5), 'c');
"""

"""
#ffC0cb // pink
plt.plot(r_vals, np.asarray(scf_vals_animal_110_1), '#ffC0cb');
plt.plot(r_vals, np.asarray(scf_vals_animal_110_2), '#ffC0cb');
plt.plot(r_vals, np.asarray(scf_vals_animal_110_3), '#ffC0cb');
plt.plot(r_vals, np.asarray(scf_vals_animal_110_4), '#ffC0cb');
plt.plot(r_vals, np.asarray(scf_vals_animal_110_5), '#ffC0cb');
"""

#ffb302 // orange
"""
plt.plot(r_vals, np.asarray(scf_vals_animal_130_1), '#ffb302');
plt.plot(r_vals, np.asarray(scf_vals_animal_130_2), '#ffb302');
plt.plot(r_vals, np.asarray(scf_vals_animal_130_3), '#ffb302');
plt.plot(r_vals, np.asarray(scf_vals_animal_130_4), '#ffb302');
plt.plot(r_vals, np.asarray(scf_vals_animal_130_5), '#ffb302');
"""

#db82ff light purple
"""
plt.plot(r_vals, np.asarray(scf_vals_animal_170_1), '#db82ff');
plt.plot(r_vals, np.asarray(scf_vals_animal_170_2), '#db82ff');
plt.plot(r_vals, np.asarray(scf_vals_animal_170_3), '#db82ff');
plt.plot(r_vals, np.asarray(scf_vals_animal_170_4), '#db82ff');
plt.plot(r_vals, np.asarray(scf_vals_animal_170_5), '#db82ff');
"""

#ff965e
"""
plt.plot(r_vals, np.asarray(scf_vals_animal_210_1), '#969696');
plt.plot(r_vals, np.asarray(scf_vals_animal_210_2), '#969696');
plt.plot(r_vals, np.asarray(scf_vals_animal_210_3), '#969696');
plt.plot(r_vals, np.asarray(scf_vals_animal_210_4), '#969696');
plt.plot(r_vals, np.asarray(scf_vals_animal_210_5), '#969696');
"""

#fffa00
"""
plt.plot(r_vals, np.asarray(scf_vals_animal_250_1), '#fffa00');
plt.plot(r_vals, np.asarray(scf_vals_animal_250_2), '#fffa00');
plt.plot(r_vals, np.asarray(scf_vals_animal_250_3), '#fffa00');
plt.plot(r_vals, np.asarray(scf_vals_animal_250_4), '#fffa00');
plt.plot(r_vals, np.asarray(scf_vals_animal_250_5), '#fffa00');
"""

#00ff19
"""
plt.plot(r_vals, np.asarray(scf_vals_animal_290_1), '#00ff19');
plt.plot(r_vals, np.asarray(scf_vals_animal_290_2), '#00ff19');
plt.plot(r_vals, np.asarray(scf_vals_animal_290_3), '#00ff19');
plt.plot(r_vals, np.asarray(scf_vals_animal_290_4), '#00ff19');
plt.plot(r_vals, np.asarray(scf_vals_animal_290_5), '#00ff19');
"""

#4935ff blue
"""
plt.plot(r_vals, np.asarray(scf_vals_dump_500_01), 'r', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_500_02), 'r', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_500_03), 'r', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_500_04), 'r', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_500_05), 'r', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_500_06), 'r', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_500_07), 'r', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_500_08), 'r', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_500_09), 'r', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_500_10), 'r', linewidth=1.0);
"""

'''
plt.plot(r_vals, np.asarray(scf_vals_dump_1000_01), 'r', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_1000_02), 'r', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_1000_03), 'r', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_1000_04), 'r', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_1000_05), 'r', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_1000_06), 'r', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_1000_07), 'r', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_1000_08), 'r', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_1000_09), 'r', linewidth=1.0);
plt.plot(r_vals, np.asarray(scf_vals_dump_1000_10), 'r', linewidth=1.0);
'''
