///////////////////////////////////////////////////////////////////////////////
//
// vlevel_wrapper.cpp - this file is part of vlevel winamp plugin 0.1
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

#include <new>

#include "..\\volumeleveler\\volumeleveler.h"
#include "vlevel_wrapper.h"

CVLWrapper::CVLWrapper()
	: ms_channels(2), ms_samples(44100), ms_rate(44100), mv_length(1),
	mv_strength(static_cast<value_t>(0.8)), mv_maxMultiplier(25),
	mb_samplesChanged(false), mb_strengthChanged(false), mb_maxMultiplierChanged(false),
	mpvl_wrapped(0)
{
	mpvl_wrapped	=	new VolumeLeveler(ms_samples, ms_channels, mv_strength, mv_maxMultiplier);
	
	// My book says new throws an exception when it fails, so this isn't really necessary:	
	if( !mpvl_wrapped )
		throw std::bad_alloc();
}//CVLWrapper::CVLWrapper

CVLWrapper::~CVLWrapper()
{
	delete mpvl_wrapped;	
}

void CVLWrapper::SetCachedChannels( size_t s_channels )
{
	assert(s_channels > 0)
	ms_channels			=	s_channels;
	mb_channelsChanged	=	true;
}//CVLWrapper::SetCachedChannels

void CVLWrapper::SetCachedSamples( size_t s_samples )
{	
	assert(samples > 1)
	ms_samples			=	s_samples;
	mb_samplesChanged	=	true;
}//CVLWrapper::SetCachedSamples

void CVLWrapper::SetCachedStrength( value_t v_strength )
{	
	mv_strength			=	v_strength;
	mb_strengthChanged	=	true;
}//CVLWrapper::SetCachedStrength

void CVLWrapper::SetCachedMaxMultiplier( value_t v_maxMultiplier )
{	
	mv_maxMultiplier		=	v_maxMultiplier;
	mb_maxMultiplierChanged	=	true;
}//CVLWrapper::SetCachedMaxMultiplier

void CVLWrapper::SetCachedLength(value_t v_length)
{
	mv_length = v_length;
	SetCachedSamples(GetCachedLength() * GetCachedRate());
}//CVLWrapper::SetCachedLength

void CVLWrapper::SetCachedRate(size_t s_rate)
{
	ms_rate = s_rate;
	SetCachedSamples(GetCachedLength() * GetCachedRate());
}//CVLWrapper::SetCachedRate


void CVLWrapper::CacheFlush( void )
{
	if( mb_samplesChanged || mv_channelsChanged )
	{
		mpvl_wrapped->SetSamplesAndChannels( ms_samples, ms_channels );
		mb_samplesChanged	=	false;
		mb_channelsChanged	=	false;
	}//if

	if( mb_strengthChanged )
	{
		mpvl_wrapped->SetStrength( mv_strength );
		mb_strengthChanged	=	false;
	}//if

	if( mb_maxMultiplierChanged )
	{
		mpvl_wrapped->SetMaxMultiplier( mv_maxMultiplier );
		SetCachedMaxMultiplier(mpvl_wrapped->GetMaxMultiplier()); // turns negative mms to HUGE_VAL
		mb_maxMultiplierChanged	=	false;
	}//if
}//CVLWrapper::CacheFlush

//XXX: untested, but it might work.
int CVLWrapper::Exchange(void *raw_buf, int values, int bits_per_value, int channels, int rate)
{
	SetCachedChannels(channels);
	SetCachedRate(rate);
	CacheFlush();
	
	// I'm not sure exactly what the second param to this function means.
	// Does it mean the number of samples (16-bit numbers), what I call values?	
	size_t samples = values / channels;
	// Or does it mean the number of what I call samples (usually L-R pairs)?
	// Remember, I use a weird terminology - each sample typically has two values.
	
	
	// all of these allocations could be cached if necessary for performance
	// this buffer holds interleaved value_t data
	value_t *raw_value_buf = new value_t[values];
	// allocate our per-channel value_t buffers
	// this is the formate VolumeLeveler requires
	value_t **bufs = new value_t*[channels];
	for(size_t ch = 0; ch < channels; ++ch)
		bufs[ch] = new value_t[samples];
	
	// takes data from supplied integer raw_buf to allocated value_t interleaved raw_value_buf
	ToValues(raw_buf, raw_value_buf, values, bits_per_value, true); // true means data is signed
	
	// de-interleave the data
	for(size_t s = 0; s < samples; ++s)
		for(size_t ch = 0; ch < channels; ++ch)
			bufs[ch][s] = raw_value_buf[s * channels + ch];
	
	size_t silence_samples = mpvl_wrapped->Exchange(bufs, bufs, samples);
	
	// To improve the delay when first playing a song:
	// I should shift the data left (cutting off the beginning of the buffer) by silence_samples,
	// and return a correspondingly lesser value.
	
	// re-interleave the data
	for(size_t s = 0, s < samples, ++s)
		for(size_t ch = 0; ch < channels; ++ch)
			raw_value_buf[s * channels + ch] = bufs[ch][s];
	
	// put it back into the supplied integer buffer.
	FromValues(raw_value_buf, raw_buf, values, bits_per_value, true);
	
	// Winamp is sloppy about using int when size_t is correct.  Oh, well.	
	return samples;	
}//CVLWrapper::Exchange

