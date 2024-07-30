/**
*
* @verbatim
	The format of this block:
	    <process updmode="NL" />                  <!-- EWL WL NL EWL_epoch NONE -->
	@endverbatim

* @verbatim
	History
		-1.0 glfeng   2019-02-27 creat the file.
		-1.1 bwang    2019-04-10 Adding Doxygen Style Code Remarks
	@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		   gsetproc.h
* @brief	   control set from XML
* @author      glfeng, Wuhan University
* @version	   1.0.0
* @date		   2019-04-10
*
*/

#ifndef GSETPROC_H
#define GSETPROC_H

#define XMLKEY_PROC "process"

#include "gset/gsetbase.h"
#include "gutils/ExportUPD.h"

using namespace std;
using namespace gnut;
namespace great
{
	/** @brief class enum of UPD mode. */
	enum UPDMODE { NONE, WL, EWL, EWL_epoch, NL };

	/** @brief class enum of observation combination. */
	enum OBSCOMBIN { MW_COMBIN, EWL_COMBIN };

	/**
	*@brief	   Class for set ambiguity fixed xml
	*/
	class UPD_LIBRARY_EXPORT t_gsetproc : public virtual t_gsetbase
	{
	public:

		/** @brief default constructor. */
		t_gsetproc();

		/** @brief default destructor. */
		virtual ~t_gsetproc();

		/**
		* @brief settings check.
		* @return void
		*/
		void check();

		/**
		* @brief settings help.
		* @return void
		*/
		void help();

		/**
		* @brief  get upd type.
		* @return upd type
		*/
		UPDMODE get_updtype();

		// correct bds code bias
		bool bds_code_bias_correction();
	

	
	protected:

	private:

	};
	
	/**
	* @brief change string to UPDMODE.
	* @param[in]  str   upd mode string
	* @return	  UPDMODE
	*/
	UPD_LIBRARY_EXPORT UPDMODE str2updmode(string str);

	/**
	* @brief change UPDMODE to string.
	* @param[in]  UPDMODE
	* @return	  str
	*/
	UPD_LIBRARY_EXPORT string updmode2str(UPDMODE mode);
}
#endif
