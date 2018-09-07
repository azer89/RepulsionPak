from __future__ import division
import numpy as np
import matplotlib.pylab as plt
plt.ion()

x = np.arange(-500, 500, 0.1)

### outside function f(x)
alpha = 1
f = -alpha * x

###
plt.figure(1)
plt.clf()
plt.title("outside function f(x)")
plt.plot(x, f, color='blue')
plt.show()

###
# inside function g(x)
#phi = np.max(x)
phi = 353 * 0.01
t = -1.0 * (x * x) / ((phi) * (phi))
g = np.exp(t)

plt.figure(2)
plt.clf()
plt.title("inside function g(x)")
plt.plot(x, g, color='blue')
plt.show()