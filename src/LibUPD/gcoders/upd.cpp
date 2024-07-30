/**
*
* @verbatim
History
-1.0 bwang  2019-02-22  creat the file.
-1.1 bwang  2019-04-09  Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		upd.cpp
* @brief	decode and enconde upd file
* @author   bwang, Wuhan University
* @version	1.0.0
* @date		2019-04-09
*
*/

#include "gcoders/upd.h"

using namespace std;
namespace great {

	t_upd::t_upd(t_gsetbase* s, string version, int sz)
		:t_gcoder(s, version, sz)
	{
		gtrace("t_updlane::constructor");
		_epoch = WL_IDENTIFY;
	}

	t_upd::~t_upd()
	{

	}

	int t_upd::decode_head(char* buff, int sz, vector<string>& errmsg)
	{
#ifdef BMUTEX
		boost::mutex::scoped_lock lock(_mutex);
#endif
		_mutex.lock();

		if (t_gcoder::_add2buffer(buff, sz) == 0)
		{
			_mutex.unlock();
			return 0;
		};

		string tmp;
		int tmpsize = 0;
		int consume = 0;
		string mode_tmp;
		try
		{
			while ((tmpsize = t_gcoder::_getline(tmp, 0)) >= 0)
			{
				consume += tmpsize;
				if (tmp.substr(0, 1) == "%")
				{
					int pos = tmp.find("_");
					tmp.erase(0, pos + 1);
				    tmp.erase(tmp.end() - 1,tmp.end());
					mode_tmp = tmp;
					if (mode_tmp == "EWL" || mode_tmp == "ewl" || mode_tmp == "WL" || mode_tmp == "wl" || mode_tmp == "NL" || mode_tmp == "nl")
					{
						if (mode_tmp == "EWL" || mode_tmp == "ewl")
						{
							_epoch = EWL_IDENTIFY;
						}
						else if (mode_tmp == "WL" || mode_tmp == "wl")
						{
							_epoch = WL_IDENTIFY;
						}
						t_gcoder::_consume(tmpsize);
						break;
					}
					else
					{
						if (_log)
						{
							_log->comment(2, "t_upd::decode_head", "ERROR: unknown upd mode");
						}
						else
						{
							cout << "ERROR: t_upd::decode_head , unknown upd mode"<<endl;
						}
						t_gcoder::_consume(tmpsize);
						_mutex.unlock();
						return -1;
					}
				}
				else
				{
					_mutex.unlock();
					return -1;
				}
			}
		}
		catch (...)
		{
			if (_log)
			{
				_log->comment(2, "t_upd::decode_head", "ERROR: unknown mistake");
			}
			else
			{
				cout << "ERROR: t_upd::decode_heada , unknown mistake" << endl;
			}
			return -1;
			throw("Decode upd head is Wrong!");
		}
		_mutex.unlock();
		return consume;
	}

	int t_upd::decode_data(char* buff, int sz, int& cnt, vector<string>& errmsg)
	{
#ifdef BMUTEX
		boost::mutex::scoped_lock lock(_mutex);
#endif
		_mutex.lock();

		if (t_gcoder::_add2buffer(buff, sz) == 0)
		{
			_mutex.unlock();
			return 0;
		};

		string tmp;
		int tmpsize = 0;
		int consume = 0;
		string str;

		int mjd;
		double sod;

		string prn;
		t_updrec upd;
		try
		{
			while ((tmpsize = t_gcoder::_getline(tmp, 0)) >= 0)
			{
				istringstream istr(tmp);
				consume += tmpsize;
				if (tmp.substr(1, 10) == "EPOCH-TIME")
				{
					istr >> str >> mjd >> sod;
				    _epoch.from_mjd(mjd, sod,sod-floor(sod));
				}
				else if (tmp.substr(0, 1) == "x" || tmp.substr(0, 1) == "X")
				{
					// glfeng
					istr >> prn >> upd.value >> upd.sigma >> upd.npoint;
					upd.sigma = 10000.0;
					//fill data loop
					map<string, t_gdata*>::iterator it = _data.begin();
					while (it != _data.end()) {
						if (it->second->id_type() == t_gdata::UPD)
							((t_gupd*)it->second)->add_sat_upd(_epoch, prn.substr(1), upd);
						it++;
					}
				}
				else if (tmp.substr(0, 1) == " ")
				{
					istr >> prn >> upd.value >> upd.sigma >> upd.npoint;
					//fill data loop
					map<string, t_gdata*>::iterator it = _data.begin();
					while (it != _data.end()){
						if (it->second->id_type() == t_gdata::UPD)
						    ((t_gupd*)it->second)->add_sat_upd(_epoch, prn, upd);
						it++;
					}
				}
				else if (tmp.substr(0, 3) == "EOF")
				{
					if (_log)
					{
						_log->comment(2, "t_upd::decode_data", "WARNING: End of file" + tmp);
					}
					else
					{
						cout << "WARNING: t_upd::decode_data , End of file" << tmp << endl;;
					}
					t_gcoder::_consume(tmpsize);
					_mutex.unlock();
					return -1;
				}
				else
				{
					if (_log)
					{
						_log->comment(2, "t_upd::decode_data", "WARNING: unknown upd-data message"+tmp);
					}
					else
					{
						cout << "WARNING: t_upd::decode_data , unknown upd-data message" << tmp << endl;;
					}
					t_gcoder::_consume(tmpsize);
					_mutex.unlock();
					return -1;
				}
				t_gcoder::_consume(tmpsize);
			}
		}
		catch (...)
		{
			if (_log)
			{
				_log->comment(2, "t_upd::decode_data", "ERROR: unknown mistake");
			}
			else
			{
				cout << "ERROR: t_upd::decode_data , unknown mistake"<<endl;
			}
			return -1;
			throw("Decode upd data is Wrong!");
		}
		_mutex.unlock();
		return consume;
	}

	int t_upd::encode_head(char* buff, int sz, vector<string>& errmsg)
	{
#ifdef BMUTEX
		boost::mutex::scoped_lock lock(_mutex);
#endif
		_mutex.lock();
		try
		{
			if (_ss_position == 0)
			{
				_ss << "% UPD generated using upd_";
				//get upd head information
				map<string, t_gdata*>::iterator it = _data.begin();
				for (it = _data.begin(); it != _data.end(); ++it)
				{
					if (it->second->id_type() == t_gdata::UPD)
					{
						_updmode = dynamic_cast<t_gupd*>(it->second)->get_mode();
					}
				}
				if (_updmode != UPDMODE::NONE)
				{

					_ss << updmode2str(_updmode) << endl;
				}
				else
				{
					_ss << "Wrong mode of upd: " << updmode2str(_updmode) << endl;
					int size = _fill_buffer(buff, sz);
					_mutex.unlock();
					return -1;
				}
			}
			int size = _fill_buffer(buff, sz);
			_mutex.unlock();
			return size;
		}
		catch (...)
		{
			if (_log)
			{
				_log->comment(2, "t_upd::encode_head", "ERROR: unknown mistake");
			}
			else
			{
				cout << "ERROR: t_upd::encode_head , unknown mistake" << endl;
			}
			return -1;
			throw("Encode upd head is Wrong!");
		}
	}

	int t_upd::encode_data(char* buff, int sz, int& cnt, vector<string>& errmsg)
	{
#ifdef BMUTEX
		boost::mutex::scoped_lock lock(_mutex);
#endif
		_mutex.lock();
		try
		{
			if (_ss_position == 0)
			{
				t_gtime valid_beg;
				//get data from _data
				map<t_gtime, one_epoch_upd> upd;
				auto it = _data.begin();
				for (it = _data.begin(); it != _data.end(); ++it)
				{
					if (it->second->id_type() == t_gdata::UPD)
					{
						upd = dynamic_cast<t_gupd*>(it->second)->get_upd();
						valid_beg = dynamic_cast<t_gupd*>(it->second)->get_valid_beg();
					}
				}

				// encode
				// upd
				if (_updmode == UPDMODE::NL || _updmode == UPDMODE::EWL_epoch)
				{
					auto itUpd = upd.begin();
					for (itUpd; itUpd != upd.end(); ++itUpd)
					{

						if (itUpd->first == WL_IDENTIFY || itUpd->first < valid_beg || itUpd->second.size() == 0)// WL_IDENTIFY for wl & ewl
						{
							continue;
						}
						_ss << " EPOCH-TIME" << setw(8) << itUpd->first.mjd()
							<< setw(10) << fixed << setprecision(1) << itUpd->first.sod()
							+ itUpd->first.dsec() << endl;

						auto itepo = itUpd->second.begin();
						for (itepo; itepo != itUpd->second.end(); itepo++)
						{
							if (itepo->second->npoint <= 2 || itepo->second->sigma > 0.10)
							{
								_ss << "x";
							}
							else{
								_ss << " ";
							}
							_ss << setw(11) << left << itepo->first << fixed << right << setw(10) << setprecision(3)
								<< itepo->second->value << fixed << setw(10) << setprecision(3) << itepo->second->sigma
								<< setw(5) << itepo->second->npoint << endl;
						}
					}
				}
				else if (_updmode == UPDMODE::WL || _updmode == UPDMODE::EWL)
				{
					auto itUpd = upd.begin();
					for (itUpd; itUpd != upd.end(); ++itUpd)
					{
						// WL_IDENTIFY for wl & ewl
						if (itUpd->first != WL_IDENTIFY)continue;

						auto itepo = itUpd->second.begin();
						for (itepo; itepo != itUpd->second.end(); itepo++)
						{
							if (itepo->second->npoint <= 2 || itepo->second->sigma > 0.10)
							{
								_ss << "x";
							}
							else{
								_ss << " ";
							}
							_ss << setw(9) << left << itepo->first << fixed << right << setw(10) << setprecision(3)
								<< itepo->second->value << fixed << setw(10) << setprecision(3) << itepo->second->sigma
								<< setw(5) << itepo->second->npoint << endl;
						}
					}
				}
				else
				{
					_ss << "Wrong data" << endl;
				}

				_ss << "EOF" << endl;
			}

			int size = _fill_buffer(buff, sz);
			_mutex.unlock();
			return size;
		}
		catch (...)
		{
			if (_log)
			{
				_log->comment(2, "t_upd::encode_data", "ERROR: unknown mistake");
			}
			else
			{
				cout << "ERROR: t_upd::encode_data , unknown mistake" << endl;
			}
			return -1;
			throw("Encode upd data is Wrong!");
		}
	}

}//namespace