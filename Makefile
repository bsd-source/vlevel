# Copyright 2003 Tom Felker
#
# This file is part of VLevel.
#
# VLevel is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# VLevel is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with VLevel; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

# CXX=g++-2.96 #faster, for some reason.
CXXFLAGS=-Wall -O3

all: vlevel-bin

vlevel-bin: vlevel-bin.o volumeleveler.o commandline.o
	$(CXX) $(CXXFLAGS) -o vlevel-bin vlevel-bin.o volumeleveler.o commandline.o

volumeleveler.o: volumeleveler.cpp volumeleveler.h
	$(CXX) $(CXXFLAGS) -c volumeleveler.cpp

vlevel-bin.o: vlevel-bin.cpp volumeleveler.h commandline.h
	$(CXX) $(CXXFLAGS) -c vlevel-bin.cpp

commandline.o: commandline.cpp commandline.h
	$(CXX) $(CXXFLAGS) -c commandline.cpp

clean:
	rm -f *.o vlevel-bin