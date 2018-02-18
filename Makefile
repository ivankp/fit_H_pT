SHELL = bash

ROOT_CXXFLAGS := $(shell root-config --cflags | sed 's/ -std=c++[^ ]\+ / /')
ROOT_LIBS     := $(shell root-config --libs) -lMinuit

.PHONY: all clean

all: pT_yy.pdf

# https://twiki.cern.ch/twiki/bin/view/LHCPhysics/CERNYellowReportPageBR

pT_yy.pdf: fit.so draw.so Makefile $(shell which hed)
	hed H1j_NLO.root H1j_B.root H1j_B_mtop.root AA1j_NLO.root -o $@ \
	  -e 's/^(pT_yy).*/\1/' \
	     'sd:with_photon_cuts/all::' \
	     'f:.*/::' 'f/^H/{ sn/pT_yy$$/; scale 2.270E-03 }' \
	     'scale 1e3 width' \
	     'fl/([^_]+).*/\1 /' 'f/mtop/{l+//mtop/}' \
	     'nl+/^pT_yy_?//' \
	     'l/^H/[H#rightarrowAA]/' \
	     'l/(\dj)/+\1/' 'l/(\d+)_(\d+)/m_{#lower[-0.3]{AA}}#in#kern[0.6]{[}\1,\2]/' \
	     'l/AA/#gamma#gamma/' \
	  -g 'div 2 1' 'mult 2 0' 'rm 1' \
	     'margin 0.1:0.1:0.04:0.1' 'rat 0.3 width' \
	     'log y' 'leg 0.71:0.70:0.96:0.9' \
	     't//Diphoton pT @ NLO + #gamma cuts/' \
	     'x//#gamma#gamma p_{T} [GeV]/' 'y,,d#sigma/dp_{T}  [fb/GeV],' \
	     'title_offset x 1.25' 'title_offset y 0.7' \
	     'range y 3e-7:2e2' \
	     'tex 750:0.07 H#rightarrow#gamma#gamma#kern[0.4]{B}R:#kern[0.4]{2}.27#times10^{#minus3} 12 1' \
	     'load ./fit.so 150:1000' \
	     'load ./draw.so' \
	     'tex 60:1.3e-4 y=exp(A+Blogx+Clog^{2}x) 12 1.0' \
	     'pad 2' 'log y' \
	  --colors 602 64 46 8 94 -vc

%.so: %.cc
	g++ -std=c++14 -Wall -O3 -fPIC $(ROOT_CXXFLAGS) $< -shared -o $@ $(ROOT_LIBS)

clean:
	@rm -vf fit.so draw.so

