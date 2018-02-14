SHELL = bash

ROOT_CXXFLAGS := $(shell root-config --cflags | sed 's/ -std=c++[^ ]\+ / /')
ROOT_LIBS     := $(shell root-config --libs) -lMinuit

.PHONY: all clean

all: pT_yy.pdf

# https://twiki.cern.ch/twiki/bin/view/LHCPhysics/CERNYellowReportPageBR

pT_yy.pdf: %.pdf: fit.so draw.so Makefile
	hed {H,AA}1j_NLO.root -o $@ \
	  -e 's/^(pT_yy).*/\1/' \
	     'sd:with_photon_cuts/all::' \
	     'nl/^pT_yy_?//' \
	     'f:.*/::' 'f/^H/[H#rightarrowAA]/{ sn/pT_yy$$/; scale 2.270E-03 }' \
	     'scale 1e3 width' \
	     'f+l/_NLO\.root/ /' \
	     'l/AA/#gamma#gamma/' 'l/(\dj)/+\1/' 'l/_/.../' \
	     'load ./fit.so 150:1000' \
	  -g 'log y' 'leg 0.75:0.73:0.96:0.9' 'margin 0.1:0.1:0.04:0.1' \
	     't//Diphoton pT @ NLO + #gamma cuts/' \
	     'x//#gamma#gamma p_{T} [GeV]/' 'y,,d#sigma/dp_{T}  [fb/GeV],' \
	     'title_offset x 1.25' \
	     'tex 755:0.7 H#rightarrow#gamma#gamma#kern[0.4]{B}R:#kern[0.4]{2}.27#times10^{#minus3} 12 0.73' \
	     'load ./draw.so' \
	     'tex 60:2e-4 y=exp(A+Blogx+Clog^{2}x) 12 0.8' \
	  --colors 602 46 8 94

%.so: %.cc
	g++ -std=c++14 -Wall -O3 -fPIC $(ROOT_CXXFLAGS) $< -shared -o $@ $(ROOT_LIBS)

clean:
	@rm -vf fit.so draw.so

