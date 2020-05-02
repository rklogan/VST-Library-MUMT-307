#include "ModalURB.h"

ModalURB::ModalURB(double fs){
	Fs = fs;
	bws = {3, 1, 2, 2, 2, 2, 4, 3};
	amps = {0.0885, 0.3393, 0.5523, 0.4367, 0.9, 0.121, 0.2951, 0.0369};
	for(int i = 0; i < 8; i++){
		filters[i] = new Biquad(0, 1);
		filters[i].setFs(Fs);
	}
}

void ModalURB::updateParams(double f){
	updateFreqs(f);
	for(int i = 0; i < 8; i++){
		filters[i].setFc(f[i], true);
	}
}


void ModalURB::updateFreqs(double f){
	f0 = f;
	freqs[0] = 0.4863636364 * f0;
	freqs[1] = 0.9318181818 * f0;
	freqs[2] = f0;
	freqs[3] = 1.0136363636 * f0;
	freqs[4] = 2.0045454545 * f0;
	freqs[5] = 3.0136363636 * f0;
	freqs[6] = 6.0090909091 * f0;
	freqs[7] = 5.0181818182 * f0;
}

void ModalURB::generateNote(double f, double *output, int nFrames){
	updateParams(f);

	//create an impulse
	double *impulse = (double *) malloc(sizeof(double) * nFrames);
	impulse[0] = 1;
	for(int i = 1; i < nFrames; i++){
		impulse[i] = 0;
	}
	
	//clear output buffer
	for(int j = 0; j < nFrames; j++){
		output[j] = 0;
	}
	
	//apply imulse to each filter
	for(int i = 0; i < 8; i++){
		double *out = (double *) malloc(sizeof(double)*nFrames);
		filters[i].processBuffer(impulse, out, nFrames);
		
		//copy the scaled filter output to the output buffer
		for(int j = 0; j < nFrames; j++){
			output[j] += amps[i] * out[j];
		}
	}

	//apply a linear envelope to the output
	for(int i = 0; i < nFrames; i++){
		output[i] *= ( nFrames - (i+1) ) / nFrames;
	}
}