#ifndef BIQUAD_H
#define BIQUAD_H

#include <cmath>

//global paramters
#define M_PI 3.14159265358
#define LN2 0.69314718056

class Biquad{
protected:
	/* The mode parameter is used to change the mode of operation
	 * of the biqaud filter.
	 * 0: Band-Pass Filter
	 * 1: Low-Pass Filter
	 * 2: High-Pass Filter
	 * 3: Notch Filter
	 * 4: All-Pass Filter
	 * 5: Low-Shelf Filter
	 * 6: High-Shelf Filter
	 * Other values will default to a band pass filter */
	unsigned int mode;
	double Fs;							//Sample Freqeuncy
	double fc;							//center/cutoff frequency
	//bandwitdth related parameters.
	unsigned int BWMode;
	double Q;							
	double BW;
	double slope;
	double alpha;						//intermediary BW parameter
	double alphaPrime;					//intermediary BW parameter
	
	double dBG;						//user parameter for gain in db. 
	double gain;						//only for shelving Filters
	
	double omega0;						//angular frequency
	double cosW0;						
	double sinW0;
	
	//Filter co-efficients
	double b0;						
	double b1;
	double b2;
	double a0;
	double a1;
	double a2;

	//delay lines
	double x0, x1, x2;
	double y1, y2;

public:
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
	Biquad(unsigned int m, unsigned int bwm);

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
	Biquad(unsigned int m, unsigned int bwm, double fs, double f, double q, double dbg);
	
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
	Biquad(unsigned int m, unsigned int bwm, double fs, double f, double q);

	bool setParamsSlope(double fs, double f, double s, double dbg);

	void initHist();

	bool setParamsBW(double fs, double f, double b);

	void setParamsQ(double fs, double f, double q);

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
	bool setFs(double f, bool updateDeps);

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
	bool setFc(double f, bool updateDeps);

	/**
	 * setQ provides an interface for the user to provude the q of
	 * the filter. 
	 * @param q The new value for Q
	 * @param updateDeps IFF true alpha and coFs will also be updated. Should only be set if no other parameters are being changed.
	 * @return true IFF the value of Q was changed
	 */
	bool setQ(double q, bool updateDeps);

	/**
	 * setBW provides an interface for the user to provide the 3dB
	 * bandwidth for either a BPF or a notch filter. The filter must
	 * be in either of these two modes for this function to work.
	 * The filter must also be in BWMode 1.
	 * @param b The value of the -3dB bandwidth in octaves
	 * @param updateDeps IFF true alpha and coFs will also be updated. Should only be set if no other parameters are being changed
	 * @return true IFF the value of BW was changed
	 */
	bool setBW(double b, bool updateDeps);

	/**
	 * setSlope provides an interface for the user to provide a slope
	 * value to the filter in dB/octave. The filter must be in a
	 * shelving filter mode, and the BWMode must be 2 for this 
	 * function to work.
	 * @param s The value of the slope in dB/octave
	 * @param updateDeps IFF true, alpha alpha' and coFs will be updated as well. Should only be set if no other parameters are being changed
	 * @return true if the value was updated and false otherwise
	 */
	bool setSlope(double s, bool updateDeps);

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
	void updateAlpha();

	/**
	 * updateAlphaPrime will update alphaPrime based on hte values 
	 * stored in gain and alpha if we are in a shelving mode and
	 * do nothing otherwise
	 * @see math.sqrt()
	 * @return true If alphaPrime was updated and false otherwise
	 */
	bool updateAlphaPrime();

	/**
	 * updateGain is used to update the gain parameter when the db
	 * gain is changed by the user. Note this function does not do
	 * anything if the Biquad filter is not a shelving filter.
	 * @see dBG
	 * @see gain
	 * @return true IFF we are a shelving filter and parameters were updated.
	 */
	bool updateGain();

	/**
	 * updateOmega is used to update the value of omega based on the
	 * values currently stored in fc and Fs. 
	 * @param bool updateTrigs If true sinW0 and cosW0 will also be updated
	 * @see fc
	 * @see Fs
	 */
	void updateOmega(bool updateTrigs);

	/**
	 * updateCosW0 updates the global parameter cosW0 based on the
	 * value currently stored in omega0. 
	 * @see math.cos()
	 * @see omega0
	 */
	void updateCosW0();

	/**
	 * updateSinW0 updates the global parameter sinW0 based on the 
	 * value currently stored in omega0. 
	 * @see math.sin()
	 * @see omega0
	 */
	void updateSinW0();

	/**
	 * updateCoFs is used to simultaneously update all the 
	 * coefficients of the filter.
	 * @see updateAs()
	 */
	void updateCoFs();

	/**
	 * updateBs changes the values of b coefficiens. Not optimized for
	 * shelving filters. Use update Cofs instead in these cases.
	 * @see updateCoFs()
	 * @see b0
	 * @see b1
	 * @see b2
	 */
	void updateBs();

	/**
	 * updateB0 changes the value of b0. Not optimized for LPF, HPF
	 * and shelving filters. Use update Cofs instead in these cases.
	 * For LPF and HPF the performance of updateBs() will be 
	 * equivalent to that of updateCoFs
	 * @see updateCoFs()
	 * @see updateBs()
	 * @see b0
	 */
	void updatetB0();

	/**
	 * updateB1 changes the value of b1. Not optimized for LPF, HPF
	 * and shelving filters. Use update Cofs instead in these cases.
	 * For LPF and HPF the performance of updateBs() will be 
	 * equivalent to that of updateCoFs
	 * @see updateCoFs()
	 * @see updateBs()
	 * @see b1
	 */
	void updateB1();

	/**
	 * updateB2 changes the value of b2. Not optimized for LPF, HPF
	 * and shelving filters. Use update Cofs instead in these cases.
	 * For LPF and HPF the performance of updateBs() will be 
	 * equivalent to that of updateCoFs
	 * @see updateCoFs()
	 * @see updateBs()
	 * @see b2
	 */
	void updateB2();

	/**
	 * updateAs is used to update all a parameters at the same time.
	 * This method is not optimized for shelving filters. 
	 * updateCoFs() should be used instead.
	 * @see updateCoFs()
	 * @see a0
	 * @see a1
	 * @see a2
	 */
	void updateAs();

	/**
	 * updateA0 changes the value of a0. Not optimized for shelving
	 * filters. updateCoFs should be used instead
	 * @see updateCoFs()
	 * @see updateAs()
	 * @see a2
	 */
	void updateA0();

	/**
	 * updateA1 changes the value of a1. Not optimized for shelving
	 * filters. updateCoFs should be used instead
	 * @see updateCoFs()
	 * @see updateAs()
	 * @see a1
	 */
	void updateA1();

	/**
	 * updateA2 changes the value of a2. Not optimized for shelving
	 * filters. updateCoFs should be used instead
	 * @see updateCoFs()
	 * @see updateAs()
	 * @see a2
	 */
	void updateA2();

	/**
	 * updateXs is used to update the x values in the history.
	 * @param x The current value of x to be used for processing (x0).
	 * @see x0
	 * @see x1
	 * @see x2
	 */
	void updateXs(double x);

	/**
	 * updateYs is used to update the y value in the history
	 * @param y The value of y to be stored as the last output sample (y1).
	 * @see y1
	 * @see y2
	 */
	void updateYs(double y);

	/**
	 * processBuffer applied the filter to a sample buffer
	 * @param input A double array of size nFrames holding the intput signal
	 * @param output A double buffer of size nFrames for a pass by call output
	 * @param nFrames The number of samples in the input and output buffer.
	 */
	void processBuffer(double *input, double *output, int nFrames);

	/**
	 * generateOutputSample returns one sample of output based
	 * on the values contained in the history.
	 * @return a double containing the output of the filter.
	 */
	double generateOutputSample();

	unsigned int getMode(){return mode;}
	double getFs(){return Fs;}
	double getFc(){return fc;}
	unsigned int getBWMode(){return BWMode;}
	double getQ(){return Q;}							
	double getBW(){return BW;}
	double getSlope(){return slope;}
	double getAlpha(){return alpha;}
	double getdBGain(){return dBG;}
	double getOmega(){return omega0;}
	void getBs(double *bs);
	void getAs(double *as);
	void getCoFs(double *as, double *bs);
	 void getXHist(double *xs);
	 void getYHist(double *ys);
	 void getHist(double *xs, double *ys);
};
#endif