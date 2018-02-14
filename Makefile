SHELL = bash

ROOT_CXXFLAGS := $(shell root-config --cflags | sed 's/ -std=c++[^ ]\+ / /')
ROOT_LIBS     := $(shell root-config --libs) -lMinuit

.PHONY: all clean

all: pT_yy.pdf

pT_yy.pdf: %.pdf: fit.so Makefile
	hed {H,AA}1j_NLO.root -o $@ \
	  -e 'sd:with_photon_cuts/all:' \
	     's/^(pT_yy).*/\1/' \
	     'f/H1j/{ sn/^pT_yy$$/; scale 2.27e-3 }' \
	     'scale 1e3 width' \
	     'fl:_.*$$::' 'nl+/pT_yy_?/ /' \
	     'load ./fit.so 150:1000' 'mlog x' \
	  -g 'log y' 'leg' \
	  --colors 602 46 8 94

fit.so: %.so: %.cc
	g++ -std=c++14 -Wall -O3 -fPIC $(ROOT_CXXFLAGS) $< -shared -o $@ $(ROOT_LIBS)

clean:
	@rm -vf fit.so

