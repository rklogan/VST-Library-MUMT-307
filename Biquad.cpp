#include "Biquad.h"

/**
 * Creates a filter with all parameters initialized to 0
 * @param m		  0:	Band-Pass Filter
				  1:	Low-Pass Filter
				  2:	High-Pass Filter
				  3:	Notch Filter
				  4:	All-Pass Filter
				  5:	Low-Shelf Filter
				  6:	High-Shelf Filter
			default:	Band-Pass Filter
 * @param bwm	  0:	Q
				  1:	-3dB bandwidth
				  2:	dB/octave slope
			default:	Q
 */
Biquad::Biquad(unsigned int m, unsigned int bwm){
	mode = m;
	BWMode = bwm;
	initHist();

	switch(BWMode){
	case 1:
		setParamsBW(0, 0, 0);
		break;
	case 2:
		setParamsSlope(0, 0, 0, 0);
		break;
	case 0:
	default:
		setParamsQ(0, 0, 0);
	}
}

/**
 * General constructor.
 * @param m		  0:	Band-Pass Filter
				  1:	Low-Pass Filter
				  2:	High-Pass Filter
				  3:	Notch Filter
				  4:	All-Pass Filter
				  5:	Low-Shelf Filter
				  6:	High-Shelf Filter
			default:	Band-Pass Filter
 * @param bwm	  0:	Q
				  1:	-3dB bandwidth
				  2:	dB/octave slope
			default:	Q
 * @param fs Sample Frequency
 * @param f Center Frequency
 * @param q Holds q value of bwm=0; -3dB bandwidth if bwm=1;
			dB/octave slope if bwm=2
 * @param dbg Holds the gain of the filter in dB. Only valid
				for shelving filters. For any other type of
				filter this parameter has no effect.
 */
Biquad::Biquad(unsigned int m, unsigned int bwm, double fs, double f, double q, double dbg){
	mode = m;
	BWMode = bwm;

	switch(bwm){
	case 1:
		setParamsBW(fs, f, q);
		break;
	case 2:
		setParamsSlope(fs, f, q, dbg);
		break;
	case 0:
	default:
		setParamsQ(fs, f, q);
	}
	
	initHist();
}

/**
 * Constructor for non-shelving filters. If this constructor
 * is used on a shelving filter, it will generate a shelf
 * with 0dB gain unless later specified.
 * @param m		  0:	Band-Pass Filter
				  1:	Low-Pass Filter
				  2:	High-Pass Filter
				  3:	Notch Filter
				  4:	All-Pass Filter
				  5:	Low-Shelf Filter
				  6:	High-Shelf Filter
			default:	Band-Pass Filter
 * @param bwm	  0:	Q
				  1:	-3dB bandwidth
				  2:	dB/octave slope
			default:	Q
 * @param fs Sample Frequency
 * @param f Center Frequency
 * @param q Holds q value of bwm=0; -3dB bandwidth if bwm=1;
			dB/octave slope if bwm=2
 */
Biquad::Biquad(unsigned int m, unsigned int bwm, double fs, double f, double q){
	Biquad(m, bwm, fs, f, q, 0.0);
}

bool Biquad::setParamsSlope(double fs, double f, double s, double dbg){
	if(mode != 5 && mode != 6) return false;
	if(BWMode != 2) return false;
	if(fs >= 0) Fs = fs;
	if(f >= 0) fc = f;
	if(s >= 0) slope = s;
	dBG = dbg;

	updateOmega(true);
	updateGain();
	updateAlpha();
	updateAlphaPrime();
	updateCoFs();
}

void Biquad::initHist(){
	x0 = 0;
	x1 = 0;
	x2 = 0;
	y1 = 0;
	y2 = 0;
}

bool Biquad::setParamsBW(double fs, double f, double b){
	if(mode != 0 && mode != 3) return false;
	if(BWMode != 1) return false;
	if(fs >= 0) Fs = fs;
	if(f >= 0) fc = f;
	if(b >= 0) BW = b;
	updateOmega(true);
	updateAlpha();
	updateCoFs();
	return true;
}

void Biquad::setParamsQ(double fs, double f, double q){
	if(fs >= 0) Fs = fs;
	if(f >= 0) fc = f;
	if (q >= 0) Q = q;
	updateOmega(true);
	updateAlpha();
	updateCoFs();
}

/**
 * setFs provides an interface for changing the sample
 * frequency of the filter. The parameter updateDeps
 * can be set to also change all parameters that depend on
 * Fs. This flag should only be set if no other parameters
 * of the filter will be changed. 
 * @param f The new Fs
 * @param updateDeps IFF true, all dependant parameters will be updated
 * @return true IFF the value of Fs was updated
 * @see Fs
 * @see BWMode
 * @see mode
 * @see updateW0()
 * @see updateSinW0()
 * @see updateCosW0()
 * @see updateAlpha()
 * @see updateAlphaPrime()
 * @see updateCoFs()
 */
bool Biquad::setFs(double f, bool updateDeps){
	if(f >= 0) Fs = f;
	else return false;
	if(updateDeps){
		updateOmega(true);
		updateSinW0();
		updateCosW0();
		updateAlpha();
		if(BWMode == 2 && (mode==5 || mode==6)) updateAlphaPrime();
		updateCoFs();
	}
	return true;
}

/**
 * setFc provides an interface for changing the center 
 * frequency of the filter. The parameter updateDeps
 * can be set to also change all parameters that depend on
 * fc. This flag should only be set if no other parameters
 * of the filter will be changed. 
 * @param f The new fc
 * @param updateDeps IFF true, all dependant parameters will be updated
 * @return true IFF the value of fc was updated
 * @see fc
 * @see BWMode
 * @see mode
 * @see updateW0()
 * @see updateSinW0()
 * @see updateCosW0()
 * @see updateAlpha()
 * @see updateAlphaPrime()
 * @see updateCoFs()
 */
bool Biquad::setFc(double f, bool updateDeps){
	if(f >= 0) fc = f;
	else return false;
	if(updateDeps){
		updateOmega(true);
		updateSinW0();
		updateCosW0();
		updateAlpha();
		if(BWMode == 2 && (mode==5 || mode==6)) updateAlphaPrime();
		updateCoFs();
	}
	return true;
}

/**
 * setQ provides an interface for the user to provude the q of
 * the filter. 
 * @param q The new value for Q
 * @param updateDeps IFF true alpha and coFs will also be updated. Should only be set if no other parameters are being changed.
 * @return true IFF the value of Q was changed
 */
bool Biquad::setQ(double q, bool updateDeps){
	if (q < 0) return false;
	else Q = q;
	if(updateDeps){
		updateAlpha();
		updateCoFs();
	}
	return true;
}

/**
 * setBW provides an interface for the user to provide the 3dB
 * bandwidth for either a BPF or a notch filter. The filter must
 * be in either of these two modes for this function to work.
 * The filter must also be in BWMode 1.
 * @param b The value of the -3dB bandwidth in octaves
 * @param updateDeps IFF true alpha and coFs will also be updated. Should only be set if no other parameters are being changed
 * @return true IFF the value of BW was changed
 */
bool Biquad::setBW(double b, bool updateDeps){
	if(mode != 0 && mode != 3) return false;
	else if(BWMode != 1) return false;
	else if(b < 0) return false;
	else BW = b;
	if(updateDeps){
		updateAlpha();
		updateCoFs();
	}
	return true;
}

/**
 * setSlope provides an interface for the user to provide a slope
 * value to the filter in dB/octave. The filter must be in a
 * shelving filter mode, and the BWMode must be 2 for this 
 * function to work.
 * @param s The value of the slope in dB/octave
 * @param updateDeps IFF true, alpha alpha' and coFs will be updated as well. Should only be set if no other parameters are being changed
 * @return true if the value was updated and false otherwise
 */
bool Biquad::setSlope(double s, bool updateDeps){
	if(mode != 5 && mode != 6) return false;
	else if(BWMode != 2) return false;
	else if(s < 0) return false;
	else{
		slope = s;
	}
	if(updateDeps){
		updateAlpha();
		updateAlphaPrime();
		updateCoFs();
	}
	return true;
}

/**
 * update Alpha() changes the value of alpha based on the value
 * stored in sinW0 and Q, BW, or slope depending on BWMode.
 * @see math.sinh()
 * @see math.sqrt()
 * @see sinW0
 * @see gain
 * @see omega0
 * @see slope
 * @see BW
 * @see Q
 */
void Biquad::updateAlpha(){
	switch(BWMode){
	case 1:
		alpha = (sinW0 * sinh( (LN2) / 2 * BW * omega0/sinW0));
		break;
	case 2:
		alpha = sinW0/2 * sqrt( (gain+1/gain)*(1/slope-1)+2 );
		break;
	case 0:
	default:
		alpha = sinW0 / (2 * Q);
	}
}

/**
 * updateAlphaPrime will update alphaPrime based on hte values 
 * stored in gain and alpha if we are in a shelving mode and
 * do nothing otherwise
 * @see math.sqrt()
 * @return true If alphaPrime was updated and false otherwise
 */
bool Biquad::updateAlphaPrime(){
	if(mode==5 || mode==6){
		alphaPrime = 2 * sqrt(gain)*alpha;
		return true;
	}
	return false;
}

/**
 * updateGain is used to update the gain parameter when the db
 * gain is changed by the user. Note this function does not do
 * anything if the Biquad filter is not a shelving filter.
 * @see dBG
 * @see gain
 * @return true IFF we are a shelving filter and parameters were updated.
 */
bool Biquad::updateGain(){
	if(mode==5 || mode ==6){
		gain = pow(10, dBG/40);
		return true;
	}
	else return false;
}

/**
 * updateOmega is used to update the value of omega based on the
 * values currently stored in fc and Fs. 
 * @param bool updateTrigs If true sinW0 and cosW0 will also be updated
 * @see fc
 * @see Fs
 */
void Biquad::updateOmega(bool updateTrigs){
	//update value of omega0
	omega0 = 2 * M_PI * fc / Fs;
	if(updateTrigs){
		updateCosW0();
		updateSinW0();
	}
}

/**
 * updateCosW0 updates the global parameter cosW0 based on the
 * value currently stored in omega0. 
 * @see math.cos()
 * @see omega0
 */
void Biquad::updateCosW0(){
	cosW0 = cos(omega0);
}

/**
 * updateSinW0 updates the global parameter sinW0 based on the 
 * value currently stored in omega0. 
 * @see math.sin()
 * @see omega0
 */
void Biquad::updateSinW0(){
	sinW0 = sin(omega0);
}

/**
 * updateCoFs is used to simultaneously update all the 
 * coefficients of the filter.
 * @see updateAs()
 */
void Biquad::updateCoFs(){
	//switch to update a value
	switch(mode){
	case 5:break; //case 5 and 6 will be updated later
	case 6:break;
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	default:
		updateAs();
	}

	//switch to update Bs
	switch(mode){
	case 1:
	case 2:
		if(mode==1) b1 = 1 - cosW0;
		else if(mode==2) b1 = 1 + cosW0;
		b0 = b1/2;
		b2 = b0;
		if(mode==2) b1 *= -1;
		break;
	case 3:
		b0 = 1;
		b2 = 1;
	case 4:
		b1 = a1;
		if(mode==3)break;
		b0 = a2;
		b2 = a0;
		break;
	case 5:
	case 6:
		if(true){
			//calculate some intermediate vars
			double gp = gain+1;
			double gm = gain-1;
			double mc = gm*cosW0;
			double pc = gp*cosW0;
			double pmm = gp - mc;
			double mmp = gm - pc;
			double ppm = gp + mc;
			double mpp = gm + pc;
			if(mode==5){
				b0 = gain * (pmm + alphaPrime);
				b1 = 2 * gain * mmp;
				b2 = gain * (pmm - alphaPrime);
				a0 = ppm + alphaPrime;
				a1 = -2 * mpp;
				a2 = ppm - alphaPrime;
			}
			else if(mode==6){
				b0 = gain * (ppm + alphaPrime);
				b1 = -2 * gain * mpp;
				b2 = gain * (ppm -  alphaPrime);
				a0 = pmm + alphaPrime;
				a1 = -2 * mmp;
				a2 = pmm - alphaPrime;
			}
		}
		break;
	case 0:
	default:
		b0 = alpha;
		b1 = 0;
		b2 = -1 * alpha;
	}
}

/**
 * updateBs changes the values of b coefficiens. Not optimized for
 * shelving filters. Use update Cofs instead in these cases.
 * @see updateCoFs()
 * @see b0
 * @see b1
 * @see b2
 */
void Biquad::updateBs(){
	double tmp = 0;
	switch(mode){
	case 1:
		b1 = 1 - cosW0;
		b0 = b1/2;
		b2 = b0;
		break;
	case 2:
		b1 = 1 + cosW0;
		b0 = b1 / 2;
		b1 *= -1;
		b2 = b0;
		break;
	case 3:
		b0 = 1;
		b1 = -2 * cosW0;
		b2 = 1;
		break;
	case 4:
		b0 = 1 - alpha;
		b1 = -2 * cosW0;
		b2 = 1 + alpha;
		break;
	case 5:
		tmp = (gain+1) - (gain-1)*cosW0;
		b0 = gain * (tmp + alphaPrime);
		b1 = 2 * gain * ( (gain-1) - (gain+1)*cosW0);
		b2 = gain * (tmp - alphaPrime);
		break;
	case 6:
		tmp = (gain+1) + (gain-1)*cosW0;
		b0 = gain * (tmp + alphaPrime);
		b1 = -2 * gain * ( (gain-1) + (gain+1)*cosW0);
		b2 = gain * (tmp - alphaPrime);
		break;
	case 0:
	default:
		b0 = alpha;
		b1 = 0;
		b2 = -1 * alpha;
	}
}

/**
 * updateB0 changes the value of b0. Not optimized for LPF, HPF
 * and shelving filters. Use update Cofs instead in these cases.
 * For LPF and HPF the performance of updateBs() will be 
 * equivalent to that of updateCoFs
 * @see updateCoFs()
 * @see updateBs()
 * @see b0
 */
void Biquad::updatetB0(){
	switch(mode){
	case 1:
		b0 = (1 - cosW0) / 2;
		break;
	case 2:
		b0 = (1 + cosW0) / 2;
		break;
	case 3:
		b0 = 1;
		break;
	case 4:
		b0 = 1 - alpha;
		break;
	case 5:
		b0 = gain * ( (gain+1) - (gain-1)*cosW0 + alphaPrime);
		break;
	case 6:
		b0 = gain * ( (gain+1) + (gain-1)*cosW0 + alphaPrime);
		break;
	case 0:
	default:
		b0 = alpha;
	}
}

/**
 * updateB1 changes the value of b1. Not optimized for LPF, HPF
 * and shelving filters. Use update Cofs instead in these cases.
 * For LPF and HPF the performance of updateBs() will be 
 * equivalent to that of updateCoFs
 * @see updateCoFs()
 * @see updateBs()
 * @see b1
 */
void Biquad::updateB1(){
	switch(mode){
	case 1:
		b1 = 1 - cosW0;
		break;
	case 2: 
		b1 = -1 * (1 + cosW0);
		break;
	case 3:
	case 4:
		b1 = -2 * cosW0;
		break;
	case 5:
		b1 = 2 * gain * ( (gain-1) - (gain+1)*cosW0);
		break;
	case 6:
		b1 = -2 * gain * ( (gain-1) + (gain+1)*cosW0);
		break;
	case 0:
	default:
		b1 = 0;
	}
}

/**
 * updateB2 changes the value of b2. Not optimized for LPF, HPF
 * and shelving filters. Use update Cofs instead in these cases.
 * For LPF and HPF the performance of updateBs() will be 
 * equivalent to that of updateCoFs
 * @see updateCoFs()
 * @see updateBs()
 * @see b2
 */
void Biquad::updateB2(){
	switch(mode){
	case 1:
		b2 = (1 - cosW0) / 2;
		break;
	case 2:
		b2 = (1 + cosW0) / 2;
		break;
	case 3:
		b2 = 1;
		break;
	case 4:
		b2 = 1 + alpha;
		break;
	case 5:
		b2 = gain * ((gain+1) - (gain-1)*cosW0 - alphaPrime);
		break;
	case 6:
		b2 = gain * ((gain+1) + (gain-1)*cosW0 - alphaPrime);
		break;
	case 0:
	default:
		b2 = -1 * alpha;
	}
}

/**
 * updateAs is used to update all a parameters at the same time.
 * This method is not optimized for shelving filters. 
 * updateCoFs() should be used instead.
 * @see updateCoFs()
 * @see a0
 * @see a1
 * @see a2
 */
void Biquad::updateAs(){
	double tmp = 0;
	switch(mode){
	case 5:
		tmp = (gain+1) + (gain-1)*cosW0;
		a0 = tmp + alphaPrime;
		a1 = -2 * ( (gain-1) + (gain+1)*cosW0);
		a2 = tmp - alphaPrime;
		break;
	case 6:
		tmp = (gain+1) - (gain-1)*cosW0;
		a0 = tmp + alphaPrime;
		a1 = 2 * ( (gain-1) - (gain+1)*cosW0);
		a2 = tmp - alphaPrime;
		break;
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	default:
		a0 = 1 + alpha;
		a1 = -2 * cosW0;
		a2 = 1 - alpha;
	}
}

/**
 * updateA0 changes the value of a0. Not optimized for shelving
 * filters. updateCoFs should be used instead
 * @see updateCoFs()
 * @see updateAs()
 * @see a2
 */
void Biquad::updateA0(){
	switch(mode){
	case 5:
		a0 = (gain+1) + (gain-1)*cosW0 + alphaPrime;
		break;
	case 6:
		a0 = (gain+1) - (gain-1)*cosW0 + alphaPrime;
		break;
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	default:
		a0 = a0 + alpha;
	}
}

/**
 * updateA1 changes the value of a1. Not optimized for shelving
 * filters. updateCoFs should be used instead
 * @see updateCoFs()
 * @see updateAs()
 * @see a1
 */
void Biquad::updateA1(){
	switch(mode){
	case 5:
		a1 = -2 * ((gain-1) + (gain+1)*cosW0);
		break;
	case 6:
		a1 = 2 * ((gain-1) + (gain+1)*cosW0);
		break;
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	default:
		a1 = -2 * cosW0;
	}
}

/**
 * updateA2 changes the value of a2. Not optimized for shelving
 * filters. updateCoFs should be used instead
 * @see updateCoFs()
 * @see updateAs()
 * @see a2
 */
void Biquad::updateA2(){
	switch(mode){
	case 5:
		a2 = (gain+1) + (gain-1)*cosW0 - alphaPrime;
		break;
	case 6:
		a2 = (gain+1) - (gain-1)*cosW0 - alphaPrime;
		break;
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	default:
		a2 = 1-alpha;
	}
}

/**
 * updateXs is used to update the x values in the history.
 * @param x The current value of x to be used for processing (x0).
 * @see x0
 * @see x1
 * @see x2
 */
void Biquad::updateXs(double x){
	x2 = x1;
	x1 = x0;
	x0 = x;
}

/**
 * updateYs is used to update the y value in the history
 * @param y The value of y to be stored as the last output sample (y1).
 * @see y1
 * @see y2
 */
void Biquad::updateYs(double y){
	y2 = y1;
	y1 = y;
}

/**
 * processBuffer applied the filter to a sample buffer
 * @param input A double array of size nFrames holding the intput signal
 * @param output A double buffer of size nFrames for a pass by call output
 * @param nFrames The number of samples in the input and output buffer.
 */
void Biquad::processBuffer(double *input, double *output, int nFrames){
	for(int i = 0; i < nFrames; i++){
		updateXs(input[i]);
		output[i] = generateOutputSample();
		updateYs(output[i]);
	}
}

/**
 * generateOutputSample returns one sample of output based
 * on the values contained in the history.
 * @return a double containing the output of the filter.
 */
double Biquad::generateOutputSample(){
	double acc = b0 * x0;
	acc += b1 * x1;
	acc += b2 * x2;
	acc -= a1 * y1;
	acc -= a2 * y2;
	return acc / a0;
}

void Biquad::getBs(double *bs){
	bs[0] = b0;
	bs[1] = b1;
	bs[2] = b2;
}
void Biquad::getAs(double *as){
	as[0] = a0;
	as[1] = a1;
	as[2] = a2;
}
void Biquad::getCoFs(double *as, double *bs){
	getAs(as);
	getBs(bs);
}
void Biquad::getXHist(double *xs){
	 xs[0] = x0;
	 xs[1] = x1;
	 xs[2] = x2;
 }
void Biquad::getYHist(double *ys){
	 ys[0] = y1;
	 ys[1] = y2;
 }
void Biquad::getHist(double *xs, double *ys){
	 getXHist(xs);
	 getYHist(ys);
}
