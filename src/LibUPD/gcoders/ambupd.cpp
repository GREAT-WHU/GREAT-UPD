/**
*
* @verbatim
History
-1.0 bwang  2019-02-22  creat the file.
-1.1 bwang  2019-04-09  Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		ambupd.cpp
* @brief	decode and enconde ambupd file(XXXX_ambupd_yyyyddd)
*             XXXX  ---- site name
*             yyyy  ---- year
*              ddd  ---- doy
* @author   bwang, Wuhan University
* @version	1.0.0
* @date		2019-04-09
*
*/

#include"gcoders/ambupd.h"

using namespace std;
namespace great
{
	t_ambupd::t_ambupd(t_gsetbase* s, string version, int sz)
		:t_gcoder(s, version, sz)
	{
		gtrace("t_ambupd::constructor");
	}

	t_ambupd::~t_ambupd()
	{
		gtrace("t_ambupd::destructor");
	}

	int t_ambupd::decode_head(char* buff, int sz, vector<string>& errmsg)
	{
		gtrace("t_ambupd::decode_data");
#ifdef BMUTEX
		boost::mutex::scoped_lock lock(_mutex);
#endif
		_mutex.lock();

		if (t_gcoder::_add2buffer(buff, sz) == 0)
		{
			_mutex.unlock();
			return 0;
	    };

		_mutex.unlock();
		return -1;
	}

	int t_ambupd::decode_data(char* buff, int sz, int& cnt, vector<string>& errmsg)
	{
		gtrace("t_ambupd::decode_data");
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
		string sta;
		string prn;
		t_ambupd_data data_tmp;
		t_gtime epoch;
		int mjd;
		double sod;
		try
		{
			while ((tmpsize = t_gcoder::_getline(tmp, 0)) >= 0)
			{
				consume += tmpsize;
				istringstream istr(tmp);
				istr >> mjd >> sod >> sta >> prn >> data_tmp.amb_if >> data_tmp.amb_wl >> data_tmp.amb_wl_sigma;
				epoch.from_mjd(mjd, sod, sod - floor(sod));

				map<string, t_gdata*>::iterator it = _data.begin();
				while (it != _data.end())
				{
					if (it->second->id_type() == t_gdata::AMBUPD)
					{
						((t_gallambupd*)it->second)->addAmbUpd(sta, epoch, prn, data_tmp);
					}
					it++;
				}
				t_gcoder::_consume(tmpsize);
			}
		}
		catch (...)
		{
			if (_log)
			{
				_log->comment(2, "t_ambupd::decode_data", "ERROR: unknown mistake");
			}
			else
			{
				cout << "ERROR: t_ambupd::decode_data , unknown mistake" << endl;
			}
			return -1;
			throw("Decode ambupd head is Wrong!");
		}
		_mutex.unlock();
		return tmpsize;

	}

	
}