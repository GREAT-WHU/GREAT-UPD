/**
*
* @verbatim
History
-1.0 glfeng   2019-02-27 creat the file.
-1.1 bwang    2019-04-10 Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		   gsetproc.cpp
* @brief	   control set from XML
* @author      glfeng, Wuhan University
* @version	   1.0.0
* @date		   2019-04-10
*
*/

#include "gset/gsetproc.h"
#include <sstream>

using namespace std;
using namespace pugi;

namespace great
{
	t_gsetproc::t_gsetproc()
	 : t_gsetbase()
	{
		_gmutex.lock();

		_set.insert(XMLKEY_PROC);

		_gmutex.unlock();
	}

	t_gsetproc::~t_gsetproc()
	{

	}

	 void t_gsetproc::check()
	 {
		 _gmutex.lock();
		 _gmutex.unlock();
		 return;
	 }

	 void t_gsetproc::help()
	 {
		 _gmutex.lock();
		 cerr << "<process>\n"
			 << " updmode = \"NL\"             <!--EWL WL NL EWL_epoch NONE--> \n";
		 _gmutex.unlock();
		 return;
	 }

	 UPDMODE t_gsetproc::get_updtype()
	 {
		 _gmutex.lock();

		 string tmp = _doc.child(XMLKEY_ROOT).child(XMLKEY_PROC).attribute("updmode").value();

		 transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);

		 UPDMODE mode = str2updmode(trim(tmp));

		 _gmutex.unlock(); return mode;
	 }


	 bool t_gsetproc::bds_code_bias_correction()
	 {
		 _gmutex.lock();

		 bool tmp = _doc.child(XMLKEY_ROOT).child(XMLKEY_PROC).attribute("bds_code_bias_corr").as_bool();

		 _gmutex.unlock(); return tmp;
	 }
	 

	 UPDMODE str2updmode(string str)
	 {
		 str = trim(str);
		 if (str == "wl" || str == "WL")
		 {
			 return UPDMODE::WL;
		 }
		 else if (str == "ewl" || str == "EWL")
		 {
			 return UPDMODE::EWL;
		 }
		 else if (str == "ewl_epoch" || str == "EWL_epoch" || str == "EWL_EPOCH")
		 {
			 return UPDMODE::EWL_epoch;
		 }
		 else if (str == "nl" || str == "NL")
		 {
			 return UPDMODE::NL;
		 }
		 else
		 {
			 cout << "*** warning: not defined upd mode [" << str << "]\n"; cout.flush();
		 }
		 return UPDMODE::NONE;
	 }

	 string updmode2str(UPDMODE mode)
	 {
		 string str;
		 switch (mode)
		 {
		 case UPDMODE::EWL:
			 str = "EWL";
			 break;
		 case UPDMODE::EWL_epoch:
			 str = "EWL_epoch";
			 break;
		 case UPDMODE::NL:
			 str = "NL";
			 break;
		 case UPDMODE::WL:
			 str = "WL";
			 break;
		 case UPDMODE::NONE:
			 str = "NONE";
			 break;
		 default:
			 str = "NONE";
			 break;
		 }
		 return str;
	 }

}
