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

// vlevel-bin.cpp - the vlevel-bin command, uses VolumeLeveler

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <limits.h>
#include <assert.h>

#include "volumeleveler.h"
#include "commandline.h"

using namespace std;

void ToDouble(char *in, double *out, size_t values, unsigned int bits_per_value);
void FromDouble(double *in, char *out, size_t values, unsigned int bits_per_value);
void LevelRaw(istream &in, ostream& out, VolumeLeveler &vl, unsigned int bits_per_value);
void Usage();

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

void Usage()
{
  cerr << "Perhaps at some point this will be documented." << endl;
}

int main(int argc, char *argv[])
{
  CommandLine cmd(argc, argv);
  size_t length = 3 * 44100;
  size_t channels = 2;
  double strength = .8, max_multiplier = 15;
  string infile, outfile, option, argument;
  
  while(option = cmd.GetOption(), !option.empty()) {
    
    if(option == "length" || option == "l") {
      if((istringstream(cmd.GetArgument()) >> length).fail()) {
        cerr << cmd.GetProgramName() << ": bad or no option for --length" << endl;
        return 2;
      }
    } else if(option == "channels" || option == "c") {
      if((istringstream(cmd.GetArgument()) >> channels).fail()) {
        cerr << cmd.GetProgramName() << ": bad or no option for --channels" << endl;
        return 2;
      }
    } else if(option == "strength" || option == "s") {
      if((istringstream(cmd.GetArgument()) >> strength).fail()) {
        cerr << cmd.GetProgramName() << ": bad or no option for --strength" << endl;
        return 2;
      }
    } else if(option == "max-multiplier" || option == "m") {
      if((istringstream(cmd.GetArgument()) >> max_multiplier).fail()) {
        cerr << cmd.GetProgramName() << ": bad or no option for --max-multiplier" << endl;
        return 2;
      }
    } else if(option == "help" || option == "h") {
      Usage();
      return 0;
    } else {
      cerr << cmd.GetProgramName() << ": unrecognized option " << option << endl;
      Usage();
      return 2;
    }
  }
  
  cerr << "Beginning VLevel with:" << endl
       << "length: " << length << endl
       << "channels: " << channels << endl
       << "strength: " << strength << endl
       << "max_multiplier: " << max_multiplier << endl;
  
  VolumeLeveler l(length, channels, strength, max_multiplier);
  LevelRaw(cin, cout, l, 16);
  return 0;
}
