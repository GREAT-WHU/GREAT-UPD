/**
*
* @verbatim
History
-1.0 bwang  2019-02-20  creat the file.
-1.1 bwang  2019-04-08  Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		gambupd.cpp
* @brief	Storage the ambiguity upd files' data(only one site) for narrow-lane upd compute
* @author   bwang, Wuhan University
* @version	1.0.0
* @date		2019-04-08
*
*/

#include"gdata/gambupd.h"

using namespace std;
namespace great
{
	t_gambupd::t_gambupd() :_ambupd()
	{
		gtrace("t_gambupd::constructor");
		id_type(t_gdata::AMBUPD);
		_sat_list.clear();
	}

	t_gambupd::~t_gambupd()
	{
		gtrace("t_gambupd::destructor");
	}

	void t_gambupd::addAmbUpd(t_gtime epoch, string sat, const t_ambupd_data& data)
	{
		gtrace("t_gambupd::add_sat_ambupd");
		if (_ambupd.find(epoch) == _ambupd.end())
		{
			_ambupd[epoch][sat] = make_shared<t_ambupd_data>(data);
		}
		else if (_ambupd[epoch].find(sat) == _ambupd[epoch].end())
		{
			_ambupd[epoch][sat] = make_shared<t_ambupd_data>(data);
		}
		else
		{
			if (_ambupd[epoch][sat]->amb_if <= 0.0) _ambupd[epoch][sat]->amb_if = data.amb_if;
			if (_ambupd[epoch][sat]->amb_wl <= 0.0) _ambupd[epoch][sat]->amb_wl = data.amb_wl;
		}

		_sat_list.insert(sat);
	}

	shared_ptr<t_ambupd_data>& t_gambupd::getSatAmbUpd(t_gtime epoch, string sat)
	{
		return _ambupd[epoch][sat];
	}

	t_sat_ambupd& t_gambupd::getEpoAmbUpd(t_gtime epoch)
	{
		return _ambupd[epoch];
	}

	t_epo_ambupd& t_gambupd::getAllAmbUpd()
	{
		return _ambupd;
	}
    
	t_gtime t_gambupd::getSatEndTime()
	{
		t_gtime end_time;
		end_time = _ambupd.rbegin()->first;		
		return end_time;
	}

}