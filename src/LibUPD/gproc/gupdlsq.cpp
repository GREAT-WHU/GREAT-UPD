/**
*
* @verbatim
History
-1.0 bwang xjhan glfeng 2019-02-22  creat the file.
-1.1 glfeng  2019-03-02  Adjusted some functions and variables.
-1.2 bwang  2019-04-09  Adding Doxygen Style Code Remarks.
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		gupdlsq.cpp
* @brief	compute Wide-Lane, Ex-Wide-Lane and Narrow-Lane upd.
* @author   glfeng bwang xjhan , Wuhan University
* @version	1.0.0
* @date		2019-04-09
*
*/

#include <algorithm>
#include "gproc/gupdlsq.h"


namespace great
{
	struct OneArc
	{
		bool is_ref;   // ref arc or not
		bool is_con;   // connect to ref arc or not
		double rnw;
		double srnw;
		int nepos;
	};

	t_gupdlsq::t_gupdlsq(UPDMODE updmode, t_gsetbase* set, t_gallproc* data, t_glog* log):
		_corbds(set)
	{
		// set settings && log file
		_set = set;
		_glog = log;
		_data = data;
		
		// set upd estimate mode
		_updmode = updmode;
		
		if (_cal_UPD)
		{
			delete _cal_UPD;
			_cal_UPD = nullptr;
		}
		_cal_UPD = new t_gupd();
		_cal_UPD->set_mode(updmode);

		// set Out
		_setOut();
        _setOut_EWL();
	}

	t_gupdlsq::~t_gupdlsq()
	{
		if (_cal_UPD) { delete _cal_UPD; _cal_UPD = nullptr; }
		if (_fambres) { if (_fambres->is_open()) { _fambres->close(); }; delete _fambres; _fambres = nullptr; }
        if (_fambres_EWL) { if (_fambres_EWL->is_open()) { _fambres_EWL->close(); }; delete _fambres_EWL; _fambres_EWL = nullptr; }
	}

	bool t_gupdlsq::GenerateProduct()
	{
		// Encoder UPD files
		string path = dynamic_cast<t_gsetout*>(_set)->outputs("upd");
			
		t_gcoder* gencode = new t_upd(_set, "", 4096);
		gencode->clear();
		gencode->glog(_glog);
		gencode->path(path);
		gencode->add_data("IDX", _cal_UPD);

		t_gfile* gout = new t_gfile;
		gout->glog(_glog);
		gout->path(path);
		gout->coder(gencode);
		write_log_info(_glog, 1, " t_gupdlsq::GenerateProduct :  Writing IFCB/UPD file " + path);
		gout->run_write();
		delete gout; gout = NULL;
		delete gencode; gencode = NULL;

		return true;
	}

	bool t_gupdlsq::_initUPDProcData(t_gallproc * data)
	{
		set<string> sys = dynamic_cast<t_gsetgen*>(_set)->sys();
		if (sys.size() > 1) {
			write_log_info(_glog, 1, " ERROR[t_gupdlsq::_initUPDProcData] : Only One SYS can be process every time!");
			return false;
		}
		_sys = *sys.begin();

		// prepare data
		if (_updmode == UPDMODE::NL)
		{
			_allambupd = dynamic_cast<t_gallambupd*>((*data)[t_gdata::AMBUPD]);
			_allWLUPD = dynamic_cast<t_gupd*>((*data)[t_gdata::UPD]);
			if (!_allambupd || !_allWLUPD)
			{
				write_log_info(_glog, 1, " ERROR[t_gupdlsq::_initUPDProcData] : can not init NL process data.");
				return false;
			}
			_site_ambupd_end = _allambupd->getSitesEndRecord();
		}
		else
		{
			_gifcb = dynamic_cast<t_gifcb*>((*data)[t_gdata::IFCB]);
			_allobs = dynamic_cast<t_gallobs*>((*data)[t_gdata::ALLOBS]);
			_allbias = dynamic_cast<t_gallbias*>((*data)[t_gdata::ALLBIAS]);
			_allambupd = dynamic_cast<t_gallambupd*>((*data)[t_gdata::AMBUPD]);
			_allambflag = dynamic_cast<t_gallambflag*>((*data)[t_gdata::AMBFLAG]);
			if (!_allobs || !_allbias || !_allambflag)
			{
				write_log_info(_glog, 1, " ERROR[t_gupdlsq::_initUPDProcData] : can not init EWL/WL process data.");
				return false;
			}
			if (_updmode != UPDMODE::WL && (_sys == "GPS" && !_gifcb))
			{
				write_log_info(_glog, 1, " ERROR[t_gupdlsq::_initUPDProcData] : can not init EWL process data.");
				return false;
			}
		}

		// GLO freq ID / BDS codebias
		_allnav = dynamic_cast<t_gallnav*>((*data)[t_gdata::GRP_EPHEM]);
		if (_sys == "GLO") 
		{
			_glo_freq_num = _allnav->glo_freq_num();
		}

		write_log_info(_glog, 1, " INIT [t_gupdlsq::_initUPDProcData] : process data in UPD Estimation is prepared.");
		return true;
	}

	bool t_gupdlsq::_initSatSite()
	{
		try
		{
			set<string> sat_rm = dynamic_cast<t_gsetgen*>(_set)->sat_rm();
			if (_updmode == UPDMODE::NL)
			{
				// set parlist<first sats, then sites> + sites + sats according ambupd
				// get sat list
				set<string> sats1, sats2, sats3;
				sats1 = _allambupd->get_sats();
				sats2 = GNSS_SATS()[t_gsys::str2gsys(_sys)];

				// sats3 = intersection of sats1 and sats2
				set_intersection(sats1.begin(), sats1.end(), sats2.begin(), sats2.end(), inserter(sats3, sats3.begin()));
				// _sats = sats3 - sat_rm
				set_difference(sats3.begin(), sats3.end(), sat_rm.begin(), sat_rm.end(), inserter(_sats, _sats.begin()));
				// push sats into _parlist				
				for (auto it = _sats.begin(); it != _sats.end(); it++)
				{
                    _cal_UPD->re_init_upd(WL_IDENTIFY, *it);
					_parlist.push_back(*it);
				}
				// get site list && init site wl upd && push sites into _parlist
			    set<string> sites1, sites2;
				sites1 = dynamic_cast<t_gsetgen*>(_set)->recs();
				sites2 = _allambupd->get_sites();
				set_intersection(sites1.begin(), sites1.end(), sites2.begin(), sites2.end(), inserter(_sites, _sites.begin()));				
				//_sites = _allambupd->get_sites();
				for (set<string>::iterator iter_site = _sites.begin(); iter_site != _sites.end(); iter_site++)
				{
					_allWLUPD->re_init_upd(WL_IDENTIFY, *iter_site);
					_parlist.push_back(*iter_site);
				}
			}
			else
			{
				// set parlist<first sats, then sites> + sites + sats according to log/log13
				set<string> sats1, sats2, sats3;
				sats1 = _allambflag->getSatList();
				sats2 = GNSS_SATS()[t_gsys::str2gsys(_sys)];
				// sats3 = intersection of sats1 and sats2
				set_intersection(sats1.begin(), sats1.end(), sats2.begin(), sats2.end(), inserter(sats3, sats3.begin()));
				// _sats = sats3 - sat_rm
				set_difference(sats3.begin(), sats3.end(), sat_rm.begin(), sat_rm.end(), inserter(_sats, _sats.begin()));
				// push sats into _parlist && init sat wl upd				
				for (auto it = _sats.begin(); it != _sats.end(); it++)
				{
					_cal_UPD->re_init_upd(WL_IDENTIFY, *it);
					_parlist.push_back(*it);
				}
				
				// get site list && init site wl upd && push sites into _parlist
				set<string> sites1, sites2, sites3, sites4;
				sites1 = dynamic_cast<t_gsetgen*>(_set)->recs();
				sites2 = _allambflag->getSiteList();
				sites4 = _allobs->getSiteList();
				set_intersection(sites1.begin(), sites1.end(), sites2.begin(), sites2.end(), inserter(sites3, sites3.begin()));
				set_intersection(sites3.begin(), sites3.end(), sites4.begin(), sites4.end(), inserter(_sites, _sites.begin()));
				//_sites = dynamic_cast<t_gsetgen*>(_set)->recs();
				for (set<string>::iterator iter_site = _sites.begin(); iter_site != _sites.end(); iter_site++)
				{
					_cal_UPD->re_init_upd(WL_IDENTIFY, *iter_site);
					_parlist.push_back(*iter_site);
				}
			}
			return true;
		}

		catch (...)
		{
			write_log_info(_glog, 1, " ERROR[t_gupdlsq::_initSatSite] : Unknown Mistake");
			return false;
		}
	}

	void t_gupdlsq::_setOut()
	{
		gtrace("t_gupdlsq::_setOut");
		// set ambres file
		_fambres = new t_giof;
		_fambres->tsys(t_gtime::GPS);
		_fambres->mask(UPDType2String(_updmode) + "-res");
		_fambres->append(dynamic_cast<t_gsetout*>(_set)->append());	
	}

	void t_gupdlsq::_setOut_EWL()
	{
		gtrace("t_gupdlsq::_setOut_EWL");
		// set ambres file
		_fambres_EWL = new t_giof;
		_fambres_EWL->tsys(t_gtime::GPS);
		_fambres_EWL->mask("EWL-res");
		_fambres_EWL->append(dynamic_cast<t_gsetout*>(_set)->append());
	}
	
	bool t_gupdlsq::processBatch(const t_gtime& beg, const t_gtime& end, const double &intv)
	{
		/** Prepare data */
		if (!_initUPDProcData(_data)) return false;

		/** Init sat/site info */
		if (!_initSatSite()) return false;

		/** Init wavelen */
		if (!_setWavelength()) return false;

		if (_updmode == UPDMODE::WL || _updmode == UPDMODE::EWL || _updmode == UPDMODE::EWL_epoch)
		{
			if (!_processWL(beg, end, intv)) return false;
		}
		else if (_updmode == UPDMODE::NL)
		{
			if (!_processNL(beg, end, intv)) return false;
		}
		return true;
	}

	bool t_gupdlsq::_processWL(const t_gtime& beg, const t_gtime& end, const double &intv)
	{
		auto it_site = _sites.begin();
		map <string, t_epo_sat_amb> all_site_amb;  // ewl_epoch
		/** Loop sites */
		for (it_site; it_site != _sites.end(); it_site++)
		{
			/** Init sat_site pair ambiguities */
			auto it_sat = _sats.begin();
			for (; it_sat != _sats.end(); it_sat++)
			{
				_sat_site_amb[*it_site][*it_sat] = 9.90;  /// == xx(ambiguity of satellite-station pair)
			}

			t_all_sats_OW OW;             /// == OW
			t_epo_sat_amb one_site_amb;   /// == wl/ewl

			/** Form WL/EWL observations */
			if (!t_gupdlsq::_combineObsIF(*it_site, beg, end, intv, one_site_amb, OW)) return false;

			if (_updmode == UPDMODE::WL || _updmode == UPDMODE::EWL)
			{
				/** Calculate EWL/WL ambiguities for UPD estimation */
				if (!t_gupdlsq::_calculateAmbWL(one_site_amb, OW)) return false;

          	    /** Save float EWL ambiguities for res calculation */
				if (_updmode == UPDMODE::EWL)
				{
					auto itsat = OW.begin();
					for (itsat; itsat != OW.end(); itsat++)
					{
						auto itepo = itsat->second.begin();
						for (itepo; itepo != itsat->second.end(); itepo++)
						{
							for (int iepo = (*itepo)->beg_epo; iepo <= (*itepo)->end_epo; iepo++)
							{							
								_AMB_RES_EWL[iepo][*it_site][itsat->first].ewl_res = (*itepo)->rwl;
								_AMB_RES_EWL[iepo][*it_site][itsat->first].ewl_amb_sigma = (*itepo)->srwl;

							}
						}
					}
				}
				/** Connect EWL/WL ambiguities of a satellite-station pair */
				if (!t_gupdlsq::_connectAmb(*it_site, OW)) return false;
		}
			else if (_updmode == UPDMODE::EWL_epoch)
			{
				all_site_amb[*it_site] = one_site_amb;
			}
	}

		if (_updmode == UPDMODE::WL || _updmode == UPDMODE::EWL)
		{
			/** Search reference station */
			if (!t_gupdlsq::_setRefStation(WL_IDENTIFY)) return false;

			/** Get initial values of UPDs from UD-ambiguities */
			if (!t_gupdlsq::_initUPD(WL_IDENTIFY)) return false;

			/** estimate upd of each satellite and station **/
			if (!t_gupdlsq::_updEstimate(WL_IDENTIFY)) return false;

            /** calculate and write EWL UPD res */
            if (_updmode == UPDMODE::EWL)
			{
				_writeAmbResidual_EWL();
			}
			
			/** Delete site upd result */
			for (set<string>::iterator it_site = _sites.begin(); it_site != _sites.end(); ++it_site)
			{
				string site = *it_site;
				_cal_UPD->delete_upd(WL_IDENTIFY, site);
			}
		}
		else if (_updmode == UPDMODE::EWL_epoch)
		{
			int iepo = 0;
			t_gtime epoch = beg;
			t_gtime pre_epoch;
			bool is_first = true;
			_cal_UPD->set_valid_beg(epoch);
			while (epoch <= end)
			{
				iepo = iepo + 1;
				it_site = _sites.begin();
				pre_epoch.from_mjd(epoch.mjd(), epoch.sod() - intv, epoch.dsec());
				for (it_site; it_site != _sites.end(); it_site++)
				{
					auto it_sat = _sats.begin();
					t_all_sats_OW OW;
					shared_ptr<t_oneway_ambiguity> temp_single_ow;
					_cal_UPD->copy_upd(pre_epoch, *it_site, epoch, is_first, true);

					t_epo_sat_amb one_site_amb_tmp;
					for (; it_sat != _sats.end(); it_sat++)
					{
						if (it_site == _sites.begin()) _cal_UPD->copy_upd(pre_epoch, *it_sat, epoch, is_first, false);
						one_site_amb_tmp[iepo][*it_sat] = all_site_amb[*it_site][iepo][*it_sat];
						temp_single_ow = make_shared<t_oneway_ambiguity>();
						(*temp_single_ow).beg_epo = iepo;
						(*temp_single_ow).end_epo = iepo;
						(*temp_single_ow).sat = *it_sat;
						OW[*it_sat].push_back(temp_single_ow);
					}
					if (!t_gupdlsq::_calculateAmbWL(one_site_amb_tmp, OW)) return false;
					if (!t_gupdlsq::_connectAmb(*it_site, OW)) return false;
				}
				if (!t_gupdlsq::_setRefStation(epoch)) return false;
				if (!t_gupdlsq::_initUPD(epoch)) return false;
				if (!t_gupdlsq::_updEstimate(epoch)) return false;

				epoch.from_mjd(epoch.mjd(), epoch.sod() + intv, epoch.dsec());
				is_first = false;
			}
			/** Delete site upd result */
			for (set<string>::iterator it_site = _sites.begin(); it_site != _sites.end(); ++it_site)
			{
				string site = *it_site;
				_cal_UPD->delete_upd(epoch - intv, site);
			}
            
		}

		return true;
	}

	bool t_gupdlsq::_processNL(const t_gtime& beg, const t_gtime& end, const double &intv)
	{
		t_gtime epoch = beg;
		t_gtime pre_epoch;
		t_epoch_amb AMB;
		bool is_first = true;
		int nepo = 0;
		string sys_idx;

		///** Remove the impact of  TAI-tsys difference */
		// epoch = epoch + epoch.tai_tsys(t_gtime::GPS);
		// loop epo
		while (epoch <= end)
		{
			cerr << epoch.str_ymdhms("", false) << endl;
			pre_epoch.from_mjd(epoch.mjd(), epoch.sod() - intv, epoch.dsec());
			// loop site
			auto itsite = _sites.begin();
			for (itsite; itsite != _sites.end(); itsite++)
			{
				/** use last epoch results as current epoch init */
				_cal_UPD->copy_upd(pre_epoch, *itsite, epoch, is_first, true);
				// loop sat
				auto itsat = _sats.begin();
				for (itsat; itsat != _sats.end(); itsat++)
				{
					/** Init sat_site pair ambiguities */
					_sat_site_amb[*itsite][*itsat] = 9.90;
					/** use last epoch results as current epoch init */
					if (itsite == _sites.begin()) _cal_UPD->copy_upd(pre_epoch, *itsat, epoch, is_first, false);
					/** Assign value to AMB from _gambupd */
					if (!_allambupd->isAmbUpdValid(epoch, *itsite, *itsat))continue;
					AMB[*itsite][*itsat].bc = _allambupd->getOneSiteOneEpoOneSatAmbUpd(epoch, *itsite, *itsat)->amb_if;
					AMB[*itsite][*itsat].bwi = _allambupd->getOneSiteOneEpoOneSatAmbUpd(epoch, *itsite, *itsat)->amb_wl;
					AMB[*itsite][*itsat].bw0 = 0;
					AMB[*itsite][*itsat].bw = _allambupd->getOneSiteOneEpoOneSatAmbUpd(epoch, *itsite, *itsat)->amb_wl;
					AMB[*itsite][*itsat].sbw = _allambupd->getOneSiteOneEpoOneSatAmbUpd(epoch, *itsite, *itsat)->amb_wl_sigma;
					AMB[*itsite][*itsat].epo = epoch;
				}
				/** Calculate NL ambiguities. */
				if (!_calculateAmbNL(*itsite, epoch, AMB)) return false;
			}
			nepo++;
			if (nepo >= 1)   
			{
				if (_jref_site.empty())
				{
					/** Search reference station */
					if (!t_gupdlsq::_setRefStation(epoch)) return false;
				}
				if (!_jref_site.empty() || !_iref_site.empty())
				{
					/** Set NL UPD valid begin time */
					_cal_UPD->set_valid_beg(epoch);
					/** Get initial values of UPDs from UD-ambiguities */
					if (!t_gupdlsq::_initUPD(epoch)) return false;
					/** estimate upd of each satellite and station **/
					if (!t_gupdlsq::_updEstimate(epoch)) 
					{
						epoch.from_mjd(epoch.mjd(), epoch.sod() + intv, epoch.dsec());
						is_first = false;
						continue;
					}
						 
					/** Write amb_res file */
					_writeAmbResidual(epoch);
				}
			}
			/** Increase time */
			epoch.from_mjd(epoch.mjd(), epoch.sod() + intv, epoch.dsec());
			is_first = false;			
		}

		/** Delete site upd result */
		for (set<string>::iterator it_site = _sites.begin(); it_site != _sites.end(); ++it_site)
		{
			string site = *it_site;
			_cal_UPD->delete_upd(epoch - intv, site);
		}
		return true;
	}

	string t_gupdlsq::UPDType2String(UPDMODE type)
	{
		switch (type) {
		case EWL:  return "EWL";
		case WL:  return "WL";
		case NL:  return "NL";
		default:             return "UNDEF";
		}
		return "UNDEF";
	}

	bool t_gupdlsq::_setWavelength()
	{
		_proc_band = dynamic_cast<t_gsetgnss*>(_set)->band(t_gsys::str2gsys(_sys));	

		if ((_updmode == EWL_epoch || _updmode == EWL) && _proc_band.size() != 3)
		{
			write_log_info(_glog, 1, " ERROR[t_gupdlsq::_setWavelength] : Unrecognized wavelength : " + _sys);
			return false;
		}
		
		set<string> sats_wavelen;	
		switch (t_gsys::str2gsys(_sys))
		{
		case GPS:
			sats_wavelen.insert("G"); 
			break;
		case GAL:
			sats_wavelen.insert("E"); 
			break;
		case BDS:
			sats_wavelen.insert("C"); 
			break;
		case GLO:
			for (auto it_sat : _glo_freq_num)
			{
				sats_wavelen.insert(it_sat.first);
			}
			break;
		default: 
			break;
		}

		if (sats_wavelen.size() == 0)
		{
			write_log_info(_glog, 1, " ERROR[t_gupdlsq::_setWavelength] : Unrecognized system (GCRE) : " + _sys); 
			return false;
		}

		t_gobsgnss* gnss = new t_gobsgnss();
		for (auto sat : sats_wavelen)
		{
			gnss->sat(sat);
			if(_sys == "GLO") gnss->channel(_glo_freq_num[sat]);

			_sys_wavelen[sat]["L1"] = gnss->wavelength(_proc_band[0]);
			_sys_wavelen[sat]["L2"] = gnss->wavelength(_proc_band[1]);
			
			_sys_wavelen[sat]["WL"] = gnss->wavelength_WL(_proc_band[0], _proc_band[1]);
			_sys_wavelen[sat]["NL"] = gnss->wavelength_NL(_proc_band[0], _proc_band[1]);
			if (_updmode == EWL_epoch || _updmode == EWL)
			{
				_sys_wavelen[sat]["L3"] = gnss->wavelength(_proc_band[2]);
				_sys_wavelen[sat]["EWL"] = gnss->wavelength_WL(_proc_band[1], _proc_band[2]);
			}
		}

		delete gnss; gnss = nullptr;
		return true;
	}

	bool t_gupdlsq::_combineObsIF(string site, const t_gtime& T_beg, const t_gtime& T_end, double intv,
		t_epo_sat_amb& one_site_amb, t_all_sats_OW& OW)
	{
		//local variables
		int num_epo;
		string site_lower;
		vector<t_gobsgnss> obsdata;
		t_vec_ambflag one_sat_flag;
		shared_ptr<t_oneway_ambiguity> temp_single_ow;
		t_gambflag one_site_flag;

		double mw_obs;
		double correct;
		GOBSBAND b1, b2, b3;
		t_gobs gobs1, gobs2, gobs3, gobs4, gobs5;
		try
		{
			//get number of epochs
			num_epo = round((T_end - T_beg) / intv);

			// get one site ambflag
			site_lower = "";
			transform(site.begin(), site.end(), back_inserter(site_lower), ::tolower);
			one_site_flag = _allambflag->getOneAmbFlag(site_lower);

			t_gtime it_time = T_beg;
			///** Remove the impact of  TAI-tsys difference */
			//t_gtime it_gpstime = it_time + it_time.tai_tsys(t_gtime::GPS);
            //loop over epoch
			for (int iepo = 1; iepo <= num_epo; iepo++)
			{
				obsdata = _allobs->obs(site, it_time);

				if (_sys == "BDS")
				{
					t_gtriple BDS_tmp(0.0, 0.0, 0.0);
					_corbds.apply_IGSO_MEO(it_time, BDS_tmp, _allnav, obsdata);

					if (obsdata.size() > 0 && BDS_tmp.zero())
					{
						BDS_tmp = _allobs->getsitecrd(site);
						_corbds.apply_IGSO_MEO(it_time, BDS_tmp, _allnav, obsdata);
					}

				}

				//loop over sats
				for (auto it_satdata = obsdata.begin(); it_satdata != obsdata.end(); it_satdata++)
				{
					string sat = it_satdata->sat();
					one_site_amb[iepo][sat] = 0.0;
					if (sat.substr(0, 1) == "R" && it_satdata->channel() >= DEF_CHANNEL) it_satdata->channel(_glo_freq_num[sat]);

					//put amb arc into OW
					int AMB_pos;
					if (!one_site_flag.isValid(sat, it_time, AMB_pos)) continue;
					one_sat_flag = one_site_flag.getAmbFlagData()[sat];
					if (one_sat_flag[AMB_pos]->iflag != "0")
					{
						temp_single_ow = make_shared<t_oneway_ambiguity>();
						(*temp_single_ow).beg_epo = iepo;
						(*temp_single_ow).sat = sat;
						OW[sat].push_back(temp_single_ow);
						one_site_flag.reset_iflag(sat, "0", AMB_pos);
					}
					if (OW[sat].size() != 0) OW[sat].back()->end_epo = iepo;

					//b1 = t_gsys::band_priority(it_satdata->gsys(), FREQ_1);
					//b2 = t_gsys::band_priority(it_satdata->gsys(), FREQ_2);
					b1 = _proc_band[0];
					b2 = _proc_band[1];

					gobs1 = t_gobs(it_satdata->select_range(b1));
					gobs2 = t_gobs(it_satdata->select_range(b2));
					gobs3 = t_gobs(it_satdata->select_phase(b1));
					gobs4 = t_gobs(it_satdata->select_phase(b2));

					// WL
					if (_updmode == UPDMODE::WL)
					{
						mw_obs = it_satdata->MW_cycle(gobs3, gobs4, gobs1, gobs2);
					}//EWL/EWL_epoch
					else
					{
						//b3 = t_gsys::band_priority(it_satdata->gsys(), FREQ_3);
						b3 = _proc_band[2];
						gobs5 = t_gobs(it_satdata->select_phase(b3));
						mw_obs = it_satdata->EWL_cycle(gobs3, gobs4, gobs5, gobs1, gobs2);
					}

					if (double_eq(mw_obs, 0.0)) continue;

					//dcb correct
					correct = 0.0;
					if (_updmode == UPDMODE::WL)
					{
						_apply_DCB( *it_satdata, correct, OBSCOMBIN::MW_COMBIN, &gobs1, &gobs2);
					}
					else
					{
						_apply_DCB(*it_satdata, correct, OBSCOMBIN::EWL_COMBIN, &gobs1, &gobs2);
						if (sat.substr(0, 1) == "G")
						{
							one_epoch_ifcb epoch_ifcb = _gifcb->get_epo_ifcb(it_satdata->epoch());
							if (epoch_ifcb.find(sat) == epoch_ifcb.end()) continue;

							if (_gifcb->get_ifcb_sigma(it_satdata->epoch(), sat) <= 0.2 &&
								_gifcb->get_ifcb_npoint(it_satdata->epoch(), sat) > 2)
							{
								double factor = -3.113;
								correct -= factor * _gifcb->get_ifcb_value(it_satdata->epoch(), sat);
							}
						}
					}
					
					mw_obs += correct;

					one_site_amb[iepo][sat] = mw_obs;

				}//end sat

				it_time = it_time + intv;
			}//end epo

			return true;
		}
		catch (...)
		{
			write_log_info(_glog, 1, " ERROR[t_gupdlsq::_combineObsIF] : unknown mistake !");
			return false;
		}
	}

	bool t_gupdlsq::_calculateAmbWL(t_epo_sat_amb& one_site_amb, t_all_sats_OW& OW)
	{
		list<pair<double, double>> wx;
		double mean, sigma, mean_sig;
		int npos = 0;

		try
		{
			auto itsat = OW.begin();
			// loop sats
			for (itsat; itsat != OW.end(); itsat++)
			{
				auto itepo = itsat->second.begin();  
				// each sat has several arcs, loop arcs
				for (itepo; itepo != itsat->second.end(); )
				{
					wx.clear();
					npos = distance(itsat->second.begin(), itepo);  // index of arc in one sat
					OW[itsat->first][npos]->rwl = 0.0;
					// each arc has several epoch, loop epo
					for (int iepo = (*itepo)->beg_epo; iepo <= (*itepo)->end_epo; iepo++)
					{
						// one_site_amb means wl/ewl
						if (double_eq(one_site_amb[iepo][itsat->first], 0.0))  continue;
						wx.push_back(make_pair(one_site_amb[iepo][itsat->first], 1.0));
					}
					// Calculate mean as well as sigma
					getMeanWgt(false, wx, mean, sigma, mean_sig);
					// Select OK wl/ewl ambiguity according to threshold
					if ((_updmode == UPDMODE::WL && (wx.size() <= 15 || mean_sig > 0.15))
					 || (_updmode == UPDMODE::EWL && (wx.size() <= 15 || mean_sig > 0.15))
					 || (_updmode == UPDMODE::EWL_epoch && wx.size() <= 0))
					{
						itepo = itsat->second.erase(itepo);
					}
					else
					{
						OW[itsat->first][npos]->rwl = mean;
						OW[itsat->first][npos]->srwl = mean_sig;
						itepo++;
					}				
				}
			}
			return true;
		}
		catch (...)
		{
			write_log_info(_glog, 1, " ERROR[t_gupdlsq::_calculateAmbWL] : unknown mistake !");
			return false;
		}
	}

	bool t_gupdlsq::_calculateAmbNL(string site, const t_gtime& t, t_epoch_amb& AMB)
	{
		map<string, double> xwl;
		map<string, double> wgt;
		list<pair<double, double>> wl_sig;
		map<string, bool> isfixed;
		double sigx = 0.0;
		double mean = 0.0;
		double sig = 0.0;
		int npnt = 0;
		int ndel = 0;
		try 
		{
			//get wide-lane ambiguities which are corrected with satellite UPDs
			auto itsat = AMB[site].begin();
			for (itsat; itsat != AMB[site].end(); itsat++)
			{
				// skip sat that unvalid in Wl UPD
				if (_allWLUPD->get_upd_sigma(WL_IDENTIFY, itsat->first) > 1000.0) continue;
				/*skip past ambiguities(30000 ago).Be aware ambiguities are kept if no new one comes in for each satellite*/
				double dt = (itsat->second.epo.dmjd() - t.dmjd()) * 86400;
				if (abs(dt) > 30000.0) continue;
				if (_site_ambupd_end[site] < t) continue;   

				xwl[itsat->first] = itsat->second.bw0 + itsat->second.bw - _allWLUPD->get_upd_value(WL_IDENTIFY, itsat->first)
					- _allWLUPD->get_upd_value(WL_IDENTIFY, site);
				wgt[itsat->first] = 1.0;
				wl_sig.push_back(make_pair(xwl[itsat->first], wgt[itsat->first]));
				npnt++;
			}
			if (npnt > 0)
			{
				getMeanFract(wl_sig, mean, sig, sigx);
				if (sigx <= 0.1)
				{
					double wl_rec = _allWLUPD->get_upd_value(WL_IDENTIFY, site);
					if (!double_eq(wl_rec, 0.0)) mean = getFraction(mean, -0.5);
					_allWLUPD->reset_upd(WL_IDENTIFY, site, wl_rec + mean, sigx, wl_sig.size());
				}
			}

			//fix wide-lane using the estimated rec. UPD
			for (auto it_sat = AMB[site].begin(); it_sat != AMB[site].end(); ++it_sat)
			{
				// add for end of ambupd file
				double dt = (it_sat->second.epo.dmjd() - t.dmjd()) * 86400;
				if (abs(dt) > 30000.0) continue;

				if (_site_ambupd_end[site] < t) continue;

				string sat = it_sat->first;

				double bw = AMB[site][sat].bw0 + AMB[site][sat].bw - _allWLUPD->get_upd_value(WL_IDENTIFY, sat) -
					_allWLUPD->get_upd_value(WL_IDENTIFY, site);
				double sigma = AMB[site][sat].sbw;

				// define tmp freq
				string str_idx;
				str_idx = sat.substr(0, 1) == "R" ? sat : sat.substr(0, 1);
				if (fabs(bw - round(bw)) < 0.30 && sigma < 0.130 &&
					_allWLUPD->get_upd_sigma(WL_IDENTIFY, site) <= 0.150 &&
					_allWLUPD->get_upd_sigma(WL_IDENTIFY, sat) <= 0.150)
				{

					_AMB_RES[site][sat].wl_fixed = true;
					_AMB_RES[site][sat].wl_res = bw - round(bw);

					//compute narrow-lane ambiguties
					AMB[site][sat].bn = -AMB[site][sat].bc / _sys_wavelen[str_idx]["NL"] +
						_sys_wavelen[str_idx]["WL"] / _sys_wavelen[str_idx]["L2"] * round(bw);

					_sat_site_amb[site][sat] = getFraction(it_sat->second.bn, -0.50);
				}

				else
				{
					_AMB_RES[site][sat].wl_fixed = false;
					_AMB_RES[site][sat].wl_res = bw - round(bw);
				}
			}
			return true;
		}
		catch (...)
		{
			write_log_info(_glog, 1, " ERROR[t_gupdlsq::_calculateAmbNL] : unknown mistake !");
			return false;
		}
	}

	bool t_gupdlsq::_connectAmb(string site, t_all_sats_OW& OW)
	{	
		try 
		{

			vector<OneArc> ARC;
			auto itsat_ow = OW.begin();
			int  j, k;
			double dif, idif, sigma;
			// loop sats
			for (itsat_ow; itsat_ow != OW.end(); itsat_ow++)
			{
				ARC.clear();
				auto itepo = (itsat_ow->second).begin();
				// loop arcs and put them into vector structure
				for (itepo; itepo != itsat_ow->second.end(); itepo++)
				{
					OneArc tmp_arc;
					tmp_arc.is_ref = false;
					tmp_arc.is_con = false;
					tmp_arc.rnw = (*itepo)->rwl;
					tmp_arc.srnw = (*itepo)->srwl;
					tmp_arc.nepos = (*itepo)->end_epo - (*itepo)->beg_epo;
					ARC.push_back(tmp_arc);
				}
				// connect ambiguities to reference ambiguities (reference ambiguities may be more than one)
				if (ARC.size() == 0) continue;
				if (ARC.size() == 1)
				{
					ARC[0].is_ref = true;
				}
				else
				{
					for (j = 0; j < ARC.size() - 1; j++)
					{
						if (ARC[j].is_con) continue;
						for (k = j + 1; k < ARC.size(); k++)
						{
							if (ARC[k].is_con) continue;
							dif = ARC[k].rnw - ARC[j].rnw;
							idif = round(dif);
							dif = dif - idif;
							sigma = sqrt(pow(ARC[k].srnw, 2) + pow(ARC[j].srnw, 2));
							if (sigma < 0.10) sigma = 0.10;
							if (abs(dif) > 3 * sigma || sigma > 0.15) continue;
							ARC[j].is_ref = true;
							ARC[k].is_con = true;
							ARC[j].nepos = ARC[j].nepos + ARC[k].nepos;
							ARC[j].rnw = (ARC[j].rnw * pow(ARC[k].srnw, 2) + (ARC[k].rnw - idif) * pow(ARC[j].srnw, 2))
								/ (pow(ARC[k].srnw, 2) + pow(ARC[j].srnw, 2));
							ARC[j].srnw = ARC[k].srnw * ARC[j].srnw / sqrt(ARC[k].srnw / pow(ARC[j].srnw, 2) + ARC[j].srnw / pow(ARC[k].srnw, 2));
						}
					}
				}
				
				//selected the longst ref ambiguity 
				k = 0;
				for (j = 0; j < ARC.size(); j++)
				{
					if (ARC[j].is_con) continue;
					if (k > ARC[j].nepos) continue;
					if (ARC[j].rnw == 0.0) continue;

					// selected ambiguity of corresponding site-sat pair
					_sat_site_amb[site][itsat_ow->first] = ARC[j].rnw - round(ARC[j].rnw);
					k = ARC[j].nepos;
				}
			}
			return true;
		}
		catch (...)
		{
			write_log_info(_glog, 1, " ERROR[t_gupdlsq::_connectAmb] : unknown mistake !");
			return false;
		}
	}

	bool t_gupdlsq::_setRefStation(const t_gtime& t)
	{
		int msat_max;
		int msat;

		_iref_site = "";
		_jref_site = "";

		try 
		{
			//search station with max.observed satellites with valid satelite upds
			msat_max = 0;
			for (set<string>::iterator it_site = _sites.begin(); it_site != _sites.end(); ++it_site)
			{
				string site = *it_site;
				msat = 0;
				for (set<string>::iterator it_sat = _sats.begin(); it_sat != _sats.end(); ++it_sat)
				{
					string sat = *it_sat;
					if (double_eq(_sat_site_amb[site][sat], 9.90)) continue;
					if (!_cal_UPD->upd_usable(t, sat)) continue;
					msat++;
				}

				if (msat < msat_max) continue;
				msat_max = msat;
				_iref_site = site;
				if (msat == _sats.size()) break;
			}

			if (msat_max < 5) _iref_site = "";

			// search station with max. observed satellites
			// init all sat/site upd
			if (_iref_site == "")
			{
				msat_max = 0;
				for (set<string>::iterator it_site = _sites.begin(); it_site != _sites.end(); ++it_site)
				{
					string site = *it_site;
					_cal_UPD->re_init_upd(t, site);
					msat = 0;
					for (set<string>::iterator it_sat = _sats.begin(); it_sat != _sats.end(); ++it_sat)
					{
						string sat = *it_sat;
						_cal_UPD->re_init_upd(t, sat);
						if (double_eq(_sat_site_amb[site][sat], 9.90)) continue;
						msat++;
					}

					if (msat <= msat_max) continue;
					msat_max = msat;
					_jref_site = site;
					if (msat == _sats.size()) break;
				}
			}
			else
			{
				// init all site upds & not usable sat upds 
				for (set<string>::iterator it_site = _sites.begin(); it_site != _sites.end(); ++it_site)
				{
					string site = *it_site;
					_cal_UPD->re_init_upd(t, site);
				}
				for (set<string>::iterator it_sat = _sats.begin(); it_sat != _sats.end(); ++it_sat)
				{
					string sat = *it_sat;
					if (_cal_UPD->upd_usable(t, sat)) continue;
					_cal_UPD->re_init_upd(t, sat);
				}
			}

			return true;
		}
		catch (...)
		{
			write_log_info(_glog, 1, " ERROR[t_gupdlsq::_setRefStation] : unknown mistake !");
			return false;
		}	
	}

	bool t_gupdlsq::_initUPD(const t_gtime& epoch)
	{
		int iIter;
		double mean, sigma, sigx;
		t_updrec one_upd;
		// initial value + wgt --> mean value/sig
		list<pair<double, double>> dxx; 

		// take the fractional part of the reference station as satellite upd (assume
		// receiver upd is zero.It is possible that a satellite is not tracked at the
		// station.That means no upd for these satellites.This must be solved after
		// receiver upds are determined
		try
		{
			// set satellite upd initial value
			if (_iref_site.empty())
			{
				for (auto it_sat = _sats.begin(); it_sat != _sats.end(); it_sat++)
				{
					if (double_eq(_sat_site_amb[_jref_site][*it_sat], 9.90) || _cal_UPD->upd_usable(epoch, *it_sat))
						continue;

					one_upd.value = _sat_site_amb[_jref_site][*it_sat];
					one_upd.sigma = 1e-4;
					one_upd.npoint = 0;

					_cal_UPD->add_sat_upd(epoch, *it_sat, one_upd);
				}
			}

			//iteratively
			for (iIter = 1; iIter <= 3; iIter++)
			{
				// estimate the upds for receivers by removing satellite upds from the ud-amb.
				// and then taking the mean.
				for (auto it_site = _sites.begin(); it_site != _sites.end(); it_site++)
				{

					if (_cal_UPD->upd_usable(epoch, *it_site)) continue;

					dxx.clear();

					for (auto it_sat = _sats.begin(); it_sat != _sats.end(); it_sat++)
					{
						if (double_eq(_sat_site_amb[*it_site][*it_sat], 9.90) || !_cal_UPD->upd_usable(epoch, *it_sat))
						{
							continue;
						}

						dxx.push_back(make_pair(_sat_site_amb[*it_site][*it_sat] - _cal_UPD->get_upd()[epoch][*it_sat]->value, 1.0));
					}

					if (dxx.size() > 1)
					{
						//Calculate the station upd 
						getMeanFract(dxx, mean, sigma, sigx);

						if (sigma > 999.0 || double_eq(sigma, 999.0)) continue;
						one_upd.value = mean;
						one_upd.sigma = sigx;
						one_upd.npoint = dxx.size();
						_cal_UPD->add_sat_upd(epoch, *it_site, one_upd);

						//set UPD for satellites observed at this station but not yet has an intial 
						for (auto it_sat = _sats.begin(); it_sat != _sats.end(); it_sat++)
						{
							if (double_eq(_sat_site_amb[*it_site][*it_sat], 9.90) || _cal_UPD->upd_usable(epoch, *it_sat) ||
								!_cal_UPD->upd_usable(epoch, *it_site))
							{
								continue;
							}

							one_upd.value = _sat_site_amb[*it_site][*it_sat] - _cal_UPD->get_upd()[epoch][*it_site]->value;
							one_upd.sigma = 1e-4;
							one_upd.npoint = _cal_UPD->get_upd()[epoch][*it_sat]->npoint;
							_cal_UPD->add_sat_upd(epoch, *it_sat, one_upd);
						}
					}

				}//end sites upd


				// check missing satellites at the reference stations
				for (auto it_sat = _sats.begin(); it_sat != _sats.end(); it_sat++)
				{
					if (_cal_UPD->upd_usable(epoch, *it_sat) && _cal_UPD->get_upd()[epoch][*it_sat]->npoint != 0)
					{
						continue;
					}

					dxx.clear();

					for (auto it_site = _sites.begin(); it_site != _sites.end(); it_site++)
					{
						if (double_eq(_sat_site_amb[*it_site][*it_sat], 9.90) || !_cal_UPD->upd_usable(epoch, *it_site))
						{
							continue;
						}

						dxx.push_back(make_pair(_sat_site_amb[*it_site][*it_sat] - _cal_UPD->get_upd()[epoch][*it_site]->value, 1.0));
					}

					if (dxx.size() > 1)
					{
						//calculate satellite UPD
						getMeanFract(dxx, mean, sigma, sigx);

						one_upd.value = mean;
						one_upd.sigma = sigx;
						one_upd.npoint = dxx.size();
						_cal_UPD->add_sat_upd(epoch, *it_sat, one_upd);
					}
				}//end sats upd
			}// end Iteration

			return true;
		}
		catch (...)
		{
			write_log_info(_glog, 1, " ERROR[t_gupdlsq::_initUPD] : unknown mistake !");
			return false;
		}
	}

	bool t_gupdlsq::_updEstimate(const t_gtime& t)
	{
		// init par num and deleted par num
		int npar = _sites.size() + _sats.size();
		int npar_delete;
		// store idx of par in NEQ
		map<string, int> par_idx;
		// store used obs num in par estimate, default is 0
		map<string, int> par_nobs;
		// store site-sat pair amb whether is fixed, default is false
		map<pair<string, string>, bool> par_fixed;
		// ambiguities num
		int ntot_save_ambiguity;   //number fixed last time
		int ntot_ambiguity;        //number fixable
		int nall_ambiguity;        //number of all ambiguities
		int usable_ambiguity;     //number with both sat. and rec. upds
		//for sigma calculate
		double ltpl;
		//index
		int i, j, iter;
		// distinct NL/WL/EWL
		double threshold = 0.25;

		try
		{
			// Combine equation
			ntot_save_ambiguity = 0;
			for (iter = 1; iter <= 10; iter++)
			{
				// init par_nobs & par_fixed
				for (set<string>::iterator it_sat = _sats.begin(); it_sat != _sats.end(); ++it_sat)
				{
					string sat = *it_sat;
					par_nobs[sat] = 0;
					for (set<string>::iterator it_site = _sites.begin(); it_site != _sites.end(); ++it_site)
					{
						string site = *it_site;
						if (it_sat == _sats.begin()) par_nobs[site] = 0;
						par_fixed[make_pair(site, sat)] = false;
					}
				}

				// prepare NEQ/W
				Matrix NEQ(npar, npar); ColumnVector W(npar);
				ColumnVector xpar(npar); ColumnVector sxpar(npar);
				NEQ = 0.0; W = 0.0; xpar = 0.0; sxpar = 0.0;
				ltpl = 0.0; ntot_ambiguity = 0; nall_ambiguity = 0; usable_ambiguity = 0;
				i = 0;
				// loop sites
				for (set<string>::iterator it_site = _sites.begin(); it_site != _sites.end(); ++it_site)
				{
					i++;
					string site = *it_site;
					par_idx[site] = i + _sats.size();
					// loop sats
					j = 0;
					for (set<string>::iterator it_sat = _sats.begin(); it_sat != _sats.end(); ++it_sat)
					{
						j++;
						string sat = *it_sat;
						// first sats, then sites
						if (i == 1) par_idx[sat] = j;
						// cycle site-sat pair without data
						if (double_eq(_sat_site_amb[site][sat], 9.90)) continue;
						nall_ambiguity++;
						// cycle site-sat pair with unvalid upds
						if (!(_cal_UPD->upd_usable(t, site) && _cal_UPD->upd_usable(t, sat))) continue;
						usable_ambiguity++;
						// cycle site-sat pair whose fractional part not in threshold
						double xamb;
						int iamb;
						xamb = _sat_site_amb[site][sat] - _cal_UPD->get_upd_value(t, sat) - _cal_UPD->get_upd_value(t, site);
						iamb = round(xamb);
						if (fabs(xamb - iamb) > threshold) continue;
						// record site-sat pair used 
						par_fixed[make_pair(site, sat)] = true;
						// NEQ prepare
						NEQ(j, j) += 1.0;
						NEQ(j, i + _sats.size()) += 1.0;
						NEQ(i + _sats.size(), i + _sats.size()) += 1.0;
						// W prepare
						W(j) = W(j) - xamb + iamb;
						W(i + _sats.size()) = W(i + _sats.size()) - xamb + iamb;
						// record num of obs of obj (sat/site)
						par_nobs[sat]++;
						par_nobs[site]++;
						ntot_ambiguity++;
						// later become vtpv
						ltpl += (xamb - iamb) * (xamb - iamb);
					}
				}

				//select a refernece satellite with max. nsite
				//avoid select a ref sat without valid value ^_^
				int msite_max = 0;
				string iref_sat = "";
				for (set<string>::iterator it_sat = _sats.begin(); it_sat != _sats.end(); ++it_sat)
				{
					string sat = *it_sat;
					if (par_nobs[sat] > msite_max)
					{
						iref_sat = sat;
						msite_max = par_nobs[sat];
					}
				}
				if (iref_sat == "")
				{
					cerr << " Warning : Not enough sats in iter " << iter << " during the epoch : " << t.str_ymdhms() << endl;
					break;
				}

				// reference receiver upd, add strong constraint to avoid rank loss
				// consider obj without obs
				NEQ(par_idx[iref_sat], par_idx[iref_sat]) += 1e5;
				npar_delete = 0;
				for (auto it = _parlist.begin(); it != _parlist.end(); it++)
				{
					if (par_nobs[*it] == 0)
					{
						NEQ(par_idx[*it], par_idx[*it]) = 1e3;
						npar_delete++;
					}
					for (j = 1; j <= par_idx[*it] - 1; j++)
					{
						NEQ(par_idx[*it], j) = NEQ(j, par_idx[*it]);
					}
				}
				
				Matrix invNEQ;
				invNEQ << NEQ.i();

				// get solve
				for (i = 1; i <= npar; i++)
				{
					xpar(i) = 0.0;
					for (j = 1; j <= npar; j++)
					{
						xpar(i) += invNEQ(i, j) * W(j);
					}
					ltpl -= xpar(i) * W(i);  // vtpv
				}

				//calculate sigma
				double sigma0 = 0.0;

				if ((ntot_ambiguity - (npar - npar_delete)*1.0) > 0.0) 
				{
					sigma0 = sqrt(ltpl / (ntot_ambiguity - (npar - npar_delete)*1.0));
				}

				// add solve value to init value
				for (auto it = _parlist.begin(); it != _parlist.end(); it++)
				{
					if (par_nobs[*it] != 0)
					{
						sxpar(par_idx[*it]) = sigma0 * sqrt(invNEQ(par_idx[*it], par_idx[*it]));
					}
					if (*it != iref_sat && par_nobs[*it] == 0) continue;

					double tmp = _cal_UPD->get_upd_value(t, *it) - xpar(par_idx[*it]);
					_cal_UPD->reset_upd(t, *it, tmp, sxpar(par_idx[*it]), par_nobs[*it]);
				}

				if (ntot_save_ambiguity >= ntot_ambiguity) break;
				ntot_save_ambiguity = ntot_ambiguity;
			}


			// AMB_RES
			for (set<string>::iterator it_site = _sites.begin(); it_site != _sites.end(); ++it_site)
			{
				string site = *it_site;

				for (set<string>::iterator it_sat = _sats.begin(); it_sat != _sats.end(); ++it_sat)
				{
					string sat = *it_sat;
					if (double_eq(_sat_site_amb[site][sat], 9.90)) continue;
					if (!par_fixed[make_pair(site, sat)])
					{
						_AMB_RES[site][sat].nl_fixed = false;
						_AMB_RES[site][sat].nl_res = _sat_site_amb[site][sat] - _cal_UPD->get_upd_value(t, sat)
							- _cal_UPD->get_upd_value(t, site) - round(_sat_site_amb[site][sat]
								- _cal_UPD->get_upd_value(t, sat) - _cal_UPD->get_upd_value(t, site));

					}
					else
					{
						_AMB_RES[site][sat].nl_fixed = true;
						_AMB_RES[site][sat].nl_res = _sat_site_amb[site][sat] - _cal_UPD->get_upd_value(t, sat)
							- _cal_UPD->get_upd_value(t, site) - round(_sat_site_amb[site][sat]
								- _cal_UPD->get_upd_value(t, sat) - _cal_UPD->get_upd_value(t, site));
					}
				}
			}

			return true;
		}
		catch (...)
		{
			write_log_info(_glog, 1, " ERROR[t_gupdlsq::updEstimate] : unknown mistake !");
			return false;
		}
	}

	bool t_gupdlsq::_writeAmbResidual(const t_gtime& t)
	{
		ostringstream os;
		string wl_fix, nl_fix;

		//write Time
		os << " EPOCH-TIME" << setw(8) << t.mjd() << fixed << setprecision(1)
			<< setw(10) << t.sod() << endl;

		//write Data
		auto it_site = _AMB_RES.begin();
		for (it_site; it_site != _AMB_RES.end(); it_site++)
		{
			auto it_sat = it_site->second.begin();
			for (it_sat; it_sat != it_site->second.end(); it_sat++)
			{

				if (it_sat->second.wl_fixed)
				{
					wl_fix = "yes";
				}
				else
				{
					wl_fix = "no";
				}
				if (it_sat->second.nl_fixed)
				{
					nl_fix = "yes";
				}
				else
				{
					nl_fix = "no";
				}

				os << "  " << fixed << it_site->first << "  " << it_sat->first << right << setw(10) << setprecision(3) <<
					it_sat->second.wl_res << " " << left << setw(3) << wl_fix << " " << right << setw(10) << setprecision(3)
					<< it_sat->second.nl_res << " " << left << setw(3) << nl_fix << " " << endl;
			}
		}


		// Print amb res results
		if (_fambres) {
			_fambres->write(os.str().c_str(), os.str().size());
			_fambres->flush();
		}
		else {
			write_log_info(_glog, 1, " Warning[t_gupdlsq::_writeAmbResidual] : have no output file!");
		}
		return true;
	}

	bool t_gupdlsq::_writeAmbResidual_EWL()
	{
		ostringstream os;
		string ewl_fix;
		
		//write Data
		auto it_epoch= _AMB_RES_EWL.begin();
		for (it_epoch; it_epoch != _AMB_RES_EWL.end(); it_epoch++)
		{
			//write Time
			os << " EPOCH-TIME" << setw(8) << it_epoch->first << endl;

			auto it_site = it_epoch->second.begin();
			for (it_site; it_site != it_epoch->second.end(); it_site++)
			{
				auto it_sat = it_site->second.begin();
				for (it_sat; it_sat != it_site->second.end(); it_sat++)
				{
					double correct_amb = it_sat->second.ewl_res - _cal_UPD->get_upd_value(WL_IDENTIFY, it_site->first) - _cal_UPD->get_upd_value(WL_IDENTIFY, it_sat->first);
					
					if (fabs(correct_amb - round(correct_amb)) < 0.30 && it_sat->second.ewl_amb_sigma < 0.130)
					{
						it_sat->second.ewl_fixed = true;
						it_sat->second.ewl_res = correct_amb - round(correct_amb);
						ewl_fix = "yes";
					}

					else
					{
						it_sat->second.ewl_fixed = false;
						it_sat->second.ewl_res = correct_amb - round(correct_amb);
						ewl_fix = "no";
					}
					
					os << "  " << fixed << it_site->first << "  " << it_sat->first << right << setw(10) << setprecision(3) <<
						it_sat->second.ewl_res << " " << left << setw(3) << ewl_fix << " " << endl;
				}
			}

		}
		
		// Print EWL amb res results
		if (_fambres_EWL) {
			_fambres_EWL->write(os.str().c_str(), os.str().size());
			_fambres_EWL->flush();
		}
		else {
			write_log_info(_glog, 1, " Warning[t_gupdlsq::_writeAmbResidual_EWL] : have no output file!");
		}
		return true;
	}
	

	bool t_gupdlsq::_apply_DCB(t_gobsgnss& satdata, double& P, OBSCOMBIN obscombin, t_gobs* gobs1, t_gobs* gobs2)
	{
		if (!_allbias || !gobs1 || !gobs2)
		{
			return false;
		}
		t_gtime epoch = satdata.epoch();
		string sat = satdata.sat();

		t_gobs g1, g2;
		if (gobs1->attr() == ATTR) g1 = satdata.id_range(gobs1->band());  // automatic GOBS selection
		else g1 = gobs1->gobs();                                          // specific GOBS

		if (gobs2->attr() == ATTR) g2 = satdata.id_range(gobs2->band());  // automatic GOBS selection
		else g2 = gobs2->gobs();                                          // specific GOBS  

		// 2.xx to 3.xx
		g1.gobs2to3(satdata.gsys());
		g2.gobs2to3(satdata.gsys());


		GOBSBAND b1 = t_gsys::band_priority(satdata.gsys(), FREQ_1);
		GOBSBAND b2 = t_gsys::band_priority(satdata.gsys(), FREQ_2);
		GOBSBAND b3 = t_gsys::band_priority(satdata.gsys(), FREQ_3);

		// modified for correct P1 code bias
		double l1 = satdata.wavelength(b1);
		double l2 = satdata.wavelength(b2);
		double l3 = satdata.wavelength(b3);

		double c2 = l1 / l2;
		double c1 = 1.0 / (1.0 - c2 * c2);
		c2 = c2 * c2 * c1;

		double cor[5];

		double corr1 = 0.0, corr2 = 0.0;

		double p1p2 = _allbias->get(epoch, sat, P1, P2);
		double p1c1 = _allbias->get(epoch, sat, P1, C1);
		double p2c2 = _allbias->get(epoch, sat, P2, C2);

		cor[0] = c2 * p1p2;
		cor[1] = c1 * p1p2;
		cor[2] = cor[0] + p1c1;
		cor[3] = cor[1] + p2c2;
		cor[4] = cor[1] + p1c1;

		switch (g1.gobs())
		{
		case C1C:
			corr1 = cor[2];
			break;
		case C2C:
			corr1 = cor[3];
			break;
		case C1P:
		case C1Y:
		case C1W:
			corr1 = cor[0];
			break;
		case C2P:
		case C2Y:
		case C2W:
			corr1 = cor[1];
			break;
		case C2X:
			corr1 = cor[4];
			break;
		case C2D:
			break;
		default:
			return false;
			break;
		}

		switch (g2.gobs())
		{
		case C1C:
			corr2 = cor[2];
			break;
		case C2C:
			corr2 = cor[3];
			break;
		case C1P:
		case C1Y:
		case C1W:
			corr2 = cor[0];
			break;
		case C2P:
		case C2Y:
		case C2W:
			corr2 = cor[1];
			break;
		case C2X:
			corr2 = cor[4];
			break;
		case C2D:
			break;
		default:
			return false;
			break;
		}

		if (obscombin == MW_COMBIN)
		{
			double fact = l1 / l2;
			P += -(corr1 / l1 + corr2 / l2) * (1.0 - fact) / (1.0 + fact);
		}
		else if (obscombin == EWL_COMBIN)
		{
			double alpha = (l2*l3 - l1 * l1) / (l2*l2 - l1 * l1);
			P += -(corr1*(1 - alpha) + corr2 * alpha) * (l3 - l2) / (l2*l3);
		}
		else 
		{
			P += 0.0;
		}

		return true;
	}

}
