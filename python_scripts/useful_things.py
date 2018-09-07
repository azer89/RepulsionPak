
"""
Useful things
"""
import numpy as np

####################
####################
####################
def Dot(u, v):
    return (u[0] * v[0]) + (u[1] * v[1]); 

####################
####################
####################
def Add(pt1, pt2):
    return (pt1[0] + pt2[0], pt1[1] + pt2[1]);

####################
####################
####################
def Substract(pt1, pt2):
    return (pt1[0] - pt2[0], pt1[1] - pt2[1]);

####################
####################
####################
def Norm(pt):
    l = np.sqrt(pt[0] * pt[0] + pt[1] * pt[1]);
    return (pt[0] / l, pt[1] / l);
    
####################
####################
####################
def Dist(pt1, pt2):
    pt = (pt2[0] - pt1[0], pt2[1] - pt1[1]);
    return np.sqrt(pt[0] * pt[0] + pt[1] * pt[1]);
    
####################
####################
####################
def DistSquared(pt1, pt2):
    pt = (pt2[0] - pt1[0], pt2[1] - pt1[1]);
    return pt[0] * pt[0] + pt[1] * pt[1];


####################
####################
####################
def InterpolateLine(pt1, pt2, num_pt):
    sample_pts = []
    dir_pt = Norm( (pt2[0] - pt1[0], pt2[1] - pt1[1]) );
    line_len = Dist(pt1, pt2);
    
    d_delta = line_len / num_pt;
    for i in frange(0, line_len , d_delta):
        sample_pts.append( (pt1[0] + dir_pt[0] * i, pt1[1] + dir_pt[1] * i) );
        
    return sample_pts;

####################
####################
####################
def frange(start, stop, step):
    x = start;
    while x < stop:
        yield x;
        x += step;

####################
####################
####################
def CreatePoly(num_side, center_pt, radius):
    poly = [];

    add_value = (np.pi * 2.0 / num_side);
    
    for i in frange(0, np.pi * 2.0 , add_value):
        x_pt = center_pt[0] + radius * np.sin(i);
        y_pt = center_pt[1] + radius * np.cos(i);
        poly.append((x_pt, y_pt));
        
    return poly;
    
####################
####################
####################
def DistanceToAFiniteLine(v, w, p):
    machine_eps = np.finfo(float).eps;
    l2 = DistSquared(v, w);
    
    if (l2 > -machine_eps and l2 < machine_eps):
        return Dist(p, v);
    
    t = Dot( Substract(p, v), Substract(w, v) ) / l2;
    if(t < 0):
        return Dist(p, v);
    elif (t > 1.0):
        return Dist(p, w); 
    
    wv = Substract(w, v);
    wv = (wv[0] * t, wv[1] * t);
    projection = Add(v, wv);
    
    return Dist(projection, p);          

####################
####################
####################     
#def IsClose(a, b, rel_tol=1e-09, abs_tol=0.0):
#    return abs(a-b) <= max(rel_tol * max(abs(a), abs(b)), abs_tol);
def IsClose(a, b):
    return abs(a-b) <= 1e-5;
    
    
    
