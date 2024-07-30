#ifndef GGREATUPD_H
#define GGREATUPD_H

#include <chrono>
#include <signal.h>
#include "gset/gcfg_upd.h"
#include "gproc/gupdlsq.h"
#include "gcoders/ambupd.h"
#include "gcoders/biabernese.h"
#include "gcoders/ambflag.h"
#include "gcoders/rinexo.h"
#include "gcoders/upd.h"
#include "gcoders/rinexn.h"
#include "gio/gfile.h"
#include "gdata/gifcb.h"
#include "gcoders/ifcb.h"

using namespace std;
using namespace gnut;
using namespace great;
using namespace std::chrono;

void catch_signal(int) { cout << "Program interrupted by Ctrl-C [SIGINT,2]\n"; }

#endif