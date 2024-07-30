#include "ginfolog.h"
#include <iostream>
#include <iomanip>

namespace great
{
void write_log_info(t_glog * log, int l, string description)
{
	if (log)
	{
		log->comment(l, description);
	}
	else
	{
		std::cout << setw(88) << description << endl;
	}
}

void write_log_info(t_glog * log, int l, string ID, string description)
{
	if (log)
	{
		log->comment(l, ID, description);
	}
	else
	{
		std::cout << std::setw(8) << ID + " : " << std::setw(88) << description << endl;
	}
}
}

