#-*- coding:utf-8 -*-
# ----------------------------------------------------------------
# *                                                             * |
# * History                                                     * |
# *   -1.0 Guolong Feng  2020-05-04 created                     * |
# * Copyright (c) 2020, Wuhan University. All rights reserved.  * |
# *                                                             * |
# * Brief    batch process script for PreEdit                 * |
# * Author   Guolong Feng, Wuhan University                     * |
# * Date     2020-05-04                                         * |
# * Description     python 3.*                                  * |
# *                                                             * |
# ----------------------------------------------------------------

import os
import sys
import getopt
import platform
import gnss_config
import gnss_time
import gnss_tools
import xml.etree.ElementTree as et

monthdays = [0,31,28,31,30,31,30,31,31,30,31,30,31]

class PreEdit():
    ### Initialization
    def __init__(self,GNSSconfig):
        self.config = GNSSconfig                                     # Configure File [ini]
        self.workdir = os.path.abspath(GNSSconfig.work_dir())        # Working Directory
        self.runsoft = os.path.abspath(GNSSconfig.software())        # PreEdit Software
        
        self.ymd_beg = GNSSconfig.ymd_beg()                          # Beg DOY
        self.ymd_end = GNSSconfig.ymd_end()                          # End DOY
        self.hms_beg = GNSSconfig.hms_beg()                          # Beg Hour/Min/Sec in one Day
        self.hms_end = GNSSconfig.hms_end()                          # End Hour/Min/Sec in one Day
        self.refXML = os.path.abspath(GNSSconfig.ref_xml())          # Reference XML File
        self.satsys = gnss_tools.sat_system(GNSSconfig.satsys())     # Processing System
        self.sitelist_file = os.path.abspath(GNSSconfig.sitelist())  # Site List File
        self.interval = GNSSconfig.interval()                        # Sampling Interval
        self.elevation = GNSSconfig.minimum_elev()                   # Cutoff Elevation

        try:
            self.site_list = gnss_tools.read_sitelist(self.sitelist_file)      # Read Site List
        except Exception as e:
            self.site_list = []
            print (f"Can't get the site list from file:{self.sitelist_file}")

        if platform.system() == "Linux": 
            os.system(f"chmod +x {self.runsoft}")
            self.File_separator = "/"
        else:
            self.File_separator = "\\"

    ### Write PreEdit XML File
    def __rewrite_PreEdit_xml(self,year,doy,xmlfilename):
        
        tree = et.parse(self.refXML)
        root = tree.getroot()
        
        ## Module <gen>
        gen = root.find("gen")
        beg = gen.find("beg")
        end = gen.find("end")
        yyyy,mon,day = gnss_time.doy2ymd(year,doy)
        beg.text = f" {yyyy:04}-{mon:02}-{day:02} {self.hms_beg} "
        end.text = f" {yyyy:04}-{mon:02}-{day:02} {self.hms_end} "
        system = gen.find("sys")
        system.text = " " + self.satsys + " "
        rec = gen.find("rec")
        rec.text = ""
        sample_rate = gen.find("int")
        sample_rate.text = " " + str(self.interval) + " "

        ## Module <inputs>
        inputs = root.find("inputs")
        rinexn = inputs.find("rinexn")                       ## nav
        rinexn.text = f" {self.navfile} " 

        rinexo = inputs.find("rinexo")                       ## obs
        rinexo.text = ""

        count = 0
        for i,site in enumerate(self.site_list):
            rec.text += f" {site.upper()} "
            rinexo.text += f" {self.obsfiles[site]} " 
            if  (count+1) % 6 == 0:
                rec.text += "\n     "
                rinexo.text += "\n     "
            count +=1

        ## Module <process>
        process = root.find("process")
        process.set("minimum_elev",str(self.elevation))

        ## Module <outputs>
        outputs = root.find("outputs")
        ambflag = outputs.find("ambflag")
        ambflag.text = " " + self.ambflag + " "

        # Write XML
        tree.write(xmlfilename)

    ### Set GNSS Files(RINEX-Obs+RINEX-Nav) List
    def __set_gnsstables(self,year,doy):
        obsdir = gnss_tools.replace_YYYYDDD(self.config.obs_dir(),year,doy)
        navdir = gnss_tools.replace_YYYYDDD(self.config.nav_dir(),year,doy)
        str_yr = f"{year:04}"[-2:]

        self.navfile = f"{navdir}{self.File_separator}brdm{doy:03}0.{str_yr}p"
        self.navfile = os.path.abspath(self.navfile)

        self.obsfiles = {}    
        for site in self.site_list:
            obsfile = f"{obsdir}{self.File_separator}{site.lower()}{doy:03}0.{str_yr}o"
            self.obsfiles[site]=os.path.abspath(obsfile)

        ambflagdir = gnss_tools.replace_YYYYDDD(self.config.ambflag_dir(),year,doy)
        self.ambflag = f"{ambflagdir}{self.File_separator}$(rec)-DOY-0.-YY-o.ambflag"
        self.ambflag = os.path.abspath(self.ambflag)

    ### PreEdit Processing
    def process(self): 
        # Loop Time
        year, doy = gnss_time.ymd2doy(self.ymd_beg[0], self.ymd_beg[1], self.ymd_beg[2])
        beg_mjd = gnss_time.ymd2mjd(self.ymd_beg[0], self.ymd_beg[1], self.ymd_beg[2])
        end_mjd = gnss_time.ymd2mjd(self.ymd_end[0], self.ymd_end[1], self.ymd_end[2])
        ndays = int(end_mjd - beg_mjd) + 1
        for iday in range(ndays):
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
            
            # Generate XML and Run PreEdit
            CurrentDir = gnss_tools.replace_YYYYDDD(self.workdir,new_year,new_doy)
            if not os.path.exists(f"{CurrentDir}"):
                os.makedirs(f"{CurrentDir}")

            print (f"\n ---------------------------------------------------------------- ")
            print (f" Reference  XML : {self.refXML} ")
            print (f" Working   Path : {CurrentDir}")
            
            self.__set_gnsstables(new_year,new_doy)
            
            xmlfilename = f"{CurrentDir}{self.File_separator}PreEdit.xml"

            print (f" Product of XML : {xmlfilename} ")           
            self.__rewrite_PreEdit_xml(new_year,new_doy,xmlfilename)
            
            print (f" PreEdit  Cmd : {self.runsoft} -x {xmlfilename}")           
            os.system(f"{self.runsoft} -x {xmlfilename}")


def usage():
    print ("                                                                           ")
    print ("    Purpose: Cycle-slip detection                                          ")
    print ("    Usage: python  PreEdit.py  -c  <config_file>  --config=<config_file> ")
    print ("           -c|--config=         Reference ini file             (Required)  ")
    print ("                                                                           ")

def main():

    # Default Settings:
    configfile = ""

    # Get Arguments
    try:
        opts,args = getopt.getopt(sys.argv[1:],"hc:",["help","config="])
    except getopt.GetoptError:
        usage()
        sys.exit(1)

    # print(opts)

    for opt,arg in opts:
        if opt in ("-c","--config"):
            configfile = arg
        elif opt in ("-h","--help"):
            usage()
            sys.exit(1)

    # Program Terminated due to Incomplete Arguments
    if configfile=="" :
        usage()
        sys.exit(1)

    cfg = gnss_config.GNSSconfig(configfile)

    # Initialize PreEdit class
    runTB = PreEdit(cfg)
    # Run PreEdit
    runTB.process()


if __name__ == "__main__":

    main()
