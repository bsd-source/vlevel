// Copyright 2003 Tom Felker
//
// This file is part of VLevel.
//
// VLevel is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// VLevel is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with VLevel; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

// volumeleveler.cpp - defines the VolumeLeveler class

#include <sys/types.h>
#include <assert.h>
#include <math.h>

#include "vlevel.h"
#include "volumeleveler.h"

VolumeLeveler::VolumeLeveler(size_t l, size_t c,  value_t s, value_t m)
{
  buf = 0;
  SetSamplesAndChannels(l, c);
  SetStrength(s);
  SetMaxMultiplier(m);
}

VolumeLeveler::~VolumeLeveler()
{
  delete [] buf;
}

void VolumeLeveler::SetStrength(value_t s)
{
  assert(s >= 0 && s <= 1);
  strength = s;
}

value_t VolumeLeveler::GetStrength()
{
  return strength;
}

void VolumeLeveler::SetMaxMultiplier(value_t m)
{
  if(m <= 0) m = HUGE_VAL;
  max_multiplier = m;
}

value_t VolumeLeveler::GetMaxMultiplier()
{
  return max_multiplier;
}

size_t VolumeLeveler::GetSamples()
{
  return samples;
}

size_t VolumeLeveler::GetChannels()
{
  return channels;
}

void VolumeLeveler::SetSamplesAndChannels(size_t s, size_t c)
{
  assert(s > 0 && c > 0);
  delete [] buf;
  buf = new value_t[s * c];
  samples = s;
  channels = c;
  Flush();
}

void VolumeLeveler::Flush()
{
  for(size_t i = 0; i < samples * channels; ++i) buf[i] = 0;
  silence = samples;
  pos = max_slope_pos = 0;
  max_slope = max_slope_val = avg_amp = 0;
}

size_t VolumeLeveler::GetSilence()
{
  return silence;
}

size_t VolumeLeveler::Exchange(value_t *user_buf, size_t user_samples)
{
  // for each user_pos in user_buf
  for(size_t user_pos = 0; user_pos < user_samples; ++user_pos) {
    
    // compute multiplier
    value_t multiplier = pow(avg_amp, -strength);
    if(multiplier > max_multiplier) multiplier = max_multiplier;

    // swap buf[pos] with user_buf[user_pos], scaling user[buf] by
    // multiplier and finding max of the new sample

    value_t new_val = 0;
    for(size_t ch = 0; ch < channels; ++ch) {
      value_t in = user_buf[user_pos * channels + ch];
      value_t out = buf[pos * channels + ch];
      user_buf[user_pos * channels + ch] = out * multiplier;
      buf[pos * channels + ch] = in;
      if(fabs(in) > new_val) new_val = fabs(in);
    }

    pos = (pos + 1) % samples; // now pos is the oldest, new one is pos-1
    
    avg_amp += max_slope;

    if(pos == max_slope_pos) {
      // recompute (this is expensive)
      max_slope = -HUGE_VAL;
      for(size_t i = 1; i < samples; ++i) {
	value_t sample_val = 0; // buf[pos+i].GetMax()
	for(size_t ch = 0; ch < channels; ++ch) {
	  value_t ch_val = fabs(buf[((pos + i) % samples) * channels + ch]);
	  if(ch_val > sample_val) sample_val = ch_val;
	}
	value_t slope = (sample_val - avg_amp) / i;
	if(slope >= max_slope) { // must be >=, otherwise clipping causes excessive computation
	  max_slope_pos = (pos + i) % samples;
	  max_slope = slope;
	  max_slope_val = sample_val;
	}
      }
    } else {
      // only chance of higher slope is the new sample
     
      // recomputing max_slope isn't really necessary...
      //max_slope = (max_slope_val - avg_amp) / ((max_slope_pos - pos + samples) % samples);
      // TODO: assert that above was NOP and remove if so

      value_t slope = (new_val - avg_amp) / (samples - 1);
      
      if(slope >= max_slope) { // probably needs to be >= for same reason as above
	max_slope_pos = (pos - 1) % samples;
	max_slope = slope;
	max_slope_val = new_val;
      }
      
    }
    
  }
  // compute silence stuff
  
  if(silence >= user_samples) {
    silence -= user_samples;
    return user_samples;
  } else {
    size_t returned_silence = silence;
    silence = 0;
    return returned_silence;
  }
}

