#-*- coding:utf-8 -*-

import os
import re
import time
import ftplib

#################################### FTP Tool ####################################
# ===========================================================
# Purpose:
#     Download file from FTP site to local path
#
# Inputs:
#    domain:       Domain name of FTP site
#    serve_path:   FTP server path
#    local_path:   Local path
#    file_name:    Target file name
# ===========================================================
def ftp_download(domain,serve_path,local_path,file_name):

    # Local Path exist or Not
    if not os.path.exists(local_path):
        os.makedirs(local_path)
    # Whether File exist in Local Path
    if os.path.exists(f"{local_path}/{file_name}"):
        # Compare local size and remote size
        with ftplib.FTP(domain,timeout=30) as myftp:
            myftp.login()
            myftp.cwd(serve_path)
            # Avoid the prompt ftplib.error_perm: 550 SIZE not allowed in ASCII
            myftp.voidcmd('TYPE I') 
            remote_size = myftp.size(file_name)
            local_size = os.path.getsize(f"{local_path}/{file_name}")
            if local_size != remote_size:
                print(f"- Local file is different with remote file: {file_name} ")
            else:
                print(f"- Download {file_name} Success[exists] ")
                return True

    time_beg = time.time()
    print(f"- Begin download file {file_name}... ")
    try:
        with ftplib.FTP(domain,timeout=30) as myftp:
            print(myftp.login())
            print(serve_path)
            print(myftp.cwd(serve_path))
            try:
                with open(f"{local_path}/{file_name}","wb") as myfile:
                    myftp.retrbinary("RETR "+file_name,myfile.write)
                    time_end = time.time()
                    print(f"- Download {file_name} Success! Spend: {time_end-time_beg:.3f}s ")
                return True
            except ftplib.all_errors as e:
                print(e)
                print(f"- Download {file_name} Fail! ")
                os.remove(f"{local_path}/{file_name}")
                return False
    except ftplib.all_errors:
        print(f"- Can't Connect Download {file_name} Fail!")
        return False

# ===========================================================
# Purpose:
#     Get filelist from FTP site
#
# Inputs:
#    domain:       Domain name of FTP site
#    serve_path:   FTP server path
#
# Outputs:
#    files map     Upper site name: long name MGEX RINEX observation files
#                  Lower site name: IGS RINEX observation files
# ===========================================================
def get_filelist_from_ftp(domain,serve_path):

    try:
        with ftplib.FTP(domain,timeout=100) as myftp:
            myftp.login()
            myftp.cwd(serve_path)
            files = []
            try:
                files = myftp.nlst()
            except ftplib.all_errors as e:
                print(e)
                print(f"- No files in ftp server:{domain}, path:{serve_path} ")
            map_files = { f[:4]:f for f in files}
    except Exception:
        return {}

    return map_files


#################################### File Tool ####################################
# ===========================================================
# Purpose:
#     Read Sitelist
#
# Inputs:
#    file_name:    Name of sitelist
#
# Outputs:
#    site_list in "list" form
#
# Formats:
#    -------------------------------------------
#    | areg
#    | gop7
#    | kir8
#    -------------------------------------------
# ===========================================================
def read_sitelist(filename):
    site_list = []
    with open(filename, "r") as myfile:
        for line in myfile:
            if re.search(r"^\s(\w{4})$", line) or re.search(r"^\s(\w{4})\s", line) or re.search(r"^(\w{4})$", line) or re.search(r"^(\w{4})\s+", line):
                site = line.split()[0]
                site_list.append(site.lower())

    return site_list


#################################### String Tool ####################################
# ===========================================================
# Purpose:
#     Replace -YYYY- with 4-digit year, and replace -DDD- with DOY
#
# Inputs:
#    old_name:    string of old name 
#    year,doy:    year, day of year
#
# Outputs:
#    string of new name
# ===========================================================
def replace_YYYYDDD(old_name, year, doy):
    new_name = old_name.replace("-YYYY-",f"{year:04}")
    new_name = new_name.replace("-DDD-",f"{doy:03}")
    return new_name

# ===========================================================
# Purpose:
#     Convert Sys to System
#
# Inputs:
#    sys:    abbreviation of GNSS system (G/C/E/R)
#
# Outputs:
#    string of systems
# ===========================================================
### 
def sat_system(sys):
    process_sys = []
    for onesys in sys:
        if (onesys == "G"):
            process_sys.append("GPS")
        elif (onesys == "E"):
            process_sys.append("GAL")
        elif (onesys == "R"):
            process_sys.append("GLO")
        elif (onesys == "C"):
            process_sys.append("BDS")
    return " ".join(process_sys)
