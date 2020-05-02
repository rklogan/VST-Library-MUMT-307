#ifndef LINKWITZRILEY_H
#define LINKWITZRILEY_H

//global paramters
#define M_PI 3.14159265359
#define LN2 0.69314718056
#define SQRT2 1.41421356237

class LinkwitzRiley{
private:
	double Fs;					//sample rate
	double fc;					//cutoff fequency

	double w0, w2, w3, w4;		//paramters related to omega0
	double c1, c2, c3, c4;		//intermediate parameter
	double at, bt1, bt2;		//temp vars

	double b1, b2, b3, b4;			//b coFs
	double ha0, ha1, ha2, ha3, ha4;	//HP a coFs
	double la0, la1, la2, la3, la4; //LP a coFs

	//History
	double x4, x3, x2, x1;
	double h4, h3, h2, h1;
	double l4, l3, l2, l1;

public:
	LinkwitzRiley();
	LinkwitzRiley(double fs, double f);

	bool setFs(double fs);

	bool setFc(double f);

	double getFs(){ return Fs; }
	double getFc(){ return fc; }
	void getBs(double *b);
	void getLas(double *as);
	void getHas(double *as);
	void getCoFs(double *bs, double *las, double *has);
	double getOmega0(){return w0;}

	void updateParams(double fs, double f);

	void processBuffer(double *input, double *low, double *hi, int nFrames);
};
#endif