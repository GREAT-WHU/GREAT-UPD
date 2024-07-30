
#ifndef CONST_H
#define CONST_H 
 
/* ----------------------------------------------------------------------
  (c) 2011 Geodetic Observatory Pecny, http://www.pecny.cz (gnss@pecny.cz)
      Research Institute of Geodesy, Topography and Cartography
      Ondrejov 244, 251 65, Czech Republic

  Purpose: defines constants
  Version: $ Rev: $

  2011-01-10 /JD: created

-*/

#include <map>
#include <string>
#include <vector>
#include "gutils/ExportGnut.h"

using namespace std;

namespace gnut
{
#define G_PI        3.14159265358979311599796346854419e0    ///< pi
#define D2R         (G_PI/180.0)          ///< deg to rad
#define R2D         (180.0/G_PI)          ///< rad to deg
#define SEC2RAD     (D2R/3600.0)          ///< sec to rad
#define RAD2SEC     (R2D*3600.0)          ///< rad to sec

#define CLIGHT      2.99792458e+8         ///< speed of light [m/s]
#define OMEGA       7292115.1467e-11
// GLONASS
#define GM_PZ90     398.60044e12          ///< [] PZ90 earth's graviational constant
#define Aell_PZ90   6378136.000           ///< [m] PZ90 semi-major axes
#define C20_PZ90   -1082.62575e-6         ///< [] PZ90

// BeiDou
#define GM_CGCS      3.986004418e14      ///< []  CGCS2000 earth's graviational constant
#define OMGE_DOT_BDS 7.2921150e-5        ///< BDS value of the earth's rotation rate [rad/sec]// 
#define Aell_CGCS    6378137.000         ///< [m] CGCS2000 semi-major axes

} // namespace

#endif

