#-*- coding:utf-8 -*-
# ----------------------------------------------------------------
# *                                                             * |
# * History                                                     * |
# *   -1.0 Guolong Feng  2020-05-04 created                     * |
# * Copyright (c) 2020, Wuhan University. All rights reserved.  * |
# *                                                             * |
# * Brief    batch process script for turboedit                 * |
# * Author   Guolong Feng, Wuhan University                     * |
# * Date     2020-05-04                                         * |
# * Description     python 3.*                                  * |
# *                                                             * |
# ----------------------------------------------------------------

import os
import sys
import getopt
import ctypes
import platform
import gnss_config
import gnss_time
import gnss_tools
from xml.dom.minidom import Document


class UPD():
    ### Initialization
    def __init__(self,GNSSconfig):
        self.config = GNSSconfig                                     # Configure File [ini]
        self.workdir = os.path.abspath(GNSSconfig.work_dir())        # Working Directory
        self.runsoft = os.path.abspath(GNSSconfig.software())        # Great-UE Software
        
        self.ymd_beg = GNSSconfig.ymd_beg()                          # Beg DOY
        self.ymd_end = GNSSconfig.ymd_end()                          # End DOY
        self.hms_beg = GNSSconfig.hms_beg()                          # Beg Hour/Min/Sec in one Day
        self.hms_end = GNSSconfig.hms_end()                          # End Hour/Min/Sec in one Day
        self.satsys = gnss_tools.sat_system(GNSSconfig.satsys())          # Processing System
        self.sitelist_file = os.path.abspath(GNSSconfig.sitelist())  # Site List File
        self.interval = GNSSconfig.interval()                        # Sampling Interval
        self.sat_rm = GNSSconfig.sat_rm()                            # Excluded Satellites
        self.upd_mode = GNSSconfig.upd_mode().strip().upper()        # UPD Mode: EWL/WL/NL/EWL_EPOCH/WL+NL[default]

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

    ### Set GNSS Files(RINEX-Obs+RINEX-Nav) List
    def __set_gnsstables(self,year,doy):
        str_yr = f"{year:04}"[-2:]
        
        ## Nav File
        navdir = gnss_tools.replace_YYYYDDD(self.config.nav_dir(),year,doy)
        self.navfile = f"{navdir}{self.File_separator}brdm{doy:03}0.{str_yr}p"
        self.navfile = os.path.abspath(self.navfile)

        ## Receiever List
        count = 0
        self.rec_list=" \n"
        for i,site in enumerate(self.site_list):
            if  (count+1) % 6 == 1:
                self.rec_list += "            "
            self.rec_list += f" {site.upper()} "
            if  (count+1) % 6 == 0:
                self.rec_list += "\n"
            count += 1
        self.rec_list += "      "
        
        ## AMBFLAG + Obs + DCB +IFCB File
        if self.upd_mode == "WL" or self.upd_mode == "EWL" or self.upd_mode == "EWL_EPOCH" or self.upd_mode == "WL+NL":
            count = 0
            self.obs_list=" \n"
            self.ambflag_list=" \n"
            obsdir = gnss_tools.replace_YYYYDDD(self.config.obs_dir(),year,doy)
            obsdir = os.path.abspath(obsdir)
            ambflagdir = gnss_tools.replace_YYYYDDD(self.config.ambflag_dir(),year,doy)
            ambflagdir = os.path.abspath(ambflagdir)
            for i,site in enumerate(self.site_list):
                if  (count+1) % 6 == 1:
                    self.obs_list += "               "
                    self.ambflag_list += "                "
                self.obs_list += f" {obsdir}{self.File_separator}{site.lower()}{doy:03}0.{str_yr}o " 
                self.ambflag_list += f" {ambflagdir}{self.File_separator}{site.lower()}{doy:03}0.{str_yr}o.ambflag " 
                if  (count+1) % 6 == 0:
                    self.obs_list += "\n"
                    self.ambflag_list += "\n"
                count += 1
            self.obs_list += "      "
            self.ambflag_list += "      "

            year,mon,day = gnss_time.doy2ymd(year,doy)
            dcbdir = gnss_tools.replace_YYYYDDD(self.config.dcb_dir(),year,doy)
            dcbdir = os.path.abspath(dcbdir)
            self.dcb_list = ""
            self.dcb_list += f" {dcbdir}{self.File_separator}P1C1{str_yr}{mon:02}.DCB "
            self.dcb_list += f" {dcbdir}{self.File_separator}P1P2{str_yr}{mon:02}.DCB "
            self.dcb_list += f" {dcbdir}{self.File_separator}P2C2{str_yr}{mon:02}_RINEX.DCB "

            ifcbdir = gnss_tools.replace_YYYYDDD(self.config.ifcb_dir(),year,doy)
            ifcbdir = os.path.abspath(ifcbdir)
            self.ifcbfile = f"{ifcbdir}{self.File_separator}ifcb_{year:04}{doy:03}"
        ## AMBUPD + WL UPD File      
        if self.upd_mode == "NL" or self.upd_mode == "WL+NL":
            upddir = gnss_tools.replace_YYYYDDD(self.config.upd_dir(),year,doy)
            upddir = os.path.abspath(upddir)
            self.updfile = f"{upddir}{self.File_separator}upd_wl_{year:04}{doy:03}_" + self.config.satsys().strip()
            
            ambupddir = gnss_tools.replace_YYYYDDD(self.config.ambupd_dir(),year,doy)
            ambupddir = os.path.abspath(ambupddir)
            count = 0
            self.ambupd_list=" \n"
            for i,site in enumerate(self.site_list):
                if  (count+1) % 6 == 1:
                    self.ambupd_list += "               "
                self.ambupd_list += f" {ambupddir}{self.File_separator}{site.upper()}_ambupd_{year:04}{doy:03} " 
                if  (count+1) % 6 == 0:
                    self.ambupd_list += "\n"
                count +=1
            self.ambupd_list += "      "


    def __gen_UPD_XML(self,year,doy,updmode,out_upd_file,xmlname):
        doc = Document()  #创建DOM文档对象
        config_XML = doc.createElement("config") #创建根元素
        doc.appendChild(config_XML)
        
        ### general settings
        gen = doc.createElement("gen")
        config_XML.appendChild(gen)
        yyyy,mon,day = gnss_time.doy2ymd(year,doy)
        # beg time
        beg_time = doc.createElement("beg")
        beg_time_text = doc.createTextNode(f" {yyyy:04}-{mon:02}-{day:02} {self.hms_beg} ")  # set element text
        beg_time.appendChild(beg_time_text)
        gen.appendChild(beg_time)
        # end time
        end_time = doc.createElement("end")
        end_time_text = doc.createTextNode(f" {yyyy:04}-{mon:02}-{day:02} {self.hms_end} ")  # set element text
        end_time.appendChild(end_time_text)
        gen.appendChild(end_time)
        # system 
        system = doc.createElement("sys")
        system_text = doc.createTextNode(" " + self.satsys + " ")  # set element text
        system.appendChild(system_text)
        gen.appendChild(system)
        # site list
        rec_list = doc.createElement("rec")
        rec_list_text = doc.createTextNode(self.rec_list)  # set element text
        rec_list.appendChild(rec_list_text)
        gen.appendChild(rec_list)
        # interval
        intv = doc.createElement("int")
        intv_text = doc.createTextNode(" " + str(self.interval) + " ")  # set element text
        intv.appendChild(intv_text)
        gen.appendChild(intv)
        # excluded sats
        sat_rm = doc.createElement("sat_rm")
        sat_rm_text = doc.createTextNode(self.sat_rm)  # set element text
        sat_rm.appendChild(sat_rm_text)
        gen.appendChild(sat_rm)
    
        ### inputs settings
        inputs = doc.createElement("inputs")
        config_XML.appendChild(inputs)
        # nav RINEX
        rinexn = doc.createElement("rinexn")
        rinexn_text = doc.createTextNode(" " + self.navfile + " ")  # set element text
        rinexn.appendChild(rinexn_text)
        inputs.appendChild(rinexn)
        if updmode == "WL" or updmode == "EWL" or updmode == "EWL_EPOCH":
            # obs RINEX
            rinexo = doc.createElement("rinexo")
            rinexo_text = doc.createTextNode(self.obs_list)  # set element text
            rinexo.appendChild(rinexo_text)
            inputs.appendChild(rinexo)
            
            # ambflag : cycle slip information
            ambflag = doc.createElement("ambflag")
            ambflag_text = doc.createTextNode(self.ambflag_list)  # set element text
            ambflag.appendChild(ambflag_text)
            inputs.appendChild(ambflag)
            # DCB in CODE format
            biabern = doc.createElement("biabern")
            biabern_text = doc.createTextNode(self.dcb_list)  # set element text
            biabern.appendChild(biabern_text)
            inputs.appendChild(biabern)
        if updmode == "NL":
            # ambupd : float IF and widelane ambiguities
            ambupd = doc.createElement("ambupd")
            ambupd_text = doc.createTextNode(self.ambupd_list)  # set element text
            ambupd.appendChild(ambupd_text)
            inputs.appendChild(ambupd)
            # WL UPD for NL UPD estimation
            upd_input = doc.createElement("upd")
            upd_input_text = doc.createTextNode(" " + self.updfile + " ")  # set element text
            upd_input.appendChild(upd_input_text)
            inputs.appendChild(upd_input)
        if updmode == "EWL" or updmode == "EWL_EPOCH":
            # IFCB for EWL UPD estimation
            ifcb = doc.createElement("ifcb")
            ifcb_text = doc.createTextNode(" " + self.ifcbfile + " ")  # set element text
            ifcb.appendChild(ifcb_text)
            inputs.appendChild(ifcb)
        
        ## UPD mode settings
        process = doc.createElement("process")
        if updmode == "EWL_EPOCH":
             process.setAttribute("updmode","EWL_epoch")
        else:
            process.setAttribute("updmode",updmode)
        config_XML.appendChild(process)
        
        ## outputs settings
        outputs = doc.createElement("outputs")
        outputs.setAttribute("append","false")
        outputs.setAttribute("verb","2")
        config_XML.appendChild(outputs)
        # log UPD estimation
        log = doc.createElement("log")
        log_text = doc.createTextNode(" LOGRT.log ")  # set element text
        log.appendChild(log_text)
        outputs.appendChild(log)
        # output upd file
        upd = doc.createElement("upd")
        upd_text = doc.createTextNode(f" {out_upd_file} ")  # set element text
        upd.appendChild(upd_text)
        outputs.appendChild(upd)
    
        ########### 将DOM对象doc写入文件
        f = open(xmlname,'w')
        doc.writexml(f, newl = ' \n', addindent = '   ',encoding='utf-8')
        f.close()


    def process(self):
        year,doy = gnss_time.ymd2doy(self.ymd_beg[0],self.ymd_beg[1],self.ymd_beg[2])
        beg_mjd = gnss_time.ymd2mjd(self.ymd_beg[0],self.ymd_beg[1],self.ymd_beg[2])
        end_mjd = gnss_time.ymd2mjd(self.ymd_end[0],self.ymd_end[1],self.ymd_end[2])
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
            
            # Generate XML and Run UPD
            CurrentDir = gnss_tools.replace_YYYYDDD(self.workdir,new_year,new_doy)
            if not os.path.exists(f"{CurrentDir}"):
                os.makedirs(f"{CurrentDir}")
            
            print (f"\n ---------------------------------------------------------------- ")
            print (f" Working   Path : {CurrentDir}")
            self.__set_gnsstables(new_year,new_doy)
            
            if self.upd_mode == "WL+NL":
                # WL
                xmlfilename=f"{CurrentDir}{self.File_separator}upd_WL.xml"
                print (f" Product of XML : {xmlfilename} ")           
                out_upd_file=f"{CurrentDir}{self.File_separator}upd_wl_{new_year:04}{new_doy:03}_{self.config.satsys().strip()}"
                self.__gen_UPD_XML(new_year,new_doy,"WL",out_upd_file,xmlfilename)
                os.system(f"{self.runsoft} -x {xmlfilename}")
                # NL
                xmlfilename=f"{CurrentDir}{self.File_separator}upd_NL.xml"
                print (f" Product of XML : {xmlfilename} ")           
                out_upd_file=f"{CurrentDir}{self.File_separator}upd_nl_{new_year:04}{new_doy:03}_{self.config.satsys().strip()}"
                self.__gen_UPD_XML(new_year,new_doy,"NL",out_upd_file,xmlfilename)
                os.system(f"{self.runsoft} -x {xmlfilename}")
                
            else:
                xmlfilename=f"{CurrentDir}{self.File_separator}upd_{self.upd_mode}.xml"
                print (f" Product of XML : {xmlfilename} ")           
                out_upd_file=f"{CurrentDir}{self.File_separator}upd_{self.upd_mode.lower()}_{new_year:04}{new_doy:03}_{self.config.satsys().strip()}"
                self.__gen_UPD_XML(new_year,new_doy,self.upd_mode,out_upd_file,xmlfilename)
        
                os.system(f"{self.runsoft} -x {xmlfilename}")


def usage():
    print ("                                                                           ")
    print ("    Purpose: UPD estimation script                                         ")
    print ("    Usage: python  upd.py  -c  <config_file>  --config=<config_file>       ")
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

    # Initialize TurboEdit class
    runUPD = UPD(cfg)
    # Run UPD
    runUPD.process()
    
if __name__ == "__main__":
    
    main()
