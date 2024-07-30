/**
*
* @verbatim
History
-1.0 glfeng bwang xjhan 2019-02-26  creat the file.
-1.1 bwang              2019-04-08  Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		gambcommon.h
* @brief	declare some classes and some mathematical method.
* @author   glfeng bwang xjhan, Wuhan University
* @version	1.0.0
* @date		2019-04-08
*
*/
#ifndef GAMBCOMMON_H
#define GAMBCOMMON_H 

#include "gutils/ExportUPD.h"
#include <string>
#include <map>
#include <memory>
#include <tuple>
#include <list>
#include "gutils/gtime.h"
#include "gutils/gtypeconv.h"

using namespace std;
using namespace gnut;

namespace great
{
	/**
	*@ brief zero-difference ambiguity structure  for arc.
	*/
	class UPD_LIBRARY_EXPORT t_oneway_ambiguity
	{
	public:

		/** @brief default constructor. */
		t_oneway_ambiguity();

		/** @brief default destructor. */
		virtual ~t_oneway_ambiguity() {};

		string ambtype;  ///< C/1/2/W
		string sat;
		int    ipt     = 0;         ///< index of par
		int    beg_epo = 0;     ///< start time    
		int    end_epo = 0;     ///< end time
		double rwl   = 0.0;     ///< real value widelane(cyc) jdhuang : remove warnning
		double srwl  = 0.0;     ///< its sigma
		double rewl  = 0.0;     ///< real value extrawidelane(cyc)
		double srewl = 0.0;     ///< its sigma
		double rlc  = 0.0;      ///< real value lc from slution(m)
		double srlc = 0.0;     ///< its sigma
		double ele  = 0.0;      ///< elevation
		double r    = 0.0;        ///< ambiguity in UCUD mode
		double sr   = 0.0;       ///< sigma of ambiguity
	};

	/**
	* @brief  single-differece ambiguity  for arc.
	*/
	class t_dd_ambiguity
	{
	public:

		/** @brief default constructor. */
		t_dd_ambiguity() {};

		/** @brief default destructor. */
		virtual ~t_dd_ambiguity() {};

		string ambtype;           ///< C/1/2/W
		//bool selected;            ///< selected
		bool isWlFixed = false;            ///< widelane fixed
		bool isNlFixed = false;            ///< narrowlane fixed
		vector<tuple<string, int, int>> ddSats;   ///< sat_name, index in all_pars, index in amb_pars
		t_gtime beg_epo;
		t_gtime end_epo;
		double rwl_R1 = 0.0;
		double rwl_R2 = 0.0;		///< real widelane for glonass
		double srwl_R1= 0.0;
		double srwl_R2= 0.0;		///< real widelane sigma for glonass
		double rwl = 0.0;
		double srwl = 0.0;			///< real widelane and its sigma
		double rnl = 0.0;
		double srnl = 0.0;			///< real narrowlane and its sigma
		double rlc = 0.0;
		double srlc = 0.0;			///< lc ambiguity and its sigma
		int iwl = 0;
		int inl = 0;				///< integer wide - and narrow lane
		double factor = 0.0;
		double sd_rnl_cor = 0.0;        ///< correction
		double sd_r1_cor = 0.0;			///< correction of N1
		double sd_r2_cor = 0.0;			///< correction of N2
		double sigcor = 0.0;            ///<sigma
	};

	/**
	* @brief ambiguity info for one site one satellite one epoch  used in nl upd.
	*/
	class UPD_LIBRARY_EXPORT epoch_amb
	{
	public:
		/** @brief default constructor. */
		epoch_amb();

		/** @brief default destructor. */
		virtual ~epoch_amb();

		t_gtime epo;           ///< current epoch
		bool isnewarc = false;         ///< new arc or not
		int nepo = 0;
		double bc  = 0.0;
		double sbc = 0.0;        ///< IF ambiguity value/sigma
		double bw  = 0.0;
		double sbw = 0.0;
		double bw0 = 0.0;   ///< Widelane ambiguitiy value/sigma/init value
		double bn  = 0.0;             ///< narrowlane ambiguity
		double elev = 0.0;
		double bwi  = 0.0;
	};

	/** @brief ambiguity residual. */
	class UPD_LIBRARY_EXPORT  epoch_amb_res
	{
	public:
		/** @brief default constructor. */
		epoch_amb_res();

		/** @brief default destructor. */
		virtual ~epoch_amb_res();
		bool wl_fixed = false;
        bool ewl_fixed = false;
		bool nl_fixed = false;
		double nl_res = 0.0;
		double wl_res = 0.0;
        double ewl_res = 0.0;
        double ewl_amb_sigma = 0.0;

	};

	// UPD
	/** @brief map for storaging ambiguity info ,t_epoch_amb[site][res] = epoch_amb. */
	typedef map<string, map<string, epoch_amb> > UPD_LIBRARY_EXPORT  t_epoch_amb;

	/** @brief map for storaging ambiguity residual ,t_epoch_amb_res[site][res] = epoch_amb_res. */
	typedef map<string, map<string, epoch_amb_res> > UPD_LIBRARY_EXPORT t_epoch_amb_res;

    typedef map<int,map<string, map<string, epoch_amb_res> >> UPD_LIBRARY_EXPORT t_all_epoch_amb_res;
	// PPP AR/ WL+EWL UPD
	/** @brief map for storaging oneway_ambiguity  */
	typedef vector<shared_ptr<t_oneway_ambiguity>> UPD_LIBRARY_EXPORT t_OW_amb;  //satellites OW;

	/** @brief map for storaging oneway_ambiguity , one station/all satellite/all epoch. */
	typedef map<string, t_OW_amb> UPD_LIBRARY_EXPORT t_all_sats_OW;  //satellites OW;

	//** @brief vector for storaging double-differece ambiguity. */
	typedef vector<t_dd_ambiguity> UPD_LIBRARY_EXPORT t_DD_amb;

	// WL/EWL UPD
	//** @brief map for storaging ambiguity value, one station/all epo/all satellite. */
	typedef map<int, map<string, double> > UPD_LIBRARY_EXPORT t_epo_sat_amb;

	// MW in PPP
	//** @brief map for storaging ambiguity valid time. */
	typedef map<string, vector<pair<t_gtime, t_gtime> > > UPD_LIBRARY_EXPORT t_map_amb_time;

	//** @brief map for storaging MW obs value, one station/all epo/all satellite/idx=(1,2,3,4,5). */
	typedef map<t_gtime, map<string, map<int, double> > > UPD_LIBRARY_EXPORT t_map_MW;

	/**
	* @brief Compute mean of the fractional ambiguities.
	* @param[in]  values	list pairs contain data and it's weight.
	* @param[in]  mean	    data mean.
	* @param[in]  sigma	    data sigma.
	* @param[in]  sigx	    sigma's sigma.
	* @param[out] mean
	* @param[out] sigma
	* @param[out] sigx
	* @return	  void
	*/
	UPD_LIBRARY_EXPORT void getMeanFract(list<pair<double, double>>& values, double& mean, double& sigma, double& sigx);

	/**
	* @brief Get mean, sigma and sigma of the mean of a set sampled data.
	* @param[in]  is_edit   whether to eliminate errors.
	* @param[in]  wx     	list pairs contain data and it's weight.
	* @param[in]  mean	    data mean.
	* @param[in]  sigma	    data sigma.
	* @param[in]  sigx	    sigma's sigma.
	* @param[out] mean
	* @param[out] sigma
	* @param[out] sigx
	* @return	  void
	*/
	UPD_LIBRARY_EXPORT void getMeanWgt(bool is_edit, list<pair<double, double>>& wx, double& mean, double& sigma, double& mean_sig);

	/**
	* @brief Adjust ambiguities between -0.5~0.5 cycle.
	* @param[in]  x     real value
	* @param[in]  min   [0,1), [-0,5,0.5), [-1.0,1.0)
	* @return	  fraction of ambiguities
	*/
	UPD_LIBRARY_EXPORT double getFraction(double x, double min);

}

#endif
