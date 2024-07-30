/**
*
* @verbatim
History
-1.0 glfeng   2019-02-28 creat the file.
-1.1 bwang   2019-04-10 Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		   gcfg_upd.cpp
* @brief	   control set from XML
* @author      glfeng, Wuhan University
* @version	   1.0.0
* @date		   2019-04-10
*
*/

#include "gset/gcfg_upd.h"

using namespace std;
using namespace pugi;
using namespace gnut;
namespace great
{
	t_gcfg_upd::t_gcfg_upd()
		: t_gsetbase(),
		t_gsetgen(),
		t_gsetinp(),
		t_gsetout(),
		t_gsetgnss(),
		t_gsetproc()
	{
		_IFMT_supported.insert(AMBFLAG_INP);
		_IFMT_supported.insert(AMBUPD_INP);
		_IFMT_supported.insert(UPD_INP);
		_IFMT_supported.insert(RINEXO_INP);
		_IFMT_supported.insert(BIABERN_INP);
		_IFMT_supported.insert(RINEXN_INP);
		_IFMT_supported.insert(IFCB_INP);
		_IFMT_supported.insert(SATPARS_INP);

		_OFMT_supported.insert(UPD_OUT);
		_OFMT_supported.insert(LOG_OUT);
	}

	t_gcfg_upd::~t_gcfg_upd()
	{

	}

	void t_gcfg_upd::check()
	{
		t_gsetgen::check();
		t_gsetinp::check();
		t_gsetout::check();
		t_gsetgnss::check();
		t_gsetproc::check();
	}

	void t_gcfg_upd::help()
	{
		t_gsetbase::help_header();
		t_gsetgen::help();
		t_gsetinp::help();
		t_gsetout::help();
		t_gsetgnss::help();
		t_gsetproc::help();
		t_gsetbase::help_footer();
	}



}