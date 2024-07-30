#-*- coding:utf-8 -*-
# ----------------------------------------------------------------
# *                                                             * |
# * History                                                     * |
# *   -1.0 Guolong Feng  2020-05-05 created                     * |
# * Copyright (c) 2020, Wuhan University. All rights reserved.  * |
# *                                                             * |
# * Brief    Download GNSS Navigation Files                     * |
# * Author   Guolong Feng, Wuhan University                     * |
# * Date     2020-05-05                                         * |
# * Description     python 3.*                                  * |
# *                                                             * |
# ----------------------------------------------------------------

import os 
import sys
import getopt
import platform
import gnss_time
import gnss_tools

CDDIS_DOMAIN = "cddis.gsfc.nasa.gov" 

def usage():
    print("  Purpose: Download GNSS Navigation Files                                   ")
    print("  Usage: python nav_download.py -y <year> -d <doy> -l <length> --dst=<dst>  ")
    print("        -y              Year                                                ")
    print("        -d              Day of Year                                         ")
    print("        -l              Length of Days                                      ")
    print("        --dst=          Directory of Nav Files                              ")
    print("        -h|--help       Help Information                                    ")

def main():
    try:
        opts,args = getopt.getopt(sys.argv[1:],"hy:d:l:",["help","dst="])
    except getopt.GetoptError:
        usage()
        sys.exit(1)

    print(opts)

    year = 0
    doy  = 0
    length = 1
    dst = "" 
    for opt,arg in opts:
        if opt == "-y":
            year = int(arg)
        elif opt == "-d":
            doy = int(arg)
        elif opt == "-l":
            length = int(arg)
        elif opt == "--dst":
            dst = arg
        elif opt in ("-h","--help"):
            usage()
            sys.exit(1)

    if (year == 0 or doy == 0 or dst == ""):
        usage()
        sys.exit(1) 
    
    for iday in range(length):
        new_year = year
        new_doy = doy + iday
        if new_doy == 366:
            if gnss_time.leapyear(year) == 0:
                new_year = new_year + 1
                new_doy = 1
        elif new_doy > 366:
            if gnss_time.leapyear(year) == 0:
                new_year = new_year + 1
                new_doy = new_doy - 365
            else:
                new_year = new_year + 1
                new_doy = new_doy - 366
        
        print(f"\n- YYYYDDD: {new_year:04} {new_doy:03} ")
        str_yr = f"{new_year:04}"[-2:]
        str_nav_n = f"brdc{new_doy:03}0.{str_yr}n.Z"
        str_nav_g = f"brdc{new_doy:03}0.{str_yr}g.Z"
        str_nav_p = f"brdm{new_doy:03}0.{str_yr}p.Z"
        str_daily_dst = f"{dst}/{new_year:04}/{new_doy:03}"
        
        # Check Directory exist or Not
        if not os.path.exists(f"{str_daily_dst}"):
            os.makedirs(f"{str_daily_dst}")
        
        gnss_tools.ftp_download(CDDIS_DOMAIN,f"/pub/gps/data/daily/{new_year:04}/brdc",str_daily_dst,str_nav_n)
        gnss_tools.ftp_download(CDDIS_DOMAIN,f"/pub/gps/data/daily/{new_year:04}/brdc",str_daily_dst,str_nav_g)
        gnss_tools.ftp_download(CDDIS_DOMAIN,f"/pub/gps/data/campaign/mgex/daily/rinex3/{new_year:04}/brdm",str_daily_dst,str_nav_p)

        
        if platform.system() == "Windows": 
            os.system(f"{sys.path[0]}/gzip -df {str_daily_dst}/*.Z")
        else:
            os.system(f"gzip -df {str_daily_dst}/*.Z")
    
    sys.exit(0)


if __name__ == "__main__":
    main()
