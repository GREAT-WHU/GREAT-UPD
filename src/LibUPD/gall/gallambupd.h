/**
*
* @verbatim
History
-1.0 bwang  2019-02-22  creat the file.
-1.1 bwang  2019-04-09  Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		  gallambupd.h
* @brief	  Storage the ambiguity upd files' data(more than one site) for narrow-lane upd compute
* @author     bwang, Wuhan University
* @version	  1.0.0
* @date		  2019-04-09
*
*/
#ifndef GALLAMBUPD_H
#define GALLAMBUPD_H

#include <vector>

#include "gdata/gambupd.h"
#include "gutils/gtime.h"

using namespace std;
using namespace gnut;

namespace great
{

	/**
	*@brief	   Class for all ambupd file data of all stations storaging
	*/
	class UPD_LIBRARY_EXPORT t_gallambupd : public t_gdata
	{

	public:

		/** @brief default constructor. */
		t_gallambupd();

		/** @brief default destructor. */
		virtual ~t_gallambupd();

		/** @brief map for storaging upd of all stations. */
		typedef map<string, t_gambupd> t_map_allambupd;

		/**
		* @brief add all epochs ambupd data of one station.
		* @param[in]  sta		station's name.
		* @param[in]  ambupd	ambupd data of one station.
		* @return	  void
		*/
		void addAmbUpd(string site_name, const t_gambupd& ambupd);

		/**
		* @brief add one epoch ambupd data of one stationn.
		* @param[in]  sta		station's name.
		* @param[in]  epoch		epoch.
		* @param[in]  sat		satellite's name.
		* @param[in]  data	    class for one station/one epoch/one satellite ambupd data.
		* @return	  void
		*/
		void addAmbUpd(string site_name, t_gtime epoch, string sat_name, const t_ambupd_data& data);//one station/one epoch/one sat


		/**
		* @brief get all epoch ambupd data of one station.
		* @param[in]  sta		station's name.
		* @return	  all epoch ambupd data of one station
		*/
		t_gambupd& getOneSiteAmbUpd(string site_name); //one station

		/**
		* @brief get all satellite ambupd data of one station and one epoch.
		* @param[in]  sta		station's name.
		* @param[in]  t  		epoch
		* @return	  all satellite ambupd data of one station and one epoch
		*/
		t_sat_ambupd& getOneSiteOneEpoAmbUpd(string site_name, t_gtime epoch);  //one sation one time

		/**
		* @brief get one satellite/one epoch ambupd data of one station.
		* @param[in]  epoch     epoch
		* @param[in]  sta		station's name.
		* @param[in]  sat		satellite's name.
		* @return	  one satellite/one epoch ambupd data of one station
		*/
		shared_ptr<t_ambupd_data>& getOneSiteOneEpoOneSatAmbUpd(t_gtime epoch, string site_name, string sat_name);// one epoch / one station / one satellite

		/**
		* @brief get all station ambupd file data.
		* @return all station ambupd file data
		*/
		t_map_allambupd& getAllAmbUpd();     // all stations /all epochs / all satelltes

		/**
		* @brief judge if the ambupd usable.
		* @param[in]  epoch     epoch
		* @param[in]  sta		station's name.
		* @param[in]  sat		satellite's name.
		* @return     true  usable/ false not usable
		*/
		bool isAmbUpdValid(t_gtime epoch, string site_name, string sat_name);

		/**
		* @brief  get all sites' name of all ambupd file data.
		* @return  string container of site list
		*/
		set<string> get_sites();

		/**
		* @brief  get all sats' name of all ambupd file data.
		* @return  string container of site list
		*/
		set<string> get_sats();
 
		/** @brief return the last record of sites. */
		map<string, t_gtime> getSitesEndRecord();

	protected:
		t_map_allambupd _allambupd;///< map of station name and ambupd data for all epochs and all satellites
	};

} // namespace

#endif
