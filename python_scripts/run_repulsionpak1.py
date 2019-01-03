# -*- coding: utf-8 -*-
"""
Created on Thu Dec 13 11:35:25 2018

@author: azer
"""

import os
import shutil
import stat

# https://stackoverflow.com/questions/1868714/how-do-i-copy-an-entire-directory-of-files-into-an-existing-directory-using-pyth
def copytree(src, dst, symlinks = False, ignore = None):
    if not os.path.exists(dst):
        os.makedirs(dst);
        shutil.copystat(src, dst);
    lst = os.listdir(src);
    if ignore:
        excl = ignore(src, lst);
        lst = [x for x in lst if x not in excl];
    for item in lst:
        s = os.path.join(src, item);
        d = os.path.join(dst, item);
        if symlinks and os.path.islink(s):
            if os.path.lexists(d):
                os.remove(d);
            os.symlink(os.readlink(s), d);
            try:
                st = os.lstat(s);
                mode = stat.S_IMODE(st.st_mode);
                os.lchmod(d, mode);
            except:
                pass; # lchmod not available
        elif os.path.isdir(s):
            copytree(s, d, symlinks, ignore);
        else:
            shutil.copy2(s, d);


# MAIN function
if __name__ == "__main__":

    for i_iter in range(0, 40):
        #print i_iter;
        # 1st parameter is target 0.47
        # 2nd parameter is k_edge
        # 3rd param is timeout (miliseconds)
        os.system("C://Users//azer//workspace//RepulsionPak_SCP//Release//RepulsionPak.exe 0.435 1000 1000000 4 5 6 7 8 9 10");
        ret_dir = "C://Users//azer//OneDrive - University of Waterloo//dump_results_01//1000_" + str(i_iter);
        print ret_dir;
        copytree("C://Users//azer//OneDrive//Images//PhysicsPak_Snapshots_01", ret_dir);
        