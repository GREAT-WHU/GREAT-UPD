/**
*
* @verbatim
History
-1.0 bwang  2019-02-23  creat the file.
-1.1 bwang  2019-04-08  Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		gupd.h
* @brief	Storage the upd files' data include wide-lane, narrow-lane and ex-wide-lane
* @author   bwang, Wuhan University
* @version	1.0.0
* @date		2019-04-08
*
*/

#ifndef GUPD_H
#define GUPD_H

#include "gdata/gdata.h"
#include "gutils/gtime.h"
#include "gset/gsetproc.h"
#include "gutils/ExportUPD.h"

using namespace gnut;

#define EWL_IDENTIFY 666666  ///< the sign epoch for extra-wide-lane upd
#define WL_IDENTIFY  999999  ///< the sign epoch for wide-lane upd

namespace great
{
	/**
	*@brief	   Class for storaging one satellite upd data
	*/
	class UPD_LIBRARY_EXPORT t_updrec
	{
	public:

		/** @brief default constructor. */
		t_updrec();

		/** @brief default destructor. */
		~t_updrec() {};

		string  obj;      ///< upd objection may be site or satellite
		int     npoint;   ///< site number
		double  value;    ///< upd value
		double  sigma;    ///< std
		bool    isRef;    ///< true set as a reference
	};

	/** map container using satellite name as a index for storaging t_updrec ptr , one epoch/all satellite
	*   for wide-lane only one sign epoch "WL_IDENTIFY"   */
	typedef map<string, shared_ptr<t_updrec>> UPD_LIBRARY_EXPORT one_epoch_upd;

	/**
	*@brief	 Class for storaging all epoch/all satellite upd data
	*/
	class UPD_LIBRARY_EXPORT t_gupd : public t_gdata
	{
	public:

		/** @brief default constructor. */
		t_gupd();

		/** @brief default destructor. */
		virtual ~t_gupd();

		/**
		* @brief add upd data of one epoch/one satellite.
		* @param[in]  epoch		  epoch.
		* @param[in]  prn		  satellite name.
		* @param[in]  one_sat_upd upd data of one epoch/one satellite
		* @return	  void
		*/
		void add_sat_upd(t_gtime epoch, string prn, t_updrec one_sat_upd);

		/**
		* @brief add upd data of one epoch/all satellite.
		* @param[in]  epoch		  epoch.
		* @param[in]  one_sat_upd upd data of one epoch/all satellite
		* @return	  void
		*/
		void add_epo_upd(t_gtime epoch, one_epoch_upd one_epoch_upd);

		/**
		* @brief set upd mmode, supported EWL/WL/NL currently.
		* @param[in]  mode		  upd mode , EWL/WL/NL.
		* @return	  void
		*/
		void set_mode(UPDMODE mode);

		/**
		* @brief get upd mode, supported EWL/WL/NL currently.
		* @return	 upd mode
		*/
		UPDMODE get_mode();

		/** 
		* @brief get upd data, all epoch/all satellite. 
		* @return	 upd data
		*/
		map<t_gtime, one_epoch_upd>& get_upd(){ return _upd; };

		/** 
		* @brief get upd data's value in class t_updrec,one epoch/one satellite.
		* @param[in] t epoch time
		* @param[in] str satellite name
		* @return the upd value of satellite in epoch
		*/
		double get_upd_value(const t_gtime& t, const string& str) { return _upd[t][str]->value; };

		/**
		* @brief get upd data's sigma in class t_updrec,one epoch/one satellite. 
		* @param[in] t epoch time
		* @param[in] str satellite name
		* @return the upd sigma of satellite in epoch
		*/
		double get_upd_sigma(const t_gtime& t, const string& str) { return _upd[t][str]->sigma; };

		/**
        * @brief get upd data's npoint in class t_updrec,one epoch/one satellite.
        * @param[in] t epoch time
        * @param[in] str satellite name
        * @return number of sites used in this sat's upd estimation
        */
		double get_upd_npoint(const t_gtime& t, const string& str) { return _upd[t][str]->npoint; };


		/**
		* @brief get upd data of one epoch/all satellite.
		* @param[in] t epoch time
		* @param[in] str satellite name
		* @return the upd data in epoch
		*/
		//one_epoch_upd& get_epo_upd(const t_gtime& t) { return _upd[t]; };
		one_epoch_upd& get_epo_upd(const t_gtime& t); // add leo


		/**
		* @brief reset upd data's value in class t_updrec of one epoch/one satellite.
		* @param[in]  t		  epoch.
		* @param[in]  str	  satellite name.
		* @param[in]  value	  value that upd data's value will reset.
		* @param[in]  sigma	  std that upd data's sigma will reset.
		* @param[in]  npoint  site number that upd data's npoint will reset.
		* @return	  void
		*/
		void reset_upd(const t_gtime& t, const string& str, const double& value,
			const double& sigma, const int& npoint);

		/**
		* @brief reset upd data's value in class t_updrec of one epoch/one satellite.
		* @param[in]  t		  epoch.
		* @param[in]  str	  satellite name.
		* @param[in]  value	  value that upd data's value will reset.
		* @return	  void
		*/
		void reset_upd_value(const t_gtime& t, const string& str, const double& value);

		/**
		* @brief reinitialize upd data of one epoch/one satellite.
		* @param[in]  t		  epoch.
		* @param[in]  str	  satellite name.
		* @return	  void
		*/
		void re_init_upd(const t_gtime& t, string str);  //str maybe site/sats

		/**
		* @brief delete upd data of one epoch/one satellite.
		* @param[in]  t		  epoch.
		* @param[in]  str	  satellite name.
		* @return	  void
		*/
		void delete_upd(const t_gtime& t, const string& str) { _upd[t].erase(str); };

		/**
		* @brief judge upd data is usable or not, one epoch/one satellite.
		* @param[in]  t		  epoch.
		* @param[in]  str	  satellite name.
		* @return	  void
		*/
		bool upd_usable(const t_gtime& t, const string& str);

		/**
		* @brief reset upd data's value in class t_updrec of one epoch/one satellite.
		* @param[in]  pre_t	      previous epoch.
		* @param[in]  str	      satellite name.
		* @param[in]  current_t	  current epoch.
		* @param[in]  is_first	  if it's first epoch , re-init upd data.
		* @param[in]  is_site     if it's site upd, remove old upd,add new upd.
		* @return	  void
		*/
	    void copy_upd(const t_gtime& pre_t, const string& str, const t_gtime& current_t,
			const bool& is_first, const bool& is_site);

		/**
		* @brief set the valid begin epoch.
		* @param[in]  t	    current epoch.
		* @return	  void
		*/
		void set_valid_beg(const t_gtime& t);

		/**
		* @brief get the valid begin epoch.
		* @return the valid begin epoch of upd
		*/
		t_gtime get_valid_beg(){ return _valid_beg; }
		
	protected:

		map<t_gtime, one_epoch_upd> _upd;       ///< upd map container of all epoch/all satellite
		UPDMODE                      _mode;      ///< upd mode
		t_gtime                     _valid_beg; ///< valid begin epoch

	private:

	};

}
#endif // !GALLPLANETEPH_H