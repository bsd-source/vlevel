# This file is part of VLevel, a dynamic volume normalizer.
#
# Copyright 2003 Tom Felker <tcfelker@mtco.com>
#
# This library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of the
# License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
# USA

# User-editable options:

# Change this to suit your preferences (maybe add -march=cputype)	
CXXFLAGS=-Wall -O3 -fPIC -DPIC -g

# This is where it will be installed
PREFIX=/usr/local/bin/
LADSPA_PREFIX=/usr/local/lib/ladspa/

# End of user-editable options.

all: vlevel-bin vlevel-ladspa.so

install: all
	cp -f vlevel-bin $(PREFIX)
	mkdir -p $(LADSPA_PREFIX)
	cp -f vlevel-ladspa.so $(LADSPA_PREFIX)

clean:
	rm -f *.o vlevel-bin vlevel-ladspa.so

vlevel-ladspa.so: vlevel-ladspa.o volumeleveler.o
	$(CXX) $(CXXFLAGS) -shared -o vlevel-ladspa.so vlevel-ladspa.o volumeleveler.o

vlevel-ladspa.o: vlevel-ladspa.cpp volumeleveler.h vlevel-ladspa.h vlevel.h ladspa.h
	$(CXX) $(CXXFLAGS) -c vlevel-ladspa.cpp

vlevel-bin: volumeleveler.o commandline.o vlevel-bin.o vlevel.h
	$(CXX) $(CXXFLAGS) -o vlevel-bin vlevel-bin.o volumeleveler.o commandline.o

volumeleveler.o: volumeleveler.cpp volumeleveler.h vlevel.h
	$(CXX) $(CXXFLAGS) -c volumeleveler.cpp

vlevel-bin.o: vlevel-bin.cpp volumeleveler.h commandline.h vlevel.h
	$(CXX) $(CXXFLAGS) -c vlevel-bin.cpp

commandline.o: commandline.cpp commandline.h
	$(CXX) $(CXXFLAGS) -c commandline.cpp

