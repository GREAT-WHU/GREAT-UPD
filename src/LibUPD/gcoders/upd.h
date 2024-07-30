/**
*
* @verbatim
History
-1.0 bwang  2019-02-22  creat the file.
-1.1 bwang  2019-04-09  Adding Doxygen Style Code Remarks
@endverbatim
* Copyright (c) 2018, Wuhan University. All rights reserved.
*
* @file		upd.h
* @brief	decode and enconde upd file
* @author   bwang, Wuhan University
* @version	1.0.0
* @date		2019-04-09
*
* Following is an example for the body of NL upd file.
*
* @verbatim
		====================================================================
	   ! mark         mjd      second
		====================================================================
		 EPOCH-TIME   58689    1770.0
		====================================================================
	   ! prn            value      sigma   point number
		====================================================================
		 G01             0.672     0.015   53
		 G02             0.214     0.017   39
@endverbatim
*
*/
#ifndef UPD_H
#define UPD_H

#include <string> 
#include <vector> 

#include "gcoders/gcoder.h"
#include "gdata/gupd.h"

using namespace std;
using namespace gnut;

namespace great {

	/**
	*@brief	   Class for decode/encode upd file
	*/
	class UPD_LIBRARY_EXPORT t_upd : public t_gcoder {

	public:

		/**
		* @brief constructor.
		* @param[in]  s        setbase control
		* @param[in]  version  version of the gcoder
		* @param[in]  sz       size of the buffer
		*/
		t_upd(t_gsetbase* s, string version = "", int sz = DEFAULT_BUFFER_SIZE);

		/** @brief default destructor. */
		virtual ~t_upd();

		/**
		* @brief decode header of upd file
		* @param[in]  buff        buffer of the data
		* @param[in]  sz          buffer size of the data
		* @param[out]  errmsg      error message of the data decoding
		* @return 0/ -1
		*/
		virtual  int decode_head(char* buff, int sz, vector<string>& errmsg)override;

		/**
		* @brief decode data of upd file
		* @param[in]  buff        buffer of the data
		* @param[in]  sz          buffer size of the data
		* @param[out]  errmsg      error message of the data decoding
		* @return consume size of header decoding
		*/
		virtual  int decode_data(char* buff, int sz, int& cnt, vector<string>& errmsg)override;

		/**
		* @brief encode header of upd file
		* @param[in]  buff        buffer of the data
		* @param[in]  sz          buffer size of the data
		* @param[out]  errmsg      error message of the data decoding
		* @return  size of header encoding
		*/
		virtual  int encode_head(char* buff, int sz, vector<string>& errmsg)override;

		/**
		* @brief encode data of upd file
		* @param[in]  buff        buffer of the data
		* @param[in]  sz          buffer size of the data
		* @param[out]  errmsg      error message of the data decoding
		* @return  size of data body encoding
		*/
		virtual  int encode_data(char* buff, int sz, int& cnt, vector<string>& errmsg)override;

	protected:
		UPDMODE _updmode;  ///< upd mode EWL,WL and NL.
		t_gtime _epoch;   ///< current epoch
	private:

	};

} // namespace

#endif
