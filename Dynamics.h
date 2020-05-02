#ifndef DYNAMICS_H
#define DYNAMICS_H

//include
#include <cmath>
#include "Averages.cpp"

/**
 * Dynamics is a class that is used to alter the dynamic range
 * It can operate as an Expander, Compressor, Limiter,
 * Transient Reducer or Peak Inverter.
 * @author Ryan Khan Logan
 * @see Averages.cpp
 */
class Dynamics{
protected:
	//User Parameters
	double Fs;
	double thresh;			//holds the thresh in percentage
	/* 
	 * Ratio for the module
	 *	RANGE			EFFECT
	 *	]-inf,	0[		Peak Inverter
	 *	[0,		0]		Limiter
	 *	]0,		1[		Compressor
	 *	[1,		1]		Transient Reducer
	 *	]1,		+inf[	Expander
	 */
	double ratio;		
	int atk;				//attack in samples
	int rel;				//release in samples
	double knee;			//knee width in %
	double gain;			//makeup gain in %

	/********************internal parameters********************/
	//which sample we are at in attack
	//-1 flags that we are not in attack phase
	int atkSample;
	//which sample we are at in release
	//-1 flags that we are not in release phase
	int relSample;			
	//modifier for ratio during attack or release phase
	//0 when DM is disengaged
	//1 when attack is completed
	double atkMod;
	double relMod;
	double kMod;
	//The effective ration of the DM
	double effRatio;

	/*
	 * mode of operation
	 * 0:	Peak
	 * 1:	Absolute Average
	 * 2:	RMS
	 * 3:	Cubic Mean
	 * 4:	Absolute Midpoint
	 */
	unsigned int mode; 
public:
	/**
	 * Default constructor. Defaults to peak value mode with 
	 * a sample rate of 44100.0Hz
	 */
	Dynamics();

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
	Dynamics(unsigned int m);

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
	Dynamics(unsigned int m, double fs);

	/***** Setters for User Parameters *****/
	void setFs(double fs){Fs = fs;}
	void setThreshPc(double t){thresh = t;}
	void setThreshdB(double t){thresh = dBtoPc(t);}
	void setRatio(double r){thresh = r;}
	void setAtk(int a){atk = a;}
	void setAtk(double a){atk = msToS(a);}
	void setRel(int r){rel = r;}
	void setRel(double r){rel = msToS(r);}
	void setKneePc(double k){knee = k;}
	void setKneedB(double k){knee = dBtoPc(k);}
	void setGainPc(double g){gain = g;}
	void setGaindB(double g){gain = dBtoPc(g);}

	/***** Getters For User Parameters *****/
	unsigned int getMode(){return mode;}
	double getFs(){return Fs;}
	double getThresh(){return thresh;}
	double getThreshdB(){return PcTodB(thresh);}
	double getRatio(){return ratio;}
	int getAtk(){return atk;}
	double getAtkMS(){return atk/Fs;}
	int getRel(){return rel;}
	double getRelMS(){return rel/Fs;}
	double getKnee(){return knee;}
	double getKneedB(){return PcTodB(knee);}
	double getGain(){return gain;}
	double getGaindB(){return PcTodB(knee);}

	/*** Getters For Internal Parameters ***/
	int getAtkSample(){return atkSample;}	
	int getRelSample(){return relSample;}			
	double getAtkMod(){return atkMod;}
	double getRelMod(){return relMod;}
	double getkMod(){return kMod;}
	double getEffRatio(){return effRatio;}

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
	void setParamsPc(double t, double rat, int a, int r, double k, double g);

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
	void setParamsPc( double t, double rat, double a, double r, double k, double g);
	
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
	void setParamsdB(double t, double rat, int a, int r, double k, double g);

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
	void setParamsdB(double t, double rat, double a, double r, double k, double g);
		
	/**
	 * msToS converts ms to samples given the current sample rate
	 * @param ms The value to be converted in ms
	 * @return The number of samples in ms time.
	 */
	int msToS(double ms){return ms * Fs;}

	/**
	 * Helper function to convert dB values to percentages
	 * @param dB The decibel value to be converted
	 * @return A double containing the percentage.
	 */
	double dBtoPc(double dB){return pow(10, dB/20);}

	/**
	 * PcTodB is a helper function to convert from percentages
	 * to dB values
	 * @param Pc The percentage value to convert
	 * @return The corresponding dB value
	 */
	double PcTodB(double Pc){return 20*log10(Pc);	}

	/**
	 * getBufferLevel determines which averaging algorithm
	 * to use in the compressor
	 * @param input The input buffer to get the average of
	 * @param nFrames The number of samples in the input buffer
	 * @see Averages
	 * @return The average level of the input buffer
	 */

	double getBufferLevel(double *input, int nFrames);

	/**
	 * processBuffer applies the compression to an input
	 * @param input The input sample buffer
	 * @param output Pass by call output buffer
	 * @param nFrames The number of samples in the input and output buffer
	 */
	void processBuffer(double *input, double *output, int nFrames);

	/**
	 * processBuffer applies the compression to an input
	 * @param input The input sample buffer
	 * @param sideChain The input buffer for a sidechain
	 * @param output Pass by call output buffer
	 * @param nFrames The number of samples in the input and output buffer
	 */
	void Dynamics::processBuffer(double *input, double *sideChain, double *output, int nFrames);
};

#endif