/**
*
* @verbatim
History
-1.0 glfeng   2019-02-28 creat the file.
-1.1 glfeng 2019-04-10 Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		   gcfg_upd.h
* @brief	   control set from XML
* @author      glfeng, Wuhan University
* @version	   1.0.0
* @date		   2019-04-10
*
*/

#ifndef GCFG_UPD_H
#define GCFG_UPD_H

#include "gutils/ExportUPD.h"
#include "gset/gsetgen.h"
#include "gset/gsetgnss.h"
#include "gset/gsetinp.h"
#include "gset/gsetout.h"
#include "gset/gsetproc.h"

namespace great
{
	/**
	*@brief	   Class for UPD contrl setting
	*/
	class UPD_LIBRARY_EXPORT t_gcfg_upd :
		public t_gsetgen,
		public t_gsetinp,
		public t_gsetout,
		public t_gsetgnss,
		public t_gsetproc
	{
	public:

		/** @brief default constructor. */
		t_gcfg_upd();

		/** @brief default destructor. */
		virtual ~t_gcfg_upd();

		/** 
		* @brief settings check. 
		* @return void
		*/
		void check();                                 // settings check

		/** 
		* @brief settings help. 
		* @return void
		*/
		void help();                                  // settings help

	protected:

	private:
	};

}

#endif 