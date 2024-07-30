
#ifndef GALLBIAS_H
#define GALLBIAS_H

/* ----------------------------------------------------------------------
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: container for all biases
  Version: $ Rev: $

  2012-11-05 /JD: created

-*/

#include <vector>
#include "gdata/gdata.h"
#include "gmodels/gbias.h"
#include "gutils/gtime.h"

using namespace std;

namespace gnut
{

	typedef shared_ptr<t_gbias> t_spt_bias;

	// ----------
	class GNUT_LIBRARY_EXPORT t_gallbias : public t_gdata
	{

		typedef map<GOBS, t_spt_bias>   t_map_gobs;
		typedef map<string, t_map_gobs> t_map_sat;
		typedef map<t_gtime, t_map_sat> t_map_epo;
		typedef map<string, t_map_epo>  t_map_ac;

	public:
		/** @brief constructor */
		t_gallbias();
		/** @brief destructor */
		virtual ~t_gallbias();
		/** @brief add satellite bias */
		void   add(const string& ac, const t_gtime& epo, const string& obj, t_spt_bias pt_bias); 
		/** @brief get single code bias */
		double get(const t_gtime& epo, const string& obj, const GOBS& gobs1, const GOBS& gobs2);  
		

	protected:
		/** @brief get single code bias */
		t_spt_bias _find(const string& ac, const t_gtime& epo, const string& obj, const GOBS& gobs);               
		/** @brief get all biases with particular reference singal */
		vector<t_spt_bias> _find_ref(const string& ac, const t_gtime& epo, const string& obj, const GOBS& ref);    
		/** @brief Connect DCB pt_cb2 with first GOBS */
		void       _connect_first(const t_spt_bias pt_cb1, t_spt_bias pt_cb2);
		/** @brief Connect DCB pt_cb2 with second GOBS */
		void       _connect_second(const t_spt_bias pt_cb1, t_spt_bias pt_cb2);
		/** @brief Consolidate all biases with reference signal of pt_cb2 */
		void       _consolidate(const string& ac, const string& obj, const t_spt_bias pt_cb1, t_spt_bias pt_cb2);

		t_map_ac   _mapbias;           ///< map of all satellite biases (all ACs & all period & all objects)   
		bool       _overwrite;         ///< overwrite or not

	};

} // namespace

#endif
