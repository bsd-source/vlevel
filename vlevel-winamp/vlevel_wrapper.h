///////////////////////////////////////////////////////////////////////////////
//
// vlevel_wrapper.h - this file is part of vlevel winamp plugin 0.1
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

#ifndef INCLUDED_VLEVEL_WRAPPER_H
#define INCLUDED_VLEVEL_WRAPPER_H

#include "vlevel\\vlevel.h"

//This header is the interface of the module. As people using it have no
//need for knowledge of VolumeLeveler's internals, we just tell the compiler
//he will find everything later. The header above is included, because if
//someone changes value_t's definition, it should be valid here, too
class VolumeLeveler;


//This class is a wrapper for VolumeLeveler.
//It caches some settings and de-/interlaces
//Exchange()'s in and output buffers
class CVLWrapper
{
private:
	size_t			ms_channels;

//cached values
	size_t			ms_samples;
	value_t			mv_strength;
	value_t			mv_maxMultiplier;

//change indicators - used for 2 reasons:
// - comparing float values is not accurate always
// - it is faster than comparing
	bool			mb_samplesChanged;
	bool			mb_strengthChanged;
	bool			mb_maxMultiplierChanged;

//main object
	VolumeLeveler*	mpvl_wrapped;
public:
	CVLWrapper( size_t s_channels=2 );

	~CVLWrapper( void )	{	};

//these automatically mark the values as 'changed'
	inline void SetCachedSamples( size_t s_samples );
	inline void SetCachedStrength( value_t v_strength );
	inline void SetCachedMaxMultiplier( value_t v_maxMultiplier );

//write cached values to the 'real' object
	inline void CacheFlush( void );
};//CVLWrapper

#endif//#ifndef VLEVEL_WRAPPER_H