// Tom Felker
// volumeleveler.h - declares the VolumeLeveler class

// A note on terminology: atoms are indivual double values; samples
// are one or more atoms, for example, for stereo, a sample has 2
// atoms, and channels is 2.

#ifndef VOLUMELEVELER_H
#define VOLUMELEVELER_H

#include <sys/types.h>

class VolumeLeveler {
public:

  // constructs and destructs a VolumeLeveler with a length of l
  // samples with c channels each, an effect strength of s and a
  // maximum multiplier of m
  VolumeLeveler(size_t l = 44100, size_t c = 2, double s = .8, double m = 25);
  ~VolumeLeveler();
   
  // Reallocates a buffer of l samples and c channels (contents are lost)
  void SetSamplesAndChannels(size_t l, size_t c);
  
  // get samples and number of channels
  size_t GetSamples();
  size_t GetChannels();

  // set and get the strength (between 0 and 1) (set doesn't affect the buffer)
  void SetStrength(double s);
  double GetStrength();
  
  // set and get the max multiplier (set doesn't affect the buffer)
  void SetMaxMultiplier(double m);
  double GetMaxMultiplier();
  
  // fills the buffers with silence
  void Flush();

  // how many samples are slient from Flush, not filled by input.
  size_t GetSilence();

  // replaces raw with processed, returns how many samples are
  // residual silence from when the buffers were empty.
  size_t Exchange(double *user_buf, size_t user_samples);
  
private:

  // the buffer
  double *buf;
  
  // the length of the buffer (samples)
  size_t samples;

  // the number of atoms per channel
  size_t channels;
  
  // position about to be returned (samples)
  size_t pos;
  
  // position of the max slope (samples)
  size_t max_slope_pos;
  
  // the current "blanket" amplitude
  double avg_amp;  
  
  // the maximum slope
  double max_slope;

  // the value at the maximum slope
  double max_slope_val;
  
  // the strength of the effect (between 0 and 1)
  double strength;
  
  // the maximum value by which a sample will be scaled
  double max_multiplier;
  
  // the amount of silence (data that wasn't input) left in the buffer (samples).
  size_t silence;
};

#endif // ndef VOLUMELEVELER_H
