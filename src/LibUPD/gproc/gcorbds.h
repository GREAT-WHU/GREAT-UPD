
/**
* @verbatim
	History
	 -1.0 Guolong Feng	 2020-07-05  creat the file.
  @endverbatim
* Copyright (c) 2020, Wuhan University. All rights reserved.
*
* @file			gcorbds.h
* @author       Guolong Feng, Wuhan University
* @version		1.0.0
* @date			2020-07-05
*
*/

#ifndef CORBDS_H
#define CORBDS_H

#include "gall/gallnav.h"
#include "gall/gallobs.h"

#include "gdata/gobsgnss.h"

#include "gset/gsetgen.h"
#include "gset/gsetgnss.h"
#include "gset/gsetproc.h"
#include "gutils/ExportUPD.h"

namespace great
{
	
	// BeiDou satellite-induced code pseudorange variations correct
	class UPD_LIBRARY_EXPORT t_gcorbds
	{
	public:
		t_gcorbds(t_gsetbase* settings);
		virtual ~t_gcorbds() {};

		void apply_IGSO_MEO(const t_gtime now, t_gtriple& rec_crd, t_gallnav* gnav, vector<t_gobsgnss>& obsdata);

	private:
		vector<GOBSBAND>                     _proc_band;
		bool                                 _correct_bds_code_bias;
		// Wanninger & Beer : BeiDou satellite-induced code pseudorange variations: diagnosis and therapy [unit:m]
		map<GOBSBAND, map<string, map<int, double> > > _IGSO_MEO_Corr;

		bool _recAprCoordinate(const t_gtime& now, t_gtriple& rec_crd, t_gallnav* gnav, vector<t_gobsgnss>& obsdata);

		bool _satPosClk(const t_gtime& now, string sat, t_gallnav* gnav, double P3, t_gtriple& sat_pos, double& sat_clk);

	};


} // namespace

#endif
