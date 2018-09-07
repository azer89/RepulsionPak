
import numpy as np

import useful_things as ut

if __name__ == "__main__":
    l = 9;
    sz = 3;
    for i_iter in range(0, l - 1):
        for j_iter in range(i_iter + 1, l):
            y1 = int(i_iter / sz);
            x1 = i_iter - (y1 * sz);
            y2 = int(j_iter / sz);
            x2 = j_iter - (y2 * sz);  
            #print i_iter
            #print j_iter
            #print "(", x1, ", ", y1, ")"; 
            #print "(", x2, ", ", y2, ")"; 
            #print "";
            d = ut.Dist((x1, y1), (x2, y2)); 
            print d;
            print ut.IsClose(2.0, d);
            print "";
             