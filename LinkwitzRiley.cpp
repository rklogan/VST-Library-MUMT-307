#include "LinkwitzRiley.h"

LinkwitzRiley::LinkwitzRiley(){
	updateParams(44100.0, 440.0);

	x4 = 0; x3 = 0; x2 = 0; x1 = 0;
	h4 = 0; h3 = 0; h2 = 0; h1 = 0;
	l4 = 0; l3 = 0; l2 = 0; l1 = 0;
}

LinkwitzRiley::LinkwitzRiley(double fs, double f){
	updateParams(fs, f);

	x4 = 0; x3 = 0; x2 = 0; x1 = 0;
	h4 = 0; h3 = 0; h2 = 0; h1 = 0;
	l4 = 0; l3 = 0; l2 = 0; l1 = 0;
}

bool LinkwitzRiley::setFs(double fs){
	if(fs <= 0) return false;
	else updateParams(fs, -1);
	return true;
}

bool LinkwitzRiley::setFc(double f){
	if(f <= 0) return false;
	else updateParams(-1, f);
	return true;
}

void LinkwitzRiley::getBs(double *b){
	b[0] = b1;
	b[1] = b2;
	b[2] = b3;
	b[3] = b4;
}
void LinkwitzRiley::getLas(double *as){
	as[0] = la0;
	as[1] = la1;
	as[2] = la2;
	as[3] = la3;
	as[4] = la4;
}
void LinkwitzRiley::getHas(double *as){
	as[0] = ha0;
	as[1] = ha1;
	as[2] = ha2;
	as[3] = ha3;
	as[4] = ha4;
}
void LinkwitzRiley::getCoFs(double *bs, double *las, double *has){
	getBs(bs);
	getLas(las);
	getHas(has);
}

void LinkwitzRiley::updateParams(double fs, double f){
	if (fs > 0) Fs = fs;
	if (f > 0) fc = f;

	//compute angular freqs
	w0 = 2 * M_PI * fc;
	w2 = w0 * w0;
	w3 = w2 * w0;
	w4 = w2 * w2;

	//computer intermediate params
	c1 = w0 / (tan(M_PI * fc / Fs));
	c2 = c1 * c1;
	c3 = c2 * c1;
	c4 = c2 * c2;

	//compute temps
	bt1 = SQRT2 * w3 * c1;
	bt2 = SQRT2 * w0 * c3;
	at = 4 * w2 * c2 + 2 * bt1  + c4 + 2 * bt2 + w4;

	//compute bs
	b1 = ( 4* ( w4 + bt1 - c4 - bt2 ) ) / at;
	b2 = ( 6*w4 - 8*w2*c2 + 6*c4 ) / at;
	b3 = ( 4 * ( w4 - bt1 + bt2 - c4 ) ) / at;
	b4 = ( c4 - 2*bt1 + w4 - 2*bt2 + 4*w2*c2 ) / at;

	//compute lo-pass coFs
	la0 = w4 / at;
	la1 = 4 * w4 / at;
	la2 = 6 * w4 / at;
	la3 = la1;
	la4 = la0;

	///compute hi-pass coFs
	ha0 = c4 / at;
	ha1 = -4 * c4 / at;
	ha2 = 6 * c4 / at;
	ha3 = ha1;
	ha4 = ha0;
}

void LinkwitzRiley::processBuffer(double *input, double *low, double *hi, int nFrames){
	for(int i = 0; i < nFrames; i++){
		double x0 = input[i];

		//process lowpass
		low[i] = la0*x0 + la1*x1 + la2*x2 +la3*x3 + la4*x4;
		low[i] = low[i] - b1*l1 - b2*l2 - b3*l3 - b4*l4;
		

		//process hipass
		hi[i]= ha0*x0 + ha1*x1 + ha2*x2 + ha3*x3 + ha4*x4;
		hi[i]= hi[i] - b1*h1 - b2*h2 - b3*h3 - b4*h4;
		
		//update histories
		x4 = x3; x3 = x2; x2 = x1; x1 = x0;
		l4 = l3; l3 = l2; l2 = l1; l1 = low[i];
		h4 = h3; h3 = h2; h2 = h1; h1 = hi[i];
	}
}
