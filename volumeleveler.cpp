// Tom Felker
// volumeleveler.cpp - defines the VolumeLeveler class

#include <sys/types.h>
#include <assert.h>
#include <math.h>

#include "volumeleveler.h"

VolumeLeveler::VolumeLeveler(size_t l, size_t c,  double s, double m)
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

void VolumeLeveler::SetStrength(double s)
{
  assert(s >= 0 && s <= 1);
  strength = s;
}

double VolumeLeveler::GetStrength()
{
  return strength;
}

void VolumeLeveler::SetMaxMultiplier(double m)
{
  if(m <= 0) m = HUGE_VAL;
  max_multiplier = m;
}

double VolumeLeveler::GetMaxMultiplier()
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
  buf = new double[s * c];
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

size_t VolumeLeveler::Exchange(double *user_buf, size_t user_samples)
{
  // for each user_pos in user_buf
  for(size_t user_pos = 0; user_pos < user_samples; ++user_pos) {
    
    // compute multiplier
    double multiplier = pow(avg_amp, -strength);
    if(multiplier > max_multiplier) multiplier = max_multiplier;

    // swap buf[pos] with user_buf[user_pos], scaling user[buf] by
    // multiplier and finding max of the new sample

    double new_val = 0;
    for(size_t ch = 0; ch < channels; ++ch) {
      double in = user_buf[user_pos * channels + ch];
      double out = buf[pos * channels + ch];
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
	double sample_val = 0; // buf[pos+i].GetMax()
	for(size_t ch = 0; ch < channels; ++ch) {
	  double ch_val = fabs(buf[((pos + i) % samples) * channels + ch]);
	  if(ch_val > sample_val) sample_val = ch_val;
	}
	double slope = (sample_val - avg_amp) / i;
	if(slope >= max_slope) { // must be >=, otherwise clipping causes excessive computation
	  max_slope_pos = (pos + i) % samples;
	  max_slope = slope;
	  max_slope_val = sample_val;
	}
      }
    } else {
      // only chance of higher slope is the new sample
     
      // recomputing max_slope isn't really necessary...
      max_slope = (max_slope_val - avg_amp) / ((max_slope_pos - pos + samples) % samples);
      // TODO: assert that above was NOP and remove if so

      double slope = (new_val - avg_amp) / (samples - 1);
      
      if(slope >= max_slope) { // probably needs to be >= for same reason as above
	max_slope_pos = pos;
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

