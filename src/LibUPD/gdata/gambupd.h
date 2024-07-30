/**
*
* @verbatim
History
-1.0 bwang  2019-02-21  creat the file.
-1.1 bwang  2019-04-08  Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		gambupd.h
* @brief	Storage the ambiguity upd files' data(only one site) for narrow-lane upd compute.
* @author   bwang, Wuhan University.
* @version	1.0.0
* @date		2019-04-08
*
*/

#ifndef GAMBUPD_H
#define GAMBUPD_H

#include "gutils/gfileconv.h"
#include "gdata/gdata.h"
#include "gutils/gtime.h"
#include "gutils/ExportUPD.h"

using namespace gnut;

namespace great
{
	/**
	*@brief	   Class for storaging ambupd data of one stations/one satellite/one epoch 
	*/
	class t_ambupd_data
	{
	public:

		/** @brief default constructor. */
		t_ambupd_data(){};

		/** @brief default destructor. */
		~t_ambupd_data(){};

		double amb_if = 0.0;       ///< ambiguity ionosphere-free value meter  /N1 in UCUD mode (m)
		double amb_wl = 0.0;       ///< ambiguity Wide-Lane value       cycle  /N2 in UCUD mode (m)
		double amb_wl_sigma = 0.0; ///< std of ambiguity Wide-Lane value
	};

	/** map container using satellite name as a index for storaging t_ambupd_data ptr , one station/one epoch/all satellite */
	typedef map<string, shared_ptr<t_ambupd_data>> t_sat_ambupd; 

	/** map container using epoch as a index for storaging t_sat_ambupd , one station/all epoch/all satellite */
	typedef map<t_gtime, t_sat_ambupd> t_epo_ambupd;

	/**
	*@brief	   Class for storaging ambupd data of one stations/all satellite/all epoch
	*/
	class UPD_LIBRARY_EXPORT t_gambupd : public t_gdata
	{
	public:

		/** @brief default constructor. */
		t_gambupd();

		/** @brief default destructor. */
		virtual ~t_gambupd();

		/**
		* @brief add t_ambupd_data of one station/one epoch/one satellite.
		* @param[in]  epoch		epoch.
		* @param[in]  sat		satellite prn.
		* @param[in]  data		t_ambupd_data, data of one station/one epoch/one satellite ambupd file.
		* @return	  void
		*/
		void addAmbUpd(t_gtime epoch, string sat, const t_ambupd_data& data);

		/**
		* @brief get t_ambupd_data ptr of one station/one epoch/one satellite.
		* @param[in]  epoch		epoch.
		* @param[in]  sat		satellite prn.
		* @return	  t_ambupd_data ptr, data ptr of one station/one epoch/one satellite ambupd file.
		*/
		shared_ptr<t_ambupd_data>& getSatAmbUpd(t_gtime epoch, string sat);

		/**
		* @brief get t_sat_ambupd data of one station/one epoch/all satellite.
		* @param[in]  epoch		epoch.
		* @return	  t_sat_ambupd data of one station/one epoch/all satellite.
		*/
		t_sat_ambupd& getEpoAmbUpd(t_gtime epoch);

		/**
		* @brief get t_epo_ambupd data of one station/all epoch/all satellite.
		* @return	 t_epo_ambupd data of one station/all epoch/all satellite.
		*/
		t_epo_ambupd& getAllAmbUpd();

		/**
		* @brief   return the last record of site.
		* @return  end time of the site.
		*/
		t_gtime getSatEndTime();

		set<string> getSatList() { return _sat_list; }

	protected:
		set<string>  _sat_list; ///< site name
		t_epo_ambupd _ambupd;   ///< internal variable of t_epo_ambupd storaging data 
		                        ///< of one station/all epoch/all satellite

	private:

	};
}
#endif