#include "Yin.h"

/**
 * defaults all values. Not very useful... 
 */
void Yin::init(){
	init(44100.0, 1024, 0.8);
}

/**
 * Helper function for the constructor to set all values
 */
void Yin::init(double fs, int nFrames, double t){
	Fs = fs;
	setBufSize(nFrames/2);
	prob = 0;
	thresh = t;
}

/**
 * Default constructor. Sets sample rate to 44100 Hz
 */
Yin::Yin(){
	init();
}

/**
 * Constructor allowing the user to specify all parameters
 * @param {double} fs The sample rate
 * @param {int} nFrames The number of samples in the input buffers to be analyzed.
 * @param {double} t The threshold for the Yin algorithm
 */
Yin::Yin(double fs, int nFrames, double t){
	init(fs, nFrames, t);
}

/**
 * Constructor allowing the user to specify the sample rate and
 * the threshold. This constructor is intended for use with
 * getPitch(double *input, int nFrames)
 * @param {double} fs The sample rate
 * @param {double} t The threshold for the yin algorithm
 * @see getPitch(double *input, int nFrames)
 */
Yin::Yin(double fs, double t){
	Fs = fs;
	thresh = t;
}

/**************** SETTERS ****************/
/**
 * Sets the size of the buffer and resets all values in the 
 * buffer to 0
 */
void Yin::setBufSize(int b){
	bufSize = b;
	//set up buffer
	buffer = (double *) malloc(sizeof(double) * bufSize);
	for(int i = 0; i < bufSize; i++) buffer[i] = 0;
}

/**
 * Calculates the squared difference of the signal with a shifted
 * version of itself and stores the results in buffer
 * @para, {double*} in A buffer holding the signal to be analyzed
 */
void Yin::squaredDiffs(double *in){
	int i;
	int tau;
	double delta;

	for(tau = 0; tau < bufSize; tau++){
		for(i = 0; i < bufSize; i++){
			delta = in[i] - in[i + tau];
			buffer[tau] += delta * delta;
		}
	}
}

/**
 * Calculates the cumulative mean on the normalized difference
 * of the data stored in buffer and puts the result in buffer.
 */
void Yin::CMND(){
	int tau;
	double sum = 0;
	buffer[0] = 1;

	for(tau = 1; tau < bufSize; tau++){
		sum += buffer[tau];
		buffer[tau] *= tau / sum;
	}
}

/**
 * Searches for the best correlation
 * @return The t that produces the bes autocorrelation. -1 if not found.
 */
int Yin::absThresh(){
	int tau;

	///search for values over threshold
	for(tau = 2; tau < bufSize; tau++){
		if(buffer[tau] < thresh){
			while(tau + 1 < bufSize
					&& buffer[tau + 1] < buffer[tau]){
				tau++;
			}
			//found
			prob = 1 - buffer[tau];
			break;
		}
	}

	//if no find
	if(tau == bufSize || buffer[tau] >= thresh){
		tau = -1;
		prob = 0;
	}
	return tau;
}

/**
 * Performs parabolic interpolation on the estimated t to improve the value
 * @param {int} est The current estimation of t
 * @return The improved t value
 */
double Yin::interpolate(int tauEst){
	double newTau;
	int z0, z2;

	if(tauEst < 1) z0 = tauEst;
	else z0 = tauEst - 1;

	if(tauEst + 1 < bufSize) z2 = tauEst + 1;
	else z2 = tauEst;

	//parabolic interpolation
	if(z0 == tauEst){
		if(buffer[tauEst] <= buffer[z2]){
			newTau = tauEst;
		}
		else newTau = z2;
	}
	else if(z2 == tauEst){
		if(buffer[tauEst] <= buffer[z0]){
			newTau = tauEst;
		}
		else newTau = z0;
	}
	else{
		double s0, s1, s2;
		s0 = buffer[z0];
		s1 = buffer[tauEst];
		s2 = buffer[z2];

		newTau = tauEst + (s2 - s0) / (2 * (2 * s1 - s2 - s0));
	}

	return newTau;
}

/**
 * getPitch applies the yin autocorrelation algorithm to a set of
 * samples.
 * @param {double} fs The Sample Rate in Hz
 * @param {double} t The threshold for the Yin alorithm
 * @param {double*} in The input buffer
 * @param {int} nFrames The length of the input buffer
 * @return The fundamental frequency.
 */
double Yin::getPitch(double fs, double t, double *in, int nFrames){
	int est = -1;
	double pitch = -1;

	init(fs, nFrames, t);
	squaredDiffs(in);
	CMND();
	est = absThresh();
	if(est != -1) pitch = Fs / interpolate(est);
	return pitch;
}

/**
 * Provides a user interface for a user who is applying multiple
 * autocorrelations without changing the sample rate. The sample
 * rate must be set before using this function.
 * @param {double} t The threshold for the yin algorithm
 * @param {double*} in The input buffer
 * @param nFrames The size of the input buffer
 * @return The fundamental frequency of the input buffer.
 */
double Yin::getPitch(double t, double *in, int nFrames){
	return getPitch(Fs, t, in, nFrames);
}

/**
 *  Provides a user interface for a user who is applying multiple
 * autocorrelations without changing the sample rate or threshold.
 * This function requires that values for Fs and threshold be
 * specified before use.
 * @param {double*} in The input buffer
 * @param {int} nFrames The size of the input buffer
 * @return The fundamental frequency of the input buffer
 */
double Yin::getPitch(double *in, int nFrames){
	return getPitch(Fs, thresh, in, nFrames);
}

/**
 * Provides a user interface for a user who is applying multiple
 * autocorrelations without changing the sample rate, threshold,
 * nor buffer length. This function requires that the sample rate,
 * the threshold, and the number of samples in each buffer be 
 * specified before use. Other versions of getPitch are 
 * recommended for stability.
 * @param {double*} in The input buffer
 * @return The fundamental frequency of the input buffer
 */
double Yin::getPitch(double *in){
	int est = -1;
	double pitch = -1;
	
	setBufSize(bufSize);
	squaredDiffs(in);
	CMND();
	est = absThresh();
	if(est != -1) pitch = Fs / interpolate(est);
	return pitch;
}