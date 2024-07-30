#-*- coding:utf-8 -*-

# ===========================================================
#     Initialization File (ini) Decoder
# ===========================================================
import configparser

class GNSSconfig:
    def __init__(self,configfile=""):
        self.project_info = {}
        self.process_info = {}
        self.data_info = {}
        if configfile != "":
            cf = configparser.ConfigParser()
            cf.read(configfile)

            if cf.has_section("project"):
                self.project_info = dict(cf.items("project"))
            else:
                self.project_info = dict()
            
            if cf.has_section("process"):
                self.process_info = dict(cf.items("process"))
            else:
                self.process_info = dict()
            
            if cf.has_section("data"):
                self.data_info = dict(cf.items("data"))
            else:
                self.data_info = dict()
# ===========================================================
#     Section project
# ===========================================================
    def ymd_beg(self):
        try:
            ymd_beg = self.project_info["ymd_beg"]
        except KeyError as e:
            print(str(e))
            print("- Can't get the ymd_beg from config file!! Check the config file!!!")
            exit(1)
        ymd_beg = ymd_beg.split("-") 
        return int(ymd_beg[0]),int(ymd_beg[1]),int(ymd_beg[2])

    def ymd_end(self):
        try:
            ymd_end = self.project_info["ymd_end"]
        except KeyError as e:
            print(str(e))
            print("- Can't get the ymd_end from config file!! Check the config file!!!")
            exit(1)
        ymd_end = ymd_end.split("-") 
        return int(ymd_end[0]),int(ymd_end[1]),int(ymd_end[2])

    def hms_beg(self):
        try:
            hms_beg = self.project_info["hms_beg"]
        except KeyError as e:
            print(str(e))
            print("- Can't get the hms_beg from config file!! Check the config file!!!")
            exit(1)
        hms_beg = hms_beg.split(":") 
        str_hms_beg = hms_beg[0].zfill(2) + ":" + hms_beg[1].zfill(2) + ":" + hms_beg[2].zfill(2)
        return str_hms_beg

    def hms_end(self):
        try:
            hms_end = self.project_info["hms_end"]
        except KeyError as e:
            print(str(e))
            print("- Can't get the hms_end from config file!! Check the config file!!!")
            exit(1)
        hms_end = hms_end.split(":") 
        str_hms_end = hms_end[0].zfill(2) + ":" + hms_end[1].zfill(2) + ":" + hms_end[2].zfill(2)
        return str_hms_end

    def satsys(self):
        if not self.project_info.get("satsys"):
            return self.project_info.setdefault("satsys","G")
        else:
            return self.project_info["satsys"]

    def sitelist(self):
        if not self.project_info.get("sitelist"):
            return self.project_info.setdefault("sitelist","site_list")
        else:
            return self.project_info["sitelist"]

    def interval(self):
        if not self.project_info.get("interval"):
            return int(self.project_info.setdefault("interval","30"))
        else:
            return int(self.project_info["interval"])

    def sat_rm(self):
        if not self.process_info.get("sat_rm",""):
            return self.process_info.setdefault("sat_rm","")
        else:
            sats = self.process_info["sat_rm"]
        if sats.strip() == "NONE": return ""      
        sats_remove = sats.split("+")
        str = " "
        for single in sats_remove:
            str += f"{single} "
        return str

    def ref_xml(self):
        if not self.project_info.get("ref_xml"):
            return self.project_info.setdefault("ref_xml","xx.ref_xml")
        else:
            return self.project_info["ref_xml"]

    def work_dir(self):
        if not self.project_info.get("work_dir"):
            return self.project_info.setdefault("work_dir",".")
        else:
            return self.project_info["work_dir"]

    def software(self):
        if not self.project_info.get("software"):
            return self.project_info.setdefault("software","xxxx")
        else:
            return self.project_info["software"]

# ===========================================================
#     Section process
# ===========================================================
    def upd_mode(self):
        if not self.process_info.get("upd_mode",""):
            return self.process_info.setdefault("upd_mode","WL+NL")
        else:
            return self.process_info["upd_mode"]

    def minimum_elev(self):
        if not self.process_info.get("minimum_elev",""):
            return int(self.process_info.setdefault("minimum_elev","0"))
        else:
            return int(self.process_info["minimum_elev"])

# ===========================================================
#     Section data directory
# ===========================================================
    def obs_dir(self):
        return self.data_info["obs_dir"]

    def nav_dir(self):
        return self.data_info["nav_dir"]

    def dcb_dir(self):
        return self.data_info["dcb_dir"]

    def upd_dir(self):
        return self.data_info["upd_dir"]

    def ifcb_dir(self):
        return self.data_info["ifcb_dir"]

    def ambflag_dir(self):
        return self.data_info["ambflag_dir"]

    def ambupd_dir(self):
        return self.data_info["ambupd_dir"]

