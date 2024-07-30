
/* ----------------------------------------------------------------------
 * G-Nut - GNSS software development library
 * 
  (c) 2018 G-Nut Software s.r.o. (software@gnutsoftware.com)
  
  (c) 2011-2017 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  This file is part of the G-Nut C++ library.
 
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 3 of
  the License, or (at your option) any later version.
 
  This library is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, see <http://www.gnu.org/licenses>.

-*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <memory> 

#include "gcoders/biabernese.h"
#include "gutils/gtypeconv.h"
#include "gmodels/gbias.h"

using namespace std;

namespace gnut {


t_biabernese::t_biabernese( t_gsetbase* s, string version, int sz, string id )
 : t_gcoder( s, version, sz, id )
{   
  gtrace(_class+"::construct");
  _allbias = 0;
  _ac = "";
  _beg = FIRST_TIME;
  _end = LAST_TIME;
}

int t_biabernese::decode_head(char* buff, int sz, vector<string>& errmsg)
{
#ifdef BMUTEX
  boost::mutex::scoped_lock lock(_mutex);
#endif

	gtrace("t_biabernese::decode_head");

	_mutex.lock();

	if (t_gcoder::_add2buffer(buff, sz) == 0) { _mutex.unlock(); return 0; };

	string line;
	int consume = 0;
	int tmpsize = 0;
	size_t idx = 0;
	string year = "";
	string mon = "";

	while ((tmpsize = t_gcoder::_getline(line)) >= 0) {
		consume += tmpsize;
		if (line.find("CODE'S MONTHLY") != string::npos) {
			_ac = "CODE";
			// change for another header of DCB files glfeng
			if ((idx = line.find("YEAR ")) != string::npos) {
				year = line.substr(idx + 5, 4);
				mon = line.substr(idx + 17, 2);
			}
			// add for another header of DCB files glfeng
			if ((idx = line.find("YEAR-MONTH")) != string::npos) {
				if (line.substr(idx + 11, 1) > "8") year = "19" + line.substr(idx + 11, 2);
				else year = "20" + line.substr(idx + 11, 2);
				mon = line.substr(idx + 14, 2);
			}
			int yyyy = str2int(year);
			int mm = str2int(mon);
			_beg.from_ymdhms(yyyy, mm, 1, 0, 0, 0);
			_end.from_ymdhms(yyyy, mm + 1, 1, 0, 0, 0); _end = _end - 1;
			t_gcoder::_consume(tmpsize);
			_mutex.unlock(); return -1;
		}
		else if (line.find("CODE'S 30-DAY") != string::npos){
			_ac = "CODE";
			string end_doy = "";
			if ((idx = line.find("ENDING DAY")) != string::npos) {
				year = line.substr(idx + 16, 4);
				end_doy = line.substr(idx + 11, 3);
			}
			string time_str = year + ":" + end_doy;
			_end.from_str("%Y:%j", time_str);
			_beg = _end - 86400 * 30;
			t_gcoder::_consume(tmpsize);
			_mutex.unlock(); return -1;
		}
		t_gcoder::_consume(tmpsize);
	}
	_mutex.unlock(); return consume;
}
  
int t_biabernese::decode_data(char* buff, int sz, int& cnt, vector<string>& errmsg)
{
	if(_ac == "CODE")
	{
		return _decode_data_CODE(buff, sz, cnt, errmsg);
	}
	else
	{
		return -1;
	}
}

int t_biabernese::_decode_data_CODE(char* buff, int sz, int& cnt, vector<string>& errmsg)
{
#ifdef BMUTEX
		boost::mutex::scoped_lock lock(_mutex);
#endif
	_mutex.lock();

	if (t_gcoder::_add2buffer(buff, sz) == 0) { _mutex.unlock(); return 0; };


	string line;
	int consume = 0;
	int tmpsize = 0;
	bool complete = false;

	GOBS gobs1, gobs2;
	gobs1 = gobs2 = X;

	while ((tmpsize = t_gcoder::_getline(line)) >= 0) {
		complete = false;

		size_t idx = 0;
		string signals = "";
		if ((idx = line.find("DIFFERENTIAL (")) != string::npos) {
			signals = line.substr(idx + 14, 5);
			gobs1 = str2gobs(signals.substr(0, 2));
			gobs2 = str2gobs(signals.substr(3, 2));
			consume += t_gcoder::_consume(tmpsize);
		}

		string prn = "";
		double dcb, std;
		dcb = std = 0.0;

		istringstream istr(line);
		istr >> prn >> dcb >> std;
		if (!istr.fail()) {
			complete = true;
		}
		else {
			consume += t_gcoder::_consume(tmpsize);
			continue;
		}

		if (complete) {

			map<string, t_gdata*>::iterator it = _data.begin();
			while (it != _data.end()) {
				if (it->second->id_type() == t_gdata::ALLBIAS) {
					shared_ptr<t_gbias> pt_bias = make_shared<t_gbias>();
					pt_bias->set(_beg, _end, dcb* 1e-9 * CLIGHT, gobs1, gobs2);
					((t_gallbias*)it->second)->add(_ac, _beg, prn, pt_bias);
				}
				it++;
			}
			consume += t_gcoder::_consume(tmpsize);
		}
	}

	_mutex.unlock(); return consume;
}

} // namespace
