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

// This is now the only header
// BTW, I laugh at your OS's backslashes! :-)
#include "..\\voluemeleveler\\volumeleveler.h"

//This class is a wrapper for VolumeLeveler.
//It caches some settings and de-/interlaces
//Exchange()'s in and output buffers

class CVLWrapper
{
private:

//cached values
	size_t			ms_channels;
	size_t			ms_samples;
	value_t			mv_strength;
	value_t			mv_maxMultiplier;

	// these don't need Changed flags, because they aren't real, but they affect ms_samples
	value_t			mv_length; //like mv_samples, but in seconds
	size_t			ms_rate; //used to convert length to channels
	
//change indicators - used for 2 reasons:
// - comparing float values is not accurate always
// - it is faster than comparing
	bool			mb_channelsChanged;
	bool			mb_samplesChanged;
	bool			mb_strengthChanged;
	bool			mb_maxMultiplierChanged;

//main object
	VolumeLeveler*	mpvl_wrapped;
public:
	CVLWrapper();
	~CVLWrapper();

//these automatically mark the values as 'changed'
	inline void SetCachedChannels( size_t channels );
	inline void SetCachedSamples( size_t s_samples );
	inline void SetCachedStrength( value_t v_strength );
	inline void SetCachedMaxMultiplier( value_t v_maxMultiplier );

	inline void SetCachedLength(value_t v_length);
	inline void SetCachedRate(size_t s_rate);
	
//these Get functions might be useful	
	inline size_t GetCachedChannels() { return ms_channels; };
	inline size_t GetCachedSamples() { return ms_samples; };
	inline value_t GetCachedStrength() { return mv_strength; };
	inline value_t GetCachedMaxMultiplier() { return mv_maxMultiplier; };
	
	inline value_t GetCachedLength() { return mv_length; };
	inline size_t GetCachedRate() { return ms_rate; };
	
//write cached values to the 'real' object
	inline void CacheFlush( void );
	
//exchange, which handles it's own de- and re-interlacing
//TODO: figure out what it's best to return
	int Exchange(void *samples, int numsamples, int bps, int nch, int rate); 
	
};//CVLWrapper

#endif//#ifndef VLEVEL_WRAPPER_H