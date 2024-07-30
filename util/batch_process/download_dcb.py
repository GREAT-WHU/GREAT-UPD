#-*- coding:utf-8 -*-
# ----------------------------------------------------------------
# *                                                             * |
# * History                                                     * |
# *   -1.0 Guolong Feng  2020-05-05 created                     * |
# * Copyright (c) 2020, Wuhan University. All rights reserved.  * |
# *                                                             * |
# * Brief    Download CODE DCB Files                            * |
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

DCB_DOMAIN = "ftp.aiub.unibe.ch"

def usage():
    print("  Purpose: Download CODE DCB Files                                          ")
    print("  Usage: python dcb_download.py -y <year> -d <doy> -l <length> --dst=<dst>  ")
    print("        -y              Year                                                ")
    print("        -d              Day of Year                                         ")
    print("        -l              Length of Days                                      ")
    print("        --dst=          Directory of DCB Files                              ")
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
    
    # Check Directory exist or Not
    if not os.path.exists(dst):
        os.makedirs(dst)

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

        new_year,new_mon,new_day = gnss_time.doy2ymd(new_year,new_doy)
        print(f"\n- YYYYMMDD: {new_year:04}-{new_mon:02}-{new_day:02} ")
        str_yr = f"{new_year:04}"[-2:]
        str_p1c1 = f"P1C1{str_yr}{new_mon:02}.DCB.Z"
        str_p1p2 = f"P1P2{str_yr}{new_mon:02}.DCB.Z"
        str_p2c2 = f"P2C2{str_yr}{new_mon:02}_RINEX.DCB.Z"
        
        gnss_tools.ftp_download(DCB_DOMAIN,f"/CODE/{new_year:04}",dst,str_p1c1)
        gnss_tools.ftp_download(DCB_DOMAIN,f"/CODE/{new_year:04}",dst,str_p1p2)
        gnss_tools.ftp_download(DCB_DOMAIN,f"/CODE/{new_year:04}",dst,str_p2c2)
    
    if platform.system() == "Windows": 
        os.system(f"{sys.path[0]}/gzip -df {dst}/*.Z")
    else:
        os.system(f"gzip -df {dst}/*.Z")  
    sys.exit(0)


if __name__ == "__main__":
    main()
