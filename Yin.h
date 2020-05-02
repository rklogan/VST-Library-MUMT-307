#ifndef YIN_H
#define YIN_H

/**
 * Yin is an object that implements the yin autocorrelation algorithm
 * to determine the fundamental frequency a signal
 * @see http://www.cs.tut.fi/~digaudio/htyo/lahteet/2002_JASA_YIN.pdf
 */
class Yin{
protected:
	double Fs;					//Sample rate
	double *buffer;				//buffer for correlation values
	int bufSize;				//size of buffer
	double prob;				//probability that the pitch obtained is correct
	double thresh;				//threshold for the yin algo

public:
	/**
	 * defaults all values. Not very useful... 
	 */
	void init();

	/**
	 * Helper function for the constructor to set all values
	 */
	void init(double fs, int b, double t);

	/**
	 * Default constructor. Sets sample rate to 44100 Hz
	 */
	Yin();

	/**
	 * Constructor allowing the user to specify all parameters
	 * @param {double} fs The sample rate
	 * @param {int} nFrames The number of samples in the input buffers to be analyzed.
	 * @param {double} t The threshold for the Yin algorithm
	 */
	Yin(double fs, int nFrames, double t);

	/**
	 * Constructor allowing the user to specify the sample rate and
	 * the threshold. This constructor is intended for use with
	 * getPitch(double *input, int nFrames)
	 * @param {double} fs The sample rate
	 * @param {double} t The threshold for the yin algorithm
	 * @see getPitch(double *input, int nFrames)
	 */
	Yin(double fs, double t);

	/**************** SETTERS ****************/
	void setFs(double fs){Fs = fs;}
	/**
	 * Sets the size of the buffer and resets all values in the 
	 * buffer to 0
	 */
	void setBufSize(int b);
	void setThresh(double t){thresh = t;}

	/**************** GETTERS ****************/
	double getFs(){return Fs;}
	void getBuffer(double *b){b = buffer;}
	int getBufSize(){return bufSize;}
	double getProb(){return prob;}
	double getThresh(){return thresh;}

	/**
	 * Calculates the squared difference of the signal with a shifted
	 * version of itself and stores the results in buffer
	 * @para, {double*} in A buffer holding the signal to be analyzed
	 */
	void squaredDiffs(double *in);

	/**
	 * Calculates the cumulative mean on the normalized difference
	 * of the data stored in buffer and puts the result in buffer.
	 */
	void CMND();

	/**
	 * Searches for the best correlation
	 * @return The t that produces the bes autocorrelation. -1 if not found.
	 */
	int absThresh();

	/**
	 * Performs parabolic interpolation on the estimated t to improve the value
	 * @param {int} est The current estimation of t
	 * @return The improved t value
	 */
	double interpolate(int est);

	/**
	 * getPitch applies the yin autocorrelation algorithm to a set of
	 * samples.
	 * @param {double} fs The Sample Rate in Hz
	 * @param {double} t The threshold for the Yin alorithm
	 * @param {double*} in The input buffer
	 * @param {int} nFrames The length of the input buffer
	 * @return The fundamental frequency.
	 */
	double getPitch(double fs, double t, double *in, int nFrames);

	/**
	 * Provides a user interface for a user who is applying multiple
	 * autocorrelations without changing the sample rate. The sample
	 * rate must be set before using this function.
	 * @param {double} t The threshold for the yin algorithm
	 * @param {double*} in The input buffer
	 * @param nFrames The size of the input buffer
	 * @return The fundamental frequency of the input buffer.
	 */
	double getPitch(double t, double *in, int nFrames);

	/**
	 *  Provides a user interface for a user who is applying multiple
	 * autocorrelations without changing the sample rate or threshold.
	 * This function requires that values for Fs and threshold be
	 * specified before use.
	 * @param {double*} in The input buffer
	 * @param {int} nFrames The size of the input buffer
	 * @return The fundamental frequency of the input buffer
	 */
	double getPitch(double *in, int nFrames);

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
	double getPitch(double *in);
};
#endif
