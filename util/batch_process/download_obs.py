#-*- coding:utf-8 -*-
# ----------------------------------------------------------------
# *                                                             * |
# * History                                                     * |
# *   -1.0 Guolong Feng  2020-05-05 created                     * |
# * Copyright (c) 2020, Wuhan University. All rights reserved.  * |
# *                                                             * |
# * Brief    Download GNSS Observation Files <MGEX, then IGS>   * |
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
    print("  Purpose: Download GNSS Observation Files <priority: MGEX, then IGS>                                ")
    print("  Usage: python obs_download.py -y <year> -d <doy> -l <length> --dst=<dst>  --site=<site_list_file>  ")
    print("        -y              Year                                                                         ")
    print("        -d              Day of Year                                                                  ")
    print("        -l              Length of Days                                                               ")
    print("        --dst=          Directory of Obs Files                                                       ")
    print("        --site=         Site List                                                                    ")
    print("        -h|--help       Help Information                                                             ")

def main():
    try:
        opts,args = getopt.getopt(sys.argv[1:],"hy:d:l:",["help","dst=","site="])
    except getopt.GetoptError:
        usage()
        sys.exit(1)

    print(opts)

    CRX2RNX_EXE = f"{sys.path[0]}/crx2rnx"
    GZIP_EXE = f"{sys.path[0]}/gzip"
    if platform.system() == "Linux": 
        os.system(f"chmod +x {CRX2RNX_EXE}")
        GZIP_EXE = "gzip"

    year = 0
    doy  = 0
    length = 1
    dst = "" 
    site_list_file = ""
    for opt,arg in opts:
        if opt == "-y":
            year = int(arg)
        elif opt == "-d":
            doy = int(arg)
        elif opt == "-l":
            length = int(arg)
        elif opt == "--dst":
            dst = arg
        elif opt == "--site":
            site_list_file = arg
        elif opt in ("-h","--help"):
            usage()
            sys.exit(1)

    if (year == 0 or doy == 0 or dst == "" or site_list_file == ""):
        usage()
        sys.exit(1) 
    
    # get sitelist
    site_list = gnss_tools.read_sitelist(site_list_file)
    
    # loop doy
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
        # check store directory
        str_daily_dst = f"{dst}/{new_year:04}/{new_doy:03}"
        if not os.path.exists(f"{str_daily_dst}"):
            os.makedirs(f"{str_daily_dst}")
        # get ftp file list
        str_yr = f"{new_year:04}"[-2:]
        serve_path = f"/pub/gps/data/daily/{new_year:04}/{new_doy:03}/{str_yr}d"
        ftplist = gnss_tools.get_filelist_from_ftp(CDDIS_DOMAIN,serve_path)
        # loop site
        for site in site_list:
            site_upper = site.upper()
            site_lower = site.lower()
        
            str_obs = f"{str_daily_dst}/{site_lower}{new_doy:03}0.{str_yr}o"
            # check obs file whether exist
            if os.path.exists(f"{str_obs}"): continue

            # First MGEX
            if site_upper in ftplist.keys():
                gnss_tools.ftp_download(CDDIS_DOMAIN,serve_path,str_daily_dst,ftplist[site_upper])    
                str_crx_gz = f"{str_daily_dst}/{ftplist[site_upper]}"    # MGEX Obs
                str_crx = str_crx_gz[:-3]
                str_rnx = str_crx[:-3] + "rnx"
                
                if os.path.exists(f"{str_crx_gz}"):
                    os.system(f"{GZIP_EXE} -d {str_crx_gz}")
                    os.system(f"{CRX2RNX_EXE} -f {str_crx}")
                    os.remove(str_crx)
                    os.rename(str_rnx,str_obs)
            
            # Then IGS Obs
            if not os.path.exists(f"{str_obs}"):
                if site_lower in ftplist.keys():
                    gnss_tools.ftp_download(CDDIS_DOMAIN,serve_path,str_daily_dst,ftplist[site_lower])                               
                    str_igs_dZ = f"{str_daily_dst}/{ftplist[site_lower]}"    # IGS Obs
                    str_igs_d = str_igs_dZ[:-2]
                    if os.path.exists(f"{str_igs_dZ}"):
                        os.system(f"{GZIP_EXE} -d {str_igs_dZ}")
                        os.system(f"{CRX2RNX_EXE} -f {str_igs_d}") 
                        os.remove(str_igs_d)
                    else:
                        print(f"- Can't get Observation from {CDDIS_DOMAIN} ")
                else:
                    print(f"- Can't get Observation from {CDDIS_DOMAIN} ")
    
    sys.exit(0)


if __name__ == "__main__":
    main()
