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

// vlevel-ladspa.h - for the LADSPA plugin

#ifndef VLEVEL_LADSPA_H
#define VLEVEL_LADSPA_H

#include "vlevel.h"
#include "ladspa.h"
#include "volumeleveler.h"

typedef LADSPA_Data value_t;

#define PORT_COUNT 8

#define PORT_CONTROL_LOOK_AHEAD 0
#define PORT_CONTROL_STRENGTH 1
#define PORT_CONTROL_USE_MAX_MULTIPLIER 2
#define PORT_CONTROL_MAX_MULTIPLIER 3
#define PORT_CONTROL_UNDO 4
#define PORT_OUTPUT_MULTIPLIER 5
#define PORT_INPUT1 6
#define PORT_OUTPUT1 7

// TODO: native stereo support
//#define PORT_INPUT2 8
//#define PORT_OUTPUT2 9

struct VLevelInstance {
  VLevelInstance(): vl(2, 1) {}
  VolumeLeveler vl;
  value_t *ports[PORT_COUNT];
  unsigned long sample_rate;
};


LADSPA_Handle Instantiate(const LADSPA_Descriptor *descriptor, unsigned long sample_rate);
void ConnectPort(LADSPA_Handle instance, unsigned long port, value_t *data_location);
void Activate(LADSPA_Handle instance);
void Run(LADSPA_Handle instance, unsigned long sample_count);
void Deactivate(LADSPA_Handle instance);
void Cleanup(LADSPA_Handle instance);



#endif // ndef VLEVEL_LADSPA_H
