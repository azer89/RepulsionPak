import numpy as np
import matplotlib.pylab as plt

h_step = 0.1; # gap
max_val = 20;
r_vals = np.arange(0.0, max_val, h_step );

log_vals = np.exp(-1 * r_vals * r_vals / 40000);

figfig = plt.figure(1);
figfig.patch.set_facecolor('white');
plt.clf();
ax = plt.gca();

plt.plot(r_vals, log_vals, 'r', linewidth=2);

plt.title(r"Test");
plt.show();