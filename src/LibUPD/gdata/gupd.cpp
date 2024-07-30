/**
*
* @verbatim
History
-1.0 bwang  2019-02-23  creat the file.
-1.1 bwang  2019-04-08  Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		gupd.cpp
* @brief	Storage the upd files' data include wide-lane, narrow-lane and ex-wide-lane
* @author   bwang, Wuhan University
* @version	1.0.0
* @date		2019-04-08
*
*/

#include"gdata/gupd.h"

using namespace std;
namespace great {

	t_updrec::t_updrec()
	{
		obj = " ";
		npoint = 0;
		value = 0.0;
		sigma = 1E4;
		isRef = false;
	}

	t_gupd::t_gupd(){
		gtrace("t_gupdlane::constructor");
		id_type(t_gdata::UPD);

	}

	t_gupd::~t_gupd()
	{
		_upd.clear();
	}

	void t_gupd::add_sat_upd(t_gtime epoch, string prn, t_updrec one_sat_upd)
	{
		_upd[epoch][prn] = make_shared<t_updrec>(one_sat_upd);
	}

	void t_gupd::add_epo_upd(t_gtime epoch, one_epoch_upd one_epo_upd){
		_upd[epoch] = one_epo_upd;
	}

	void t_gupd::set_mode(UPDMODE mode)
	{
		_mode = mode;
	}

	UPDMODE t_gupd::get_mode()
	{
		return _mode;
	}

	bool t_gupd::upd_usable(const t_gtime& t, const string& str)
	{
		try
		{
			bool upd_usable;
			if (_mode == UPDMODE::NL) {
				upd_usable = (_upd[t][str]->sigma < 0.100 && _upd[t][str]->npoint >= 3)
					|| _upd[t][str]->isRef || _upd[t][str]->sigma <= 1e-4;
			}
			else
			{
				upd_usable = (_upd[t][str]->sigma < 0.120 && _upd[t][str]->npoint >= 2)
					|| _upd[t][str]->isRef || _upd[t][str]->sigma <= 5e-4;
			}

			return upd_usable;
		}
		catch (...)
		{
			cout << "ERROR:t_gupd::upd_usable wrong!!!" << endl;
			throw("t_gupd::upd_usable is Wrong!");
		}
	}

	void t_gupd::re_init_upd(const t_gtime& t, string str)
	{
		t_updrec one_upd;
		one_upd.npoint = 0;
		one_upd.value = 0.0;
		one_upd.sigma = 1E4;
		one_upd.isRef = false;
		t_gupd::add_sat_upd(t, str,  one_upd);
	}

	one_epoch_upd& t_gupd::get_epo_upd(const t_gtime& t)
	{
		auto it_epo = _upd.begin();

		if (_upd.find(t) != _upd.end())
		{
			return _upd[t];
		}
		else
		{
			for (; it_epo != _upd.end(); it_epo++)
			{
				if (it_epo->first > t) break;
			}
			it_epo--;
			cout << " Warning : Use UPD at time " + it_epo->first.str_ymdhms() 
				+ " to replace UPD at time " + t.str_ymdhms() << endl;
			return it_epo->second;			
		}
	}

	void t_gupd::reset_upd(const t_gtime& t, const string& str, const double& value,
		const double& sigma, const int& npoint)
	{
		_upd[t][str]->npoint = npoint;
		_upd[t][str]->value = value;
		_upd[t][str]->sigma = sigma;
	}

	void t_gupd::reset_upd_value(const t_gtime& t, const string& str, const double& value)
	{
		_upd[t][str]->value = value;
	}

	void t_gupd::copy_upd(const t_gtime& pre_t, const string& str, const t_gtime& current_t, 
		const bool& is_first, const bool& is_site)
	{
		if (is_first == true)
		{
			t_gupd::re_init_upd(current_t, str);
		}
		else
		{
			t_updrec one_upd;
			one_upd.npoint = _upd[pre_t][str]->npoint;
			one_upd.value = _upd[pre_t][str]->value;
			one_upd.sigma = _upd[pre_t][str]->sigma;
			one_upd.isRef = _upd[pre_t][str]->isRef;
			if (is_site) _upd[pre_t].erase(str);
			t_gupd::add_sat_upd(current_t, str, one_upd);
		}
		return;
	}

	void t_gupd::set_valid_beg(const t_gtime& t) 
	{ 
		if(_valid_beg > t) _valid_beg = t; 
	}

}//namespace