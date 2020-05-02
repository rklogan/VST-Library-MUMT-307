//include
#include <cmath>
#include <limits>

//globals

/**
 * Averages provides a set of averaging functions.
 * @author Ryan Khan Logan
 */

	/*************sign dependant averages*****************/
	/**
	 * mean computes the arithmetic mean of a set of samples
	 * @param input A buffer of doubles containing the set of samples
	 * @param nFrames The number of samples in the input buffer
	 * @return The arithmetic mean of the input buffer
	 */
	double mean(double *input, int nFrames){
		double acc = 0;
		for(int i = 0; i < nFrames; i++) acc += input[i];
		return acc/nFrames;
	}

	/**
	 * Computes the geometric mean of a set of samples. Because
	 * an nth root must be performed this function is quite
	 * computationally intensive.
	 * @param input The buffer of double containing the set of samples
	 * @param nFrames The number of samples in the buffer
	 * @see math.pow()
	 * @return The geometric mean of the input buffer
	 */
	double geometricMean(double *input, int nFrames){
		double acc = input[0];
		for(int i = 1; i < nFrames; i++) acc *= input[i];
		return pow(acc, 1/nFrames);
	}

	/**
	 * Computes the harmonic mean of a set of samples
	 * @param input The buffer of samples to be averaged
	 * @param nFrames The number of samples in the input buffer
	 * @return The harmonic mean of the input buffer
	 */
	double harmonicMean(double *input, int nFrames){
		double acc = 0;
		for(int i = 0; i < nFrames; i++) acc += 1/input[i];
		return nFrames / acc;
	}

	/**
	 * Computes the midpoint between the min and max value of 
	 * the input buffer
	 * @param input A buffer samples whose midpoint is to be computed
	 * @param nFrames The number of samples in the input buffer
	 * @return The midpoint of the input buffer
	 */
	double midPoint(double *input, int nFrames){
		double max = std::numeric_limits<double>::lowest();
		double min = std::numeric_limits<double>::max();
		for(int i = 0; i < nFrames; i++){
			if(input[i] < min) min = input[i];
			if(input[i] > max) max = input[i];
		}
		return (min + max) / 2;
	}

	/*************sign independant averages****************/

	/**
	 * Computes the peak value of the input buffer
	 * @param input The input buffer whose peak is to be determined
	 * @param nFrames the number of samples in the input buffer
	 * @return The peak value of the input buffer
	 */
	double peak(double *input, int nFrames){
		double max = std::numeric_limits<double>::lowest();
		for(int i = 0; i < nFrames; i++){
			if(input[i] > 0){
				if(input[i] > max) max = input[i];
			}
			else{
				if(-1 * input[i] > max) max = -1 * input[i];
			}
		}
		return max;
	}

	/**
	 * Computes the average of the absolute values of the
	 * samples in the input buffer
	 * @param input A buffer of doubles whose average is to be found
	 * @param nFrames The number of samples in the input buffer
	 * @return The absolute average of the input buffer
	 */
	double absAvg(double *input, int nFrames){
		double acc;
		for(int i = 0; i < nFrames; i++){
			if(input[i] >= 0) acc += input[i];
			else acc -= input[i];
		}
		return acc / nFrames;
	}

	/**
	 * Computes the RMS of the input buffer provided. This 
	 * function may be computationally intensive because a 
	 * sqare root must be taken.
	 * @param input A buffer of doubles whose RMS is to be computed
	 * @param nFrames The number of samples in the input buffer
	 * @see math.sqrt()
	 * @return The RMS of the input buffer
	 */
	double rms(double *input, int nFrames){
		double acc = 0;
		for(int i = 0; i < nFrames; i++){
			acc += input[i]*input[i];
		}
		return sqrt(acc/nFrames);
	}

	/**
	 * Computes the cubic root of the input buffer provided. 
	 * This function may be computationally intensive because
	 * a cube root must be taken.
	 * @param input The input buffer whose CMC is to be taken
	 * @param nFrames The number of samples in the input buffer
	 * @see math.cbrt
	 * @return The cubic mean of the input buffer
	 */
	/*double cubicMean(double *input, int nFrames){
		double acc = 0;
		for(int i = 0; i < nFrames; i++){
			acc += input[i]*input[i]*input[i];
		}
		return cbrt(acc/nFrames);
	}*/

	/**
	 * Computes the midpoint of the absolute values of the
	 * samples provided in the input buffer
	 * @param input An input buffer of doubles
	 * @param nFrames The number of samples in the input buffer
	 * @return the absolute midpoint of the input buffer
	 */
	double absMidPoint(double *input, int nFrames){
		double max = std::numeric_limits<double>::lowest();
		double min = std::numeric_limits<double>::max();
		for(int i = 0; i < nFrames; i++){
			if(input[i] >= 0){
				if(input[i] > max) max = input[i];
				if(input[i] < min) min = input[i];
			}
			else{
				double tmp = -1 * input[i];
				if(tmp > max) max = input[i];
				if(tmp < min) min = input[i];
			}
		}
		return (max+min)/2;
	}