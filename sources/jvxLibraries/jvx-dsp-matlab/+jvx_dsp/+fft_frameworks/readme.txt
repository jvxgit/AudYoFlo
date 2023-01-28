Version1 = IND FFT Framework with overlap add: 
	- sqrt Hann window at analysis, 
	- sqrt Hann window at synthesis, 
	- Hann window must be computed outside core function.
Version2 = Linear Convolution, overlap-add:
	- Hann Window at analysis
	- no window at synthesis
	- circular convolution may lead to artefacts if not properly designed
	- Hann window must be computed outside core function.
Version3 = Linear Convolution, overlap-save:
	- No window possible in case of overlap-save
	- circular convolution may lead to artefacts if not properly designed

HK, July 2012
