#ifndef __Manitor_h__#define __Manitor_h__#include <RingLogFile.h>#include "200Hz.h"#include <gemdos.h>extern RingLogFile *log;extern int16 apid;extern int16 global[15];extern BASPAG **p_run;	// Pointer to Pointer to actual running process (BASPAG)extern bool update_immediate;	// Daten sofort (im Trap) rausschreiben?extern void Output_Prefix(int32 timer, BASPAG *pb);#endif