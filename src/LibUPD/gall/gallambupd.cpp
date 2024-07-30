/**
*
* @verbatim
History
-1.0 bwang  2019-02-22  creat the file.
-1.1 bwang  2019-04-09  Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		  gallambupd.cpp
* @brief	  Storage the ambiguity upd files' data(more than one site) for narrow-lane upd compute
* @author     bwang, Wuhan University
* @version	  1.0.0
* @date		  2019-04-09
*
*/

#include "gall/gallambupd.h"

using namespace std;

namespace great {

	t_gallambupd::t_gallambupd() :_allambupd()
	{
		gtrace("t_gallambupd::constructor");
		id_type(t_gdata::AMBUPD);
	}

	t_gallambupd::~t_gallambupd()
	{
		gtrace("t_gallambupd::destructor");
		_allambupd.clear();
	}

	void t_gallambupd::addAmbUpd(string sta, const t_gambupd& ambupd)
	{
		gtrace("t_gallambupd::add_ambupd");
		_allambupd[sta] = ambupd;
	}

	void t_gallambupd::addAmbUpd(string sta,t_gtime epoch, string sat, const t_ambupd_data& data)
	{
		gtrace("t_gallambupd::add_ambupd");
		_allambupd[sta].addAmbUpd(epoch, sat, data);
		
	}

	t_gambupd& t_gallambupd::getOneSiteAmbUpd(string sta)
	{
		gtrace("t_gallambupd::get_one_ambupd");
		return _allambupd[sta];
	}

	t_sat_ambupd& t_gallambupd::getOneSiteOneEpoAmbUpd(string sta, t_gtime t)
	{
		gtrace("t_gallambupd::get_sat_ambupd");
		return _allambupd[sta].getEpoAmbUpd(t);
	}

	map<string, t_gambupd>& t_gallambupd::getAllAmbUpd()
	{
		gtrace("t_gallambupd::get_all_ambupd");
		return _allambupd;
	}

	shared_ptr<t_ambupd_data>& t_gallambupd::getOneSiteOneEpoOneSatAmbUpd(t_gtime epoch, string sta, string sat)
	{
		gtrace("t_gallambupd::get_only_ambupd");
		return _allambupd[sta].getSatAmbUpd(epoch, sat);
	}

	bool t_gallambupd::isAmbUpdValid(t_gtime epoch, string sta, string sat)
	{
		if (_allambupd[sta].getSatAmbUpd(epoch, sat) == NULL) return false;
		else return true;
	}

	set<string> t_gallambupd::get_sites()
	{
		set<string> sites;
		for (auto it = _allambupd.begin(); it != _allambupd.end(); it++)
		{
			sites.insert(it->first);
		}
		return sites;
	}

	set<string> t_gallambupd::get_sats()
	{
		set<string> tmp, sat_list;
		for (auto it = _allambupd.begin(); it != _allambupd.end(); it++)
		{
			tmp = it->second.getSatList();
			sat_list.insert(tmp.begin(),tmp.end());
		}
		return sat_list;
	}

  
	map<string, t_gtime> t_gallambupd::getSitesEndRecord()
	{
		map<string, t_gtime> site_end;
		for (auto it = _allambupd.begin(); it != _allambupd.end(); it++)
		{
			site_end[it->first] = it->second.getSatEndTime();
		}
		return site_end;
	}
}