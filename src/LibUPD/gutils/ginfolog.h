#ifndef GINFOLOG_H
#define GINFOLOG_H

#include "gio/glog.h"
#include "gutils/ExportUPD.h"

using namespace gnut;

namespace great
{

	UPD_LIBRARY_EXPORT void  write_log_info(t_glog* log, int l, string description);
	UPD_LIBRARY_EXPORT void  write_log_info(t_glog* log, int l, string ID, string description);



}
#endif // !GINFOLOG_H
