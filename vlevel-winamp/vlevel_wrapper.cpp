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

#include ".\\vlevel\\vlevel.h"
#include ".\\vlevel\\volumeleveler.h"

#include "vlevel_wrapper.h"

CVLWrapper::CVLWrapper( size_t s_channels )
	: ms_channels(s_channels), ms_samples(44100), mv_strength(static_cast<float>(0.8)), mv_maxMultiplier(25),
	mb_samplesChanged(false), mb_strengthChanged(false), mb_maxMultiplierChanged(false),
	mpvl_wrapped(0)
{
	mpvl_wrapped	=	new VolumeLeveler(ms_samples, ms_channels, mv_strength, mv_maxMultiplier);
	if( !mpvl_wrapped )
		throw std::bad_alloc();
}//CVLWrapper::CVLWrapper

void CVLWrapper::SetCachedSamples( size_t s_samples )
{	
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

void CVLWrapper::CacheFlush( void )
{
	if( mb_samplesChanged )
	{
		mpvl_wrapped->SetSamplesAndChannels( ms_samples, ms_channels );
		mb_samplesChanged	=	false;
	}//if

	if( mb_strengthChanged )
	{
		mpvl_wrapped->SetStrength( mv_strength );
		mb_strengthChanged	=	false;
	}//if

	if( mb_maxMultiplierChanged )
	{
		mpvl_wrapped->SetMaxMultiplier( mv_maxMultiplier );
		mb_maxMultiplierChanged	=	false;
	}//if
}//CVLWrapper::CacheFlush

