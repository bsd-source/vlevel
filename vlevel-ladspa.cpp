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

// vlevel-ladspa.cpp - the ladspa plugin

#include <iostream>

#include "ladspa.h"
#include "vlevel.h"
#include "vlevel-ladspa.h"
#include "volumeleveler.h"

using namespace std;

// Is there a reason this must be allocated?  It seems to work without it.

// Why not just a LADSPA_Port struct with names and hints?

LADSPA_PortDescriptor vlevel_port_descriptors[] = {
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO,
  LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO  
};

char *vlevel_port_names[] = {
  "Look-ahead (seconds)",
  "Strength",
  "Use Maximum Multiplier",
  "Maximum Multiplier",
  "Undo",
  "Input",
  "Output"
};

// Why can't I just specify the default, instead of _LOW _HIGH masks?

LADSPA_PortRangeHint vlevel_port_range_hints[] = {
  {
    LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE |
    LADSPA_HINT_DEFAULT_MIDDLE,
    0, 5
  },
  {
    LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE |
    LADSPA_HINT_DEFAULT_HIGH,
    0, 1
  },
  {
    LADSPA_HINT_TOGGLED |
    LADSPA_HINT_DEFAULT_1,
    0, 0
  },
  {
    LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE |
    LADSPA_HINT_DEFAULT_MIDDLE,
    0, 50    
  },
  {
    LADSPA_HINT_TOGGLED |
    LADSPA_HINT_DEFAULT_0,
    0, 0
  },
  { 0, 0, 0 },
  { 0, 0, 0 }  
};

LADSPA_Descriptor vlevel_descriptor = {
  // UniqueID
  1981,
  // Label
  "vlevel",
  // Properties
  0,
  // Name
  "VLevel",
  // Maker
  "Tom Felker",
  // Copyright
  "GPL",
  // PortCount
  7,
  // PortDescriptors
  vlevel_port_descriptors,
  // PortNames
  vlevel_port_names,
  // PortRangeHints
  vlevel_port_range_hints,
  // ImplementationData
  0,
  // instantiate
  Instantiate,
  // connect_port
  ConnectPort,
  // activate
  Activate,
  // run
  Run,
  // run_adding
  0,
  // set_run_adding_gain
  0,
  // deactivate
  Deactivate,
  // cleanup
  Cleanup
};

const LADSPA_Descriptor *ladspa_descriptor(unsigned long index)
{
  if(index != 0) return 0;
  return &vlevel_descriptor;
}

LADSPA_Handle Instantiate(const LADSPA_Descriptor *descriptor, unsigned long sample_rate)
{
  VLevelInstance *pvli = new VLevelInstance;
  pvli->sample_rate = sample_rate;
  return (LADSPA_Handle)pvli;
}

void ConnectPort(LADSPA_Handle instance, unsigned long port, value_t *data_location)
{
  VLevelInstance *pvli = (VLevelInstance *)instance;
  pvli->ports[port] = data_location;
}

void Activate(LADSPA_Handle instance)
{
  VLevelInstance *pvli = (VLevelInstance *)instance;
  pvli->vl.Flush(); 
}

void Run(LADSPA_Handle instance, unsigned long sample_count)
{
  VLevelInstance *pvli = (VLevelInstance *)instance;
  
  unsigned int samples = (unsigned int)(*pvli->ports[PORT_CONTROL_LOOK_AHEAD] * pvli->sample_rate);
  if(samples != pvli->vl.GetSamples()) {
    if(samples > 60 * pvli->sample_rate) samples = 60 * pvli->sample_rate;
    if(samples < 2) samples = 2;
    pvli->vl.SetSamplesAndChannels(samples, 1);
  }

  pvli->vl.SetStrength(*pvli->ports[PORT_CONTROL_STRENGTH]);

  if(*pvli->ports[PORT_CONTROL_USE_MAX_MULTIPLIER] > 0) {
    pvli->vl.SetMaxMultiplier(*pvli->ports[PORT_CONTROL_MAX_MULTIPLIER]);
  } else {
    pvli->vl.SetMaxMultiplier(-1);
  }
  
  if(*pvli->ports[PORT_CONTROL_UNDO] > 0) {
    value_t strength = pvli->vl.GetStrength();
    pvli->vl.SetStrength(strength / (strength - 1));
  }  

  pvli->vl.Exchange(pvli->ports[PORT_INPUT1], pvli->ports[PORT_OUTPUT1], sample_count);
  
}

void Deactivate(LADSPA_Handle instance){}

void Cleanup(LADSPA_Handle instance)
{
  VLevelInstance *pvli = (VLevelInstance *)instance;
  delete pvli;
}
