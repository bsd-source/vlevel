// Tom Felker
// main.cpp - the vlevel command, uses VolumeLeveler

#include <iostream>
#include <limits.h>

#include "volumeleveler.h"

using namespace std;

// len is num of values, in needs len*bits/8 chars
void ToDouble(char *in, double *out, size_t values, unsigned int bits_per_value)
{
  switch(bits_per_value) {
  case 16:
    for(size_t i = 0; i < values; ++i) out[i] = (double)((short *)in)[i] / (SHRT_MAX + 1);
    break;
  default:
    assert(false);
  }
}

// len is num of samples, out needs len*bits/8 chars
void FromDouble(double *in, char *out, size_t values, unsigned int bits_per_value)
{
  switch(bits_per_value) {
  case 16:
    for(size_t i = 0; i < values; ++i) ((short *)out)[i] = (short)(in[i] * SHRT_MAX);
    break;
  default:
    assert(false);
  }
}

void LevelRaw(istream &in, ostream& out, VolumeLeveler &vl, unsigned int bits_per_value)
{
  assert(bits_per_value % 8 == 0);
  
  size_t bytes_per_value = bits_per_value / 8;
  size_t samples = vl.GetSamples();
  size_t channels = vl.GetChannels();
  size_t values = samples * channels;
  size_t bytes = values * bytes_per_value;

  char *raw_buf = new char[bytes];
  double *double_buf = new double[values];

  while(in) {
    in.read(raw_buf, bytes);
    size_t good_values = in.gcount() / bytes_per_value;
    size_t good_samples = good_values / channels;
    ToDouble(raw_buf, double_buf, good_values, bits_per_value);
    size_t silence_samples = vl.Exchange(double_buf, good_samples);
    size_t silence_values = silence_samples * channels;    
    good_values -= silence_values;
    FromDouble(&double_buf[silence_values], raw_buf, good_values, bits_per_value);
    out.write(raw_buf, good_values * bytes_per_value);
  }
  for(size_t i = 0; i < values; ++i) double_buf[i] = 0;
  size_t silence_samples = vl.Exchange(double_buf, samples);
  size_t silence_values = silence_samples * channels;    
  FromDouble(&double_buf[silence_values], raw_buf, values - silence_values, bits_per_value);
  out.write(raw_buf, (values - silence_values) * bytes_per_value);
  
  delete [] raw_buf;
  delete [] double_buf;
  
}



int main()
{
  VolumeLeveler l;
  l.SetStrength(.9);
  LevelRaw(cin, cout, l, 16);
  return 0;
}
