#include "CjvxDigitalFilter.h"

#include <DspFilters/Dsp.h>
#include <vector>

// =============================================================================

struct BiquadCoeffs
{
	double b0, b1, b2;
	double a0, a1, a2;
};

// =============================================================================

class IirDesigner
{
public:
	virtual ~IirDesigner() = default;

	virtual std::vector<BiquadCoeffs> coefficients() = 0;

	virtual std::complex<double> response(
		double normalizedFrequency) const = 0;
};

// =============================================================================

template<class FilterType>
class IirDesigner_order_fs_fc_df_rip : public IirDesigner
{
public:
	void setup(
		int order,
		double fs,
		double fc,
		double deltaf,
		double rippleDb)
	{
		filter_.setup(
			order,
			fs,
			fc,
			deltaf,
			rippleDb);
	}

	std::complex<double> response(
		double f) const override
	{
		return filter_.response(f);
	}

	virtual std::vector<BiquadCoeffs> coefficients()
	{
		std::vector<BiquadCoeffs> coeffs;
		Dsp::Cascade& cascade = filter_;
		const int numStages = cascade.getNumStages();
		coeffs.reserve(numStages);

		for (int k = 0; k < numStages; ++k)
		{
			const Dsp::BiquadBase& s = cascade[k];

			coeffs.push_back({
				s.getB0(),
				s.getB1(),
				s.getB2(),
				s.getA0(),
				s.getA1(),
				s.getA2()
				});
		}
		return coeffs;
	};
private:
	Dsp::SimpleFilter<FilterType> filter_;
};

// =============================================================================

template<class FilterType>
class IirDesigner_order_fs_fc_df_rip_stopdb : public IirDesigner
{
public:
	void setup(
		int order,
		double fs,
		double fc,
		double deltaf,
		double rippleDb,
		double stopDb)
	{
		filter_.setup(
			order,
			fs,
			fc,
			deltaf,
			rippleDb,
			stopDb);
	}

	std::complex<double> response(
		double f) const override
	{
		return filter_.response(f);
	}

	virtual std::vector<BiquadCoeffs> coefficients()
	{
		std::vector<BiquadCoeffs> coeffs;
		Dsp::Cascade& cascade = filter_;
		const int numStages = cascade.getNumStages();
		coeffs.reserve(numStages);

		for (int k = 0; k < numStages; ++k)
		{
			const Dsp::BiquadBase& s = cascade[k];

			coeffs.push_back({
				s.getB0(),
				s.getB1(),
				s.getB2(),
				s.getA0(),
				s.getA1(),
				s.getA2()
				});
		}
		return coeffs;
	};
private:
	Dsp::SimpleFilter<FilterType> filter_;
};

// =============================================================================

template<class FilterType>
class IirDesigner_order_fs_fc_df_stopdb : public IirDesigner
{
public:
	void setup(
		int order,
		double fs,
		double fc,
		double deltaf,
		double stopDb)
	{
		filter_.setup(
			order,
			fs,
			fc,
			deltaf,
			stopDb);
	}

	std::complex<double> response(
		double f) const override
	{
		return filter_.response(f);
	}

	virtual std::vector<BiquadCoeffs> coefficients()
	{
		std::vector<BiquadCoeffs> coeffs;
		Dsp::Cascade& cascade = filter_;
		const int numStages = cascade.getNumStages();
		coeffs.reserve(numStages);

		for (int k = 0; k < numStages; ++k)
		{
			const Dsp::BiquadBase& s = cascade[k];

			coeffs.push_back({
				s.getB0(),
				s.getB1(),
				s.getB2(),
				s.getA0(),
				s.getA1(),
				s.getA2()
				});
		}
		return coeffs;
	};
private:
	Dsp::SimpleFilter<FilterType> filter_;
};

// =============================================================================

template<class FilterType>
class IirDesigner_order_fs_fc_rip : public IirDesigner
{
public:
	void setup(
		int order,
		double fs,
		double fc,
		double rippleDb)
	{
		filter_.setup(
			order,
			fs,
			fc,
			rippleDb);
	}

	std::complex<double> response(
		double f) const override
	{
		return filter_.response(f);
	}

	virtual std::vector<BiquadCoeffs> coefficients()
	{
		std::vector<BiquadCoeffs> coeffs;
		Dsp::Cascade& cascade = filter_;
		const int numStages = cascade.getNumStages();
		coeffs.reserve(numStages);

		for (int k = 0; k < numStages; ++k)
		{
			const Dsp::BiquadBase& s = cascade[k];

			coeffs.push_back({
				s.getB0(),
				s.getB1(),
				s.getB2(),
				s.getA0(),
				s.getA1(),
				s.getA2()
				});
		}
		return coeffs;
	};
private:
	Dsp::SimpleFilter<FilterType> filter_;
};

// =============================================================================

template<class FilterType>
class IirDesigner_order_fs_fc_rip_stopdb : public IirDesigner
{
public:
	void setup(
		int order,
		double fs,
		double fc,
		double rippleDb,
		double stopDb)
	{
		filter_.setup(
			order,
			fs,
			fc,
			rippleDb,
			stopDb);
	}

	std::complex<double> response(
		double f) const override
	{
		return filter_.response(f);
	}

	virtual std::vector<BiquadCoeffs> coefficients()
	{
		std::vector<BiquadCoeffs> coeffs;
		Dsp::Cascade& cascade = filter_;
		const int numStages = cascade.getNumStages();
		coeffs.reserve(numStages);

		for (int k = 0; k < numStages; ++k)
		{
			const Dsp::BiquadBase& s = cascade[k];

			coeffs.push_back({
				s.getB0(),
				s.getB1(),
				s.getB2(),
				s.getA0(),
				s.getA1(),
				s.getA2()
				});
		}
		return coeffs;
	};
private:
	Dsp::SimpleFilter<FilterType> filter_;
};

// =============================================================================

template<class FilterType>
class IirDesigner_order_fs_fc_stopdb : public IirDesigner
{
public:
	void setup(
		int order,
		double fs,
		double fc,
		double stopDb)
	{
		filter_.setup(
			order,
			fs,
			fc,
			stopDb);
	}

	std::complex<double> response(
		double f) const override
	{
		return filter_.response(f);
	}

	virtual std::vector<BiquadCoeffs> coefficients()
	{
		std::vector<BiquadCoeffs> coeffs;
		Dsp::Cascade& cascade = filter_;
		const int numStages = cascade.getNumStages();
		coeffs.reserve(numStages);

		for (int k = 0; k < numStages; ++k)
		{
			const Dsp::BiquadBase& s = cascade[k];

			coeffs.push_back({
				s.getB0(),
				s.getB1(),
				s.getB2(),
				s.getA0(),
				s.getA1(),
				s.getA2()
				});
		}
		return coeffs;
	};
private:
	Dsp::SimpleFilter<FilterType> filter_;
};

// =============================================================================

template<class FilterType>
class IirDesigner_order_fs_fc_df : public IirDesigner
{
public:
	void setup(
		int order,
		double fs,
		double fc,
		double deltaf)
	{
		filter_.setup(
			order,
			fs,
			fc,
			deltaf);
	}

	std::complex<double> response(
		double f) const override
	{
		return filter_.response(f);
	}

	virtual std::vector<BiquadCoeffs> coefficients()
	{
		std::vector<BiquadCoeffs> coeffs;
		Dsp::Cascade& cascade = filter_;
		const int numStages = cascade.getNumStages();
		coeffs.reserve(numStages);

		for (int k = 0; k < numStages; ++k)
		{
			const Dsp::BiquadBase& s = cascade[k];

			coeffs.push_back({
				s.getB0(),
				s.getB1(),
				s.getB2(),
				s.getA0(),
				s.getA1(),
				s.getA2()
				});
		}
		return coeffs;
	};
private:
	Dsp::SimpleFilter<FilterType> filter_;
};

// =============================================================================

template<class FilterType>
class IirDesigner_order_fs_fc : public IirDesigner
{
public:
	void setup(
		int order,
		double fs,
		double fc)
	{
		filter_.setup(
			order,
			fs,
			fc);
	}

	std::complex<double> response(
		double f) const override
	{
		return filter_.response(f);
	}

	virtual std::vector<BiquadCoeffs> coefficients()
	{
		std::vector<BiquadCoeffs> coeffs;
		Dsp::Cascade& cascade = filter_;
		const int numStages = cascade.getNumStages();
		coeffs.reserve(numStages);

		for (int k = 0; k < numStages; ++k)
		{
			const Dsp::BiquadBase& s = cascade[k];

			coeffs.push_back({
				s.getB0(),
				s.getB1(),
				s.getB2(),
				s.getA0(),
				s.getA1(),
				s.getA2()
				});
		}
		return coeffs;
	};
private:
	Dsp::SimpleFilter<FilterType> filter_;
};

// =============================================================================
// =============================================================================
// =============================================================================

#define maxOrder 16

jvxErrorType
CjvxDigitalFilter::initialize(const std::string& tokenTech, const std::string& tokenDescr, jvxSize order, jvxSize fs, jvxData fc, jvxData rippleDb, jvxData stopDb, jvxData freqWidth, jvxBool withG)
{
	// ===========================================================================
	std::unique_ptr<IirDesigner> filter;
	// ===========================================================================
	
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (state == JVX_STATE_NONE)
	{
		errMessage.clear();

		// Default: successful
		res = JVX_NO_ERROR;

		if (tokenTech == "Elliptic")
		{

			if (tokenDescr == "HighPass")
			{
				auto filterSpec = std::make_unique<IirDesigner_order_fs_fc_rip_stopdb<Dsp::Elliptic::Design::HighPass<maxOrder> > >();
				filterSpec->setup(order, fs, fc, rippleDb, stopDb);
				filter = std::move(filterSpec);
			}
			else if (tokenDescr == "LowPass")
			{
				auto filterSpec = std::make_unique<IirDesigner_order_fs_fc_rip_stopdb<Dsp::Elliptic::Design::LowPass<maxOrder> > >();
				filterSpec->setup(order, fs, fc, rippleDb, stopDb);
				filter = std::move(filterSpec);
			}
			else if (tokenDescr == "BandPass")
			{
				auto filterSpec = std::make_unique<IirDesigner_order_fs_fc_df_rip_stopdb<Dsp::Elliptic::Design::BandPass<maxOrder> > >();
				filterSpec->setup(order, fs, fc, freqWidth, rippleDb, stopDb);
				filter = std::move(filterSpec);
			}
			else if (tokenDescr == "BandStop")
			{
				auto filterSpec = std::make_unique<IirDesigner_order_fs_fc_df_rip_stopdb<Dsp::Elliptic::Design::BandStop<maxOrder> > >();
				filterSpec->setup(order, fs, fc, freqWidth, rippleDb, stopDb);
				filter = std::move(filterSpec);
			}


		} // if(tokenTech == "Elliptic")		

		// ============================================================================================================

		else if (tokenTech == "ChebyshevI")
		{

			if (tokenDescr == "HighPass")
			{
				auto filterSpec = std::make_unique<IirDesigner_order_fs_fc_rip<Dsp::ChebyshevI::Design::HighPass<maxOrder> > >();
				filterSpec->setup(order, fs, fc, rippleDb);
				filter = std::move(filterSpec);
			}
			else if (tokenDescr == "LowPass")
			{
				auto filterSpec = std::make_unique<IirDesigner_order_fs_fc_rip<Dsp::ChebyshevI::Design::LowPass<maxOrder> > >();
				filterSpec->setup(order, fs, fc, rippleDb);
				filter = std::move(filterSpec);
			}
			else if (tokenDescr == "BandPass")
			{
				auto filterSpec = std::make_unique<IirDesigner_order_fs_fc_df_rip<Dsp::ChebyshevI::Design::BandPass<maxOrder> > >();
				filterSpec->setup(order, fs, fc, freqWidth, rippleDb);
				filter = std::move(filterSpec);
			}
			else if (tokenDescr == "BandStop")
			{
				auto filterSpec = std::make_unique<IirDesigner_order_fs_fc_df_rip<Dsp::ChebyshevI::Design::BandStop<maxOrder> > >();
				filterSpec->setup(order, fs, fc, freqWidth, rippleDb);
				filter = std::move(filterSpec);
			}


		} // if(tokenTech == "ChebyshevI")	

		// ============================================================================================================

		else if (tokenTech == "ChebyshevII")
		{

			if (tokenDescr == "HighPass")
			{
				auto filterSpec = std::make_unique<IirDesigner_order_fs_fc_stopdb<Dsp::ChebyshevII::Design::HighPass<maxOrder> > >();
				filterSpec->setup(order, fs, fc, stopDb);
				filter = std::move(filterSpec);
			}
			else if (tokenDescr == "LowPass")
			{
				auto filterSpec = std::make_unique<IirDesigner_order_fs_fc_stopdb<Dsp::ChebyshevII::Design::LowPass<maxOrder> > >();
				filterSpec->setup(order, fs, fc, stopDb);
				filter = std::move(filterSpec);
			}
			else if (tokenDescr == "BandPass")
			{
				auto filterSpec = std::make_unique<IirDesigner_order_fs_fc_df_stopdb<Dsp::ChebyshevII::Design::BandPass<maxOrder> > >();
				filterSpec->setup(order, fs, fc, freqWidth, stopDb);
				filter = std::move(filterSpec);
			}
			else if (tokenDescr == "BandStop")
			{
				auto filterSpec = std::make_unique<IirDesigner_order_fs_fc_df_stopdb<Dsp::ChebyshevII::Design::BandStop<maxOrder> > >();
				filterSpec->setup(order, fs, fc, freqWidth, stopDb);
				filter = std::move(filterSpec);
			}
		} // if(tokenTech == "ChebyshevII")		

		// ============================================================================================================

		else if (tokenTech == "Butterworth")
		{
			if (tokenDescr == "HighPass")
			{
				auto filterSpec = std::make_unique<IirDesigner_order_fs_fc<Dsp::Butterworth::Design::HighPass<maxOrder> > >();
				filterSpec->setup(order, fs, fc);
				filter = std::move(filterSpec);
			}
			else if (tokenDescr == "LowPass")
			{
				auto filterSpec = std::make_unique<IirDesigner_order_fs_fc<Dsp::Butterworth::Design::LowPass<maxOrder> > >();
				filterSpec->setup(order, fs, fc);
				filter = std::move(filterSpec);
			}
			else if (tokenDescr == "BandPass")
			{
				auto filterSpec = std::make_unique<IirDesigner_order_fs_fc_df<Dsp::Butterworth::Design::BandPass<maxOrder> > >();
				filterSpec->setup(order, fs, fc, freqWidth);
				filter = std::move(filterSpec);
			}
			else if (tokenDescr == "BandStop")
			{
				auto filterSpec = std::make_unique<IirDesigner_order_fs_fc_df<Dsp::Butterworth::Design::BandStop<maxOrder> > >();
				filterSpec->setup(order, fs, fc, freqWidth);
				filter = std::move(filterSpec);
			}
		}

		// ============================================================================================================

		/*
		else if (tokenTech == "Bessel")
		{
			if (tokenDescr == "HighPass")
			{
				auto filterSpec = std::make_unique<IirDesigner_order_fs_fc<Dsp::Bessel::Design::HighPass<maxOrder> > >();
				filterSpec->setup(order, fs, fc);
				filter = std::move(filterSpec);
			}
			else if (tokenDescr == "LowPass")
			{
				auto filterSpec = std::make_unique<IirDesigner_order_fs_fc<Dsp::Bessel::Design::LowPass<maxOrder> > >();
				filterSpec->setup(order, fs, fc);
				filter = std::move(filterSpec);
			}
			else if (tokenDescr == "BandPass")
			{
				auto filterSpec = std::make_unique<IirDesigner_order_fs_fc_df<Dsp::Bessel::Design::BandPass<maxOrder> > >();
				filterSpec->setup(order, fs, fc, freqWidth);
				filter = std::move(filterSpec);
			}
			else if (tokenDescr == "BandStop")
			{
				auto filterSpec = std::make_unique<IirDesigner_order_fs_fc_df<Dsp::Bessel::Design::BandStop<maxOrder> > >();
				filterSpec->setup(order, fs, fc, freqWidth);
				filter = std::move(filterSpec);
			}
		}
		*/

		// ============================================================================================================

		/*
		else if (tokenTech == "Legendre")
		{
			if (tokenDescr == "HighPass")
			{
				auto filterSpec = std::make_unique<IirDesignerImplLowHigh<Dsp::Legendre::Design::HighPass<maxOrder> > >();
				filterSpec->setup(order, fs, fc, rippleDb, stopDb);
				filter = std::move(filterSpec);
			}
			else if (tokenDescr == "LowPass")
			{
				auto filterSpec = std::make_unique<IirDesignerImplLowHigh<Dsp::Legendre::Design::LowPass<maxOrder> > >();
				filterSpec->setup(order, fs, fc, rippleDb, stopDb);
				filter = std::move(filterSpec);
			}
			else if (tokenDescr == "BandPass")
			{
				auto filterSpec = std::make_unique<IirDesignerImplBand<Dsp::Legendre::Design::BandPass<maxOrder> > >();
				filterSpec->setup(order, fs, fc, freqWidth, rippleDb, stopDb);
				filter = std::move(filterSpec);
			}
			else if (tokenDescr == "BandStop")
			{
				auto filterSpec = std::make_unique<IirDesignerImplBand<Dsp::Legendre::Design::BandStop<maxOrder> > >();
				filterSpec->setup(order, fs, fc, freqWidth, rippleDb, stopDb);
				filter = std::move(filterSpec);
			}
		}
		*/

		// ============================================================================================================

		else
		{
			res = JVX_ERROR_INVALID_SETTING;
			errMessage = "Requested filter technology <" + tokenTech + "> is not supported.";
		}

		if (res == JVX_NO_ERROR)
		{
			if (filter == nullptr)
			{
				res = JVX_ERROR_INTERNAL;
				errMessage = "No filter coefficients available.";
			}
		}

		if (res == JVX_NO_ERROR)
		{
			std::vector<BiquadCoeffs> coeffsRet = filter->coefficients();

			jvxSize i;
			N = coeffsRet.size();
			M0 = 3;
			M = 2 * M0;

			filterCoefficientsSos.clear();

			for (i = 0; i < N; i++)
			{
				std::vector<jvxData> lstSosB012A012;
				lstSosB012A012.resize(M);
				if (withG)
				{
					jvxData g = 1.0;
					if (i == 0)
					{
						g = coeffsRet[i].b0;
						filterCoefficientsG = g;
					}
					lstSosB012A012[0] = coeffsRet[i].b0 / g;
					lstSosB012A012[1] = coeffsRet[i].b1 / g;
					lstSosB012A012[2] = coeffsRet[i].b2 / g;
				}
				else
				{
					if (i == 0)
					{
						filterCoefficientsG = 1;
					}
					lstSosB012A012[0] = coeffsRet[i].b0;
					lstSosB012A012[1] = coeffsRet[i].b1;
					lstSosB012A012[2] = coeffsRet[i].b2;
				}
				lstSosB012A012[3] = coeffsRet[i].a0;
				lstSosB012A012[4] = coeffsRet[i].a1;
				lstSosB012A012[5] = coeffsRet[i].a2;

				filterCoefficientsSos.push_back(lstSosB012A012);
			}
			// res = JVX_NO_ERROR;
			state = JVX_STATE_INIT;
		}
	}

	return res;
}


jvxErrorType
CjvxDigitalFilter::terminate()
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (state == JVX_STATE_INIT)
	{
		filterCoefficientsSos.clear();
		filterCoefficientsG = 1;
		state = JVX_STATE_NONE;
	}
	return res;
}

// ============================================================================================================

jvxErrorType 
CjvxDigitalFilter::prepare(jvxSize nChans, jvxSize bsize)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (state == JVX_STATE_INIT)
	{
		//
		// jvx_circbuffer_allocate(
		//				&hdl, 2 /* <- order 2 for every section */ , 
		//				<JVX_DIFFARRAY_HP_NUM_SECTIONS>number sections>, 
		//				hdl->nChannelsIn);
		// 

		jvxSize i;
		jvx_circbuffer_allocate(&hdlFilt, 2, N, nChans);

		JVX_SAFE_ALLOCATE_FIELD_CPP_Z(coeffsFwd, jvxData, 3 * N);
		JVX_SAFE_ALLOCATE_FIELD_CPP_Z(coeffsBwd, jvxData, 3 * N);
		JVX_SAFE_ALLOCATE_FIELD_CPP_Z(gains, jvxData, N);

		jvxData* fwdWrite = coeffsFwd;
		jvxData* bwdWrite = coeffsBwd;
		jvxData* gainWrite = gains;

		for (i = 0; i < N; i++)
		{
			*fwdWrite++ = filterCoefficientsSos[i][0];
			*fwdWrite++ = filterCoefficientsSos[i][1];
			*fwdWrite++ = filterCoefficientsSos[i][2];

			*bwdWrite++ = filterCoefficientsSos[i][3];
			*bwdWrite++ = filterCoefficientsSos[i][4];
			*bwdWrite++ = filterCoefficientsSos[i][5];

			*gainWrite++ = 1.0;
		}
		gains[0] = filterCoefficientsG;

		bsizeProcessing = bsize;

		state = JVX_STATE_PROCESSING;
		res = JVX_NO_ERROR;

	}
	return res;
}

jvxErrorType 
CjvxDigitalFilter::postprocess()
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (state == JVX_STATE_PROCESSING)
	{

		JVX_SAFE_DELETE_FIELD(coeffsFwd);
		JVX_SAFE_DELETE_FIELD(coeffsBwd);
		JVX_SAFE_DELETE_FIELD(gains);

		jvx_circbuffer_deallocate(hdlFilt);
		hdlFilt = nullptr;

		state = JVX_STATE_INIT;
		res = JVX_NO_ERROR;
	}
	return res;
}

// =============================================================================================
// =============================================================================================

jvxErrorType
CjvxDigitalFilter::process(const jvxData** in, jvxData** out, jvxSize nChans, jvxSize bsize)
{
	jvxErrorType res = JVX_ERROR_WRONG_STATE;
	if (state == JVX_STATE_PROCESSING)
	{
		//			hdl,
		//			<coeffs numerator, 3 for each section>,
		//			<coeffs denominator, 3 for each section>,
		//			<gan factors, 1 per section>,
		//			<shift factor>,
		//			output,
		//			framesize);
		// jvx_circbuffer_iir_sos1can_1io(
		//			hdl,
		//			<coeffs numerator, 3 for each section>,
		//			<coeffs denominator, 3 for each section>,
		//			<gan factors, 1 per section>,
		//			<shift factor>,
		//			output,
		//			framesize);
		//

		res = jvx_circbuffer_iir_sos1can_2io(hdlFilt, coeffsFwd, coeffsBwd, gains, 0, in, out, bsize);
	}
	return res;
}
