/**
*
* @verbatim
History
-1.0 bwang  2019-02-22  creat the file.
-1.1 bwang  2019-04-09  Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		ifcb.cpp
* @brief	decode and enconde ifcb file
* @author   bwang, Wuhan University
* @version	1.0.0
* @date		2019-04-09
*
*/

#include "gcoders/ifcb.h"

using namespace std;
namespace great {

	t_ifcb::t_ifcb(t_gsetbase* s, string version, int sz)
		:t_gcoder(s, version, sz)
	{
		gtrace("t_ifcb::constructor");
	}

	t_ifcb::~t_ifcb()
	{

	}

	int t_ifcb::decode_head(char* buff, int sz, vector<string>& errmsg)
	{
		gtrace("t_ifcb::decode_head");
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

	int t_ifcb::decode_data(char* buff, int sz, int& cnt, vector<string>& errmsg)
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
		string str; // no use

		int mjd;
		double sod;

		string prn;
		t_ifcbrec ifcb;
		try
		{
			while ((tmpsize = t_gcoder::_getline(tmp, 0)) >= 0)
			{
				istringstream istr(tmp);
				consume += tmpsize;
				if (tmp.substr(1, 10) == "EPOCH-TIME")
				{
					istr >> str >> mjd >> sod;
					_epoch.from_mjd(mjd, sod, sod - floor(sod));
				}
				else if (tmp.substr(0, 1) == "x" || tmp.substr(0, 1) == "X")
				{
					//cerr << "warning: data can not use :" << tmp << endl;
				}
				else if (tmp.substr(0, 1) == " ")
				{
					istr >> prn >> ifcb.value >> ifcb.sigma >> ifcb.npoint;
					//fill data loop
					map<string, t_gdata*>::iterator it = _data.begin();
					while (it != _data.end()){
						if (it->second->id_type() == t_gdata::IFCB)
							((t_gifcb*)it->second)->add_sat_ifcb(_epoch, prn, ifcb);
						it++;
					}
				}
				else if (tmp.substr(0, 3) == "EOF")
				{
					if (_log)
					{
						_log->comment(2, "t_ifcb::decode_data", "WARNING: End of file" + tmp);
					}
					else
					{
						cout << "WARNING: t_ifcb::decode_data , End of file" << tmp << endl;;
					}
					t_gcoder::_consume(tmpsize);
					_mutex.unlock();
					return -1;
				}
				else
				{
					if (_log)
					{
						_log->comment(2, "t_ifcb::decode_data", "WARNING: unknown ifcb-data message" + tmp);
					}
					else
					{
						cout << "WARNING: t_ifcb::decode_data , unknown ifcb-data message" << tmp << endl;;
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
				_log->comment(2, "t_ifcb::decode_data", "ERROR: unknown mistake");
			}
			else
			{
				cout << "ERROR: t_ifcb::decode_data , unknown mistake" << endl;
			}
			return -1;
			throw("Decode ifcb data is Wrong!");
		}
		_mutex.unlock();
		return consume;
	}

	

}//namespace