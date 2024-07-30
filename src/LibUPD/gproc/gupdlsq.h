/**
*
* @verbatim
History
-1.0 bwang  2019-02-22  creat the file.
-1.1 glfeng  2019-03-02  Adjusted some functions and variables.
-1.2 bwang  2019-04-09  Adding Doxygen Style Code Remarks.
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		gupdlsq.h
* @brief	compute Wide-Lane, Ex-Wide-Lane and Narrow-Lane upd.
* @author   bwang, Wuhan University
* @version	1.0.0
* @date		2019-04-09
*
*/
#ifndef GUPDLSQ_H
#define GUPDLSQ_H

#include <algorithm>
#include "gutils/ExportUPD.h"
#include "gutils/gmutex.h"
#include "gset/gsetbase.h"
#include "gall/gallobs.h"
#include "gall/gallproc.h"
#include "gall/gallambupd.h"
#include "gall/gallambflag.h"
#include "gall/gallnav.h"
#include "gall/gallbias.h"
#include "gdata/gupd.h"
#include "gdata/gifcb.h"
#include "gdata/gobsgnss.h"
#include "gambfix/gambcommon.h"
#include "gset/gcfg_upd.h"
#include "gcoders/upd.h"
#include "gio/gfile.h"
#include "gutils/ginfolog.h"
#include "gproc/gcorbds.h"

namespace great
{
	/**
	* @brief class for upd calculate main function.
	*/
	class UPD_LIBRARY_EXPORT t_gupdlsq 
	{
	public:

		/**
		* @brief default constructor, init some internal variables.
		* @param[in]  mark	upd mode.
		* @param[in]  gset	upd setting of xml.
		*/
		t_gupdlsq (UPDMODE updmode,t_gsetbase* set, t_gallproc* data, t_glog* log);

		/** @brief default destructor. */
		virtual ~t_gupdlsq();

		/** @brief output UPD/IFCB File. */
		bool GenerateProduct();

		/**
		* @brief batch processing loop over epoch.
		* @param[in] beg   begin epoch.
		* @param[in] end   end epoch.
		* @param[in] intv  sampling interval.
		* @return	 true - successfully; false - failure
		*/
		bool processBatch(const t_gtime& beg, const t_gtime& end,const double &intv);

		/**
		* @brief UPD mode to string .
		* @param[in] type   upd mode(UPDMODE).
		* @return	 WL , EWL , NL change to string successfully
		*            UNDEF - failure
		*/
		string UPDType2String(UPDMODE type);


	protected:

#ifdef BMUTEX
		boost::mutex  _mutex;
#endif
		t_gmutex            _gmutex;

		/** @brief UPD mode:EWL/WL/NL. */
		UPDMODE             _updmode;   

		/** @brief Genereal log output. */
		t_glog*				_glog;         

		 /** @brief Base setting. */
		t_gsetbase*			_set; 

		/** @brief Data used to calulate. */
		t_gallproc*         _data = nullptr;           ///< all data
		t_gallobs*          _allobs = nullptr;         ///< obs data
		t_gallambflag*      _allambflag = nullptr;     ///< ambflag data
		t_gallambupd*       _allambupd = nullptr;      ///< ambupd data
		t_gallbias*			_allbias = nullptr;        ///< dcb data
		t_gupd*             _allWLUPD = nullptr;       ///< WL upd, used for nl estimate
		t_gallnav*          _allnav = nullptr;         ///< used in glonass process
		t_gifcb*            _gifcb = nullptr;          ///< used for GPS EWL
		/** @brief Result. */
		t_gupd*             _cal_UPD = nullptr;           ///< upd computed after updlsq
		

		/** @brief Output for amb res files. */
		t_epoch_amb_res _AMB_RES;          ///< ambiguity residual data
        t_all_epoch_amb_res _AMB_RES_EWL;          ///< ambiguity residual data for EWL
		t_giof*	        _fambres;          ///< output file of ambres
        t_giof*         _fambres_EWL;          ///< output file of EWL ambres

		/** @brief Combined wavelength. */	
		map<string, int> _glo_freq_num;
		map<string, map<string, double>> _sys_wavelen;  

		/** @brief Sites & Sats & ParList */
		set<string>      _sites;           ///< sites list
		set<string>      _sats;            ///< satellites list
		string           _sys;             ///< system list(string)
		vector<string>   _parlist;         ///< params list
		vector<GOBSBAND> _proc_band;       ///< processing band for WL/NL, such as B1I/B3I for BDS3

		/** @brief reference site . */
		string _iref_site;     ///< site reference
		string _jref_site;     ///< site reference
		
		/** @brief ambiguity of satellite-station pair == xx */
		map<string, map<string, double>>  _sat_site_amb;

        /** @brief use for judge whether ambupd file is end. */        
		map<string, t_gtime> _site_ambupd_end;

		t_gcorbds _corbds;

	protected:

		/**
		* @brief batch processing loop over epoch for WL/EWL UPD
		* @param[in] beg   begin epoch.
		* @param[in] end   end epoch.
		* @param[in] intv  sampling interval.
		* @return	 true - successfully; false - failure
		*/
		bool _processWL(const t_gtime& beg, const t_gtime& end, const double &intv);

		/**
		* @brief batch processing loop over epoch for NL UPD
		* @param[in] beg   begin epoch.
		* @param[in] end   end epoch.
		* @param[in] intv  sampling interval.
		* @return	 true - successfully; false - failure
		*/
		bool _processNL(const t_gtime& beg, const t_gtime& end, const double &intv);

		/**
		* @brief combine wavelengths of WL/EWL/NL .
		* @param[in] 
		* @return    true combine wavelength successfully, false failure
		*/
		bool _setWavelength();

		/**
		* @brief Init data.
		* @param[in]
		* @return    return false if no data
		*/
		bool _initUPDProcData(t_gallproc * data);

		/**
		* @brief set sites and satellites and init params according upd mode.
		* @return	 true - successful, false failure
		*/
		bool _initSatSite();

		/**
		* @brief set output file
		* @return	 true - successful, false failure
		*/
		void _setOut();
        void _setOut_EWL();

		/**
		* @brief combine observations of the EWL/WL.
		* @param[in] site          site name.
		* @param[in] T_beg         start epoch.
		* @param[in] T_end         end epoch.
		* @param[in] intv          sampling rate in seconds.
		* @param[in] one_site_amb  widelane combination observations.
		* @param[in] OW            oneway ambiguities.
		* @param[out] one_site_amb
		* @param[out] OW
		* @return    true form (ex-)widelane observations successfully, false failure
		*/
		bool _combineObsIF(string site, const t_gtime& T_beg, const t_gtime& T_end, double intv,
			t_epo_sat_amb& one_site_amb, t_all_sats_OW& OW);

		/**
		* @brief calculate wide-lane/extra-wide-lane ambiguities for UPD estimation.
		* @param[in] intv          sampling rate in seconds.
		* @param[in] one_site_amb  widelane combination observations.
		* @param[in] OW            oneway ambiguities.
		* @param[out] OW
		* @return    true calculate successfully, false failure
		*/
		bool _calculateAmbWL(t_epo_sat_amb& one_site_amb, t_all_sats_OW& OW);

		/**
		* @brief apply satellite wide-lane UPDs to the computed wide-lane integer ambiguities,
		*	    then convert LC ambiguities to narrow-lane ambiguties with the integer wide-lane ambiguities.
		* @param[in] site   site name.
		* @param[in] t      current epoch.
		* @param[in] AMB    ambupd value from gambupd data.
		* @return    true calculate successfully, false failure
		*/
		bool _calculateAmbNL(string site, const t_gtime& t, t_epoch_amb& AMB);

		/**
		* @brief connect WL/EWL ambiguities, make each satellite-station pair have only one ambiguity.
		* @param[in] site   site name.
		* @param[in] OW     oneway ambiguities..
		* @param[in] AMB    ambupd value from gambupd data.
		* @return    true calculate successfully, false failure
		*/
		bool _connectAmb(string site, t_all_sats_OW& OW);

		/**
		* @brief Select the station with the most observation satellites as the reference station.
		* @param[in] t   current epoch.
		* @return    true calculate successfully, false failure
		*/
		bool _setRefStation(const t_gtime& t);   

		/**
		* @brief Get initial values of UPDs from UD-ambiguities based on a given reference station.
		* @param[in] epoch   current epoch.
		* @return    true calculate successfully, false failure
		*/
		bool _initUPD(const t_gtime& epoch);   

		/**
		* @brief estimate upd of each satellite and station.
		* @param[in] t   current epoch.
		* @return    true calculate successfully, false failure
		*/
		bool _updEstimate(const t_gtime& t);

		/**
		* @brief Output AMB_RES files in NL UPD Estimation.
		* @param[in] t   current epoch.
		* @return    true write successfully, false failure
		*/
		bool _writeAmbResidual(const t_gtime& t);
        bool _writeAmbResidual_EWL();
		/**
		* @brief compute DCB correction for specified obs
		* @param[in] obsdata observ data info
		* @param[in] P obs value
		* @param[in] obscombin combine type of observ
		* @param[in] gobs1 get the first observ type of obs
		* @param[in] gobs2 get the second observ type
		* @return true for sucess false for fail
		*/
		bool _apply_DCB(t_gobsgnss& satdata, double& P, OBSCOMBIN obscombin, t_gobs* gobs1, t_gobs* gobs2);

	private:

	};


}
#endif //
