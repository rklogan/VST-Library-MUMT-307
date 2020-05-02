#ifndef MODALURB_H
#define MODALURB_H

//include
#include "Biquad.h"
#include "ModalURB.h"

class ModalURB{
protected:
	double Fs;
	double f0;
	Biquad filters[8];
	double freqs[8];
	double amps[8];
	double bws[8]

public:
	ModalURB(double fs);
	double getFs(){return Fs;}
	double getF0(){return f0;}
	void updateParams(double f);
	void updateFreqs(double f);
	void generateNote(double f, double *output, int nFrames)
};

#endif