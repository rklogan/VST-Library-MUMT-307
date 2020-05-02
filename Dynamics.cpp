//include
#include "Dynamics.h"

/**
 * Default constructor. Defaults to peak value mode with 
 * a sample rate of 44100.0Hz
 */
Dynamics::Dynamics(){
	mode = 0;
	Fs = 44100.00;
	atkSample = -1;
	relSample = -1;
	effRatio = 1;
}

/**
 * Constructor allowing the user to specify the mode of 
 * operation. The sample rate defaults to 44100.0Hz
 * @param m The mode of operation
 *			0:	Peak
 *			1:	Absolute Average
 *			2:	RMS
 *			3:	Cubic Mean
 *			4:	Absolute Midpoint
 */
Dynamics::Dynamics(unsigned int m){
	Fs = 44100.00;
	if(m<5) mode = m;
	else mode = 0;
	atkSample = -1;
	relSample = -1;
	effRatio = 1;
}

/**
 * Constructor allowing the user to specify the mode of 
 * operation and the sample rate
 * @param m The mode of operation
 *			0:	Peak
 *			1:	Absolute Average
 *			2:	RMS
 *			3:	Cubic Mean
 *			4:	Absolute Midpoint
 * @param fs The sample rate in Hz
 */
Dynamics::Dynamics(unsigned int m, double fs){
	Fs = fs;
	if(m<5)mode = 5;
	else mode=0;
	atkSample = -1;
	relSample = -1;
	effRatio = 1;
}

/**
 * setParamsPc provides an interface to simultaneously update all parameters of the dynamics modifier.
 * All amplitude values should be supplied in %
 * @param fs The Sample frequency in Hz
 * @param t The new threshold for the dynamics modifier in %
 * @param rat The new ratio for the dynamics modifier
 * @param a The attack of the dynamics modifier in samples
 * @param r The release of the dynamics modifier in samples
 * @param k The knee of the dynamics modifier in %
 * @param g The makeup gain of the dynamics modifier in %
 */
void Dynamics::setParamsPc(double t, double rat, int a, int r, double k, double g){
	thresh = t;
	ratio = rat;
	atk = a;
	rel = r;
	knee = k;
	gain = g;
}

/**
 * setParamsPc provides an interface to simulatneously update all paramters of the dynamics modifier.
 * All Amplitude values should be supplied in %.
 * @param fs The sample frequency in Hz
 * @param t The threshold in %
 * @param rat The ratio for the dynamics modifier
 * @param a The attack of the dynamics modifier in ms
 * @param r The release of the dynnamics modifier in ms
 * @param k The knee of the the dynamics modifier in percent
 * @param g The gain of the dynamics modifier in percent
 */
void Dynamics::setParamsPc( double t, double rat, double a, double r, double k, double g){
	setParamsPc(t, rat, msToS(a), msToS(r), k, g); 
}

/**
 * setParamsdB provides an interface to simultaneously update all the parametres of the dynamics
 * modifier. All amplitude values should be provided in dB.
 * @param fs The Sample Frequency in Hz
 * @param t The new threashold for the dynamics modifier in dB
 * @param rat The new ratio for the dynamics modifier
 * @param a The attack of the dynamics modifier in samples
 * @param r The release of the dynamics modifier in samples
 * @param k The knee of the dynamics modifier in dB
 * @param g The make up gain of the dynamics modifier in dB
 */
void Dynamics::setParamsdB(double t, double rat, int a, int r, double k, double g){
	setParamsPc(dBtoPc(t), rat, a, r, dBtoPc(k), dBtoPc(g));
}

/**
 * setParamsdB provides an interface to simulatneously update all the parameters of the dynamics
 * modifier. All applicable values should be supplied in dB
 * @param fs The Sample Frequency
 * @param t The new threshold for the dynamics modifier in dB
 * @param rat The new ratio for the dynamics modifier
 * @param a The new attack for the dyanmics modifier in ms
 * @param r The new release for the dynamics modifier in ms
 * @param k The new knee for the dynamics modifier in dB
 * @param g The new make up gain for the dynamics modifier in dB
 */
void Dynamics::setParamsdB(double t, double rat, double a, double r, double k, double g){
	setParamsPc(dBtoPc(t), rat, msToS(a), msToS(r), dBtoPc(k), dBtoPc(g));
}

/**
 * getBufferLevel determines which averaging algorithm
 * to use in the compressor
 * @param input The input buffer to get the average of
 * @param nFrames The number of samples in the input buffer
 * @see Averages
 * @return The average level of the input buffer
 */
double Dynamics::getBufferLevel(double *input, int nFrames){
	switch(mode){
	case 1:
		return absAvg(input,nFrames);
	case 2:
		return rms(input,nFrames);
	case 4:
		return absMidPoint(input,nFrames);
	case 0:
	case 3: //TODO cubic mean
	default:
		return peak(input,nFrames);
	}
}

/**
 * processBuffer applies the compression to an input
 * @param input The input sample buffer
 * @param output Pass by call output buffer
 * @param nFrames The number of samples in the input and output buffer
 */
void Dynamics::processBuffer(double *input, double *output, int nFrames){
	double l = getBufferLevel(input, nFrames);

	//process audio
	for(int i = 0; i < nFrames; i++){	
		double effGain = 1;
	
		//set effective gain
		if(ratio < 1){//compressor, limited, inverter
			//check if we are engaged
			if(l > thresh){
				//check if we are in the attack
				if(effRatio > ratio) effRatio -= effRatio / atk;

				//check if we overshot
				if(effRatio < ratio) effRatio = ratio;
			}
			else{//we are disengaged
				//check if we are in the release
				if(effRatio < 1) effRatio += effRatio / rel;

				if(effRatio > 1) effRatio = 1;
			}
			//knee
			if(l - thresh > knee) kMod = 1;
			else kMod = knee / (l - thresh);
			effGain = 1 - (knee * effRatio);
		}
		else{//Expander
			//we are engaged
			if(l > thresh){
				//check if we are in atk
				if(effRatio < ratio) effRatio += effRatio / atk;

				//check if we went above ratio
				if(effRatio > ratio) effRatio = ratio;
			}
			else{	//we are disengaged
				//check if we are in the release
				if(effRatio > 1) effRatio -= effRatio / rel;

				//check if we overshot unity
				if(effRatio < 1) effRatio = 1;
			}

			//knee
			if(l - thresh > knee) kMod = 1;
			else kMod = (l - thresh) / knee;

			effGain = 1 + (kMod * effRatio);
		}
		output[i] = gain* effGain * input[i];
	}
}

/**
 * processBuffer applies the compression to an input
 * @param input The input sample buffer
 * @param sideChain The input buffer for a sidechain
 * @param output Pass by call output buffer
 * @param nFrames The number of samples in the input and output buffer
 */
void Dynamics::processBuffer(double *input, double *sideChain, double *output, int nFrames){
	double l = getBufferLevel(sideChain, nFrames);

	//process audio
	for(int i = 0; i < nFrames; i++){	
		double effGain = 1;
	
		//set effective gain
		if(ratio < 1){//compressor, limited, inverter
			//check if we are engaged
			if(l > thresh){
				//check if we are in the attack
				if(effRatio > ratio) effRatio -= effRatio / atk;

				//check if we overshot
				if(effRatio < ratio) effRatio = ratio;
			}
			else{//we are disengaged
				//check if we are in the release
				if(effRatio < 1) effRatio += effRatio / rel;

				if(effRatio > 1) effRatio = 1;
			}
			//knee
			if(l - thresh > knee) kMod = 1;
			else kMod = knee / (l - thresh);
			effGain = 1 - (knee * effRatio);
		}
		else{//Expander
			//we are engaged
			if(l > thresh){
				//check if we are in atk
				if(effRatio < ratio) effRatio += effRatio / atk;

				//check if we went above ratio
				if(effRatio > ratio) effRatio = ratio;
			}
			else{	//we are disengaged
				//check if we are in the release
				if(effRatio > 1) effRatio -= effRatio / rel;

				//check if we overshot unity
				if(effRatio < 1) effRatio = 1;
			}

			//knee
			if(l - thresh > knee) kMod = 1;
			else kMod = (l - thresh) / knee;

			effGain = 1 + (kMod * effRatio);
		}
		output[i] = gain* effGain * input[i];
	}
}