///////////////////////////////////////////////////////////////////////////////
//
// vlevel_winamp.cpp - this file is part of vlevel winamp plugin 0.1
// Copyright (C) 2003  Markus Sablatnig
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include "dsp.h"
#include "vlevel_wrapper.h"

// avoid stupid CRT silliness
BOOL WINAPI _DllMainCRTStartup(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}

// module getter.
winampDSPModule *getModule(int which);

void config(struct winampDSPModule *this_mod);
int init(struct winampDSPModule *this_mod);
void quit(struct winampDSPModule *this_mod);
int modify_samples(struct winampDSPModule *this_mod, short int *samples, int numsamples, int bps, int nch, int srate);

// Module header, includes version, description, and address of the module retriever function
winampDSPHeader hdr = { DSP_HDRVER, "VLevel Winamp plugin test", getModule };

// first module
winampDSPModule mod =
{
	"foobar",
	NULL,	// hwndParent
	NULL,	// hDllInstance
	config,
	init,
	modify_samples,
	quit
};

#ifdef __cplusplus
extern "C" {
#endif
// this is the only exported symbol. returns our main header.
__declspec( dllexport ) winampDSPHeader *winampDSPGetHeader2()
{
	return &hdr;
}
#ifdef __cplusplus
}
#endif


winampDSPModule *getModule(int which)
{
	if( which == 0 )
		return &mod;
	else
		return NULL;
}

void config(struct winampDSPModule *this_mod)
{
	MessageBox(this_mod->hwndParent,"Hello world!",
									"TestMsg",MB_OK);
}

int init(struct winampDSPModule *this_mod)
{
	CVLWrapper*		pvlw_userData	=	0;

	try
	{
		pvlw_userData	=	new CVLWrapper;
		if( !pvlw_userData )
			return 1;

		//read values from registry and set (or read first and construct using them)

		this_mod->userData	=	static_cast<void*>(pvlw_userData);
	}//try
	catch(...)
	{
		if( pvlw_userData )
			delete pvlw_userData;
		this_mod->userData = pvlw_userData = 0;
		
		return 1;
	}//catch
	
	return 0;
}

void quit(struct winampDSPModule *this_mod)
{
	if( this_mod->userData )
		delete this_mod->userData;
	this_mod->userData	=	0;
}//quit

int modify_samples(struct winampDSPModule *this_mod, short int *samples, int numsamples, int bps, int nch, int srate)
{
	// Note - this code is untested, but it should capture the gist of things.
	
	assert(this_mod && this_mod->userData);
	
	// Exchange automatically flushes
	return this_mod->userData->Exchange((void *)samples, numsamples, bps, nch, srate);
}