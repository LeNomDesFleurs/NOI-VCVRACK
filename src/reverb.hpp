#include "filter.hpp"
#include <vector>
#include <array>

namespace noi {

namespace Reverb {

	class Schroeder{
	private:
		float m_rt60;
		float m_DryWet;
		float m_step;
		noi::Filter::Allpass ap1{0.0017};
		noi::Filter::Allpass ap2{ 0.005 };
		noi::Filter::Comb cb1{ 0.0297 };
		noi::Filter::Comb cb2{ 0.0371 };
		noi::Filter::Comb cb3{ 0.0411 };
		noi::Filter::Comb cb4{ 0.0437 };
		
	public:
		Schroeder();
		void setStep(float speed);
		void setTime(float rt60);
		void setDryWet(float DryWet);
		float process(float input);
	}; /*Schroeder*/

class StereoMoorer{
public:
/// @brief Parameters of a stereoMoorer Reverb
/// @param freeze 
/// @param drywet from 0 to 1
/// @param comb_time
/// @param variation
/// @param rt60
struct Parameters{
	bool freeze;
	float dry_wet,
		comb_time,
		variation,
		rt60;
};

StereoMoorer(noi::Reverb::StereoMoorer::Parameters params);
void updateParameters(noi::Reverb::StereoMoorer::Parameters params);
void setPan();
void setTime();
void setFreeze();
void resizeComb();
void processCombs(std::array<float, 2> inputs);
void SetSampleRate(int sample_rate);
std::array<float, 2> processStereo(std::array<float, 2> inputs);

private:
std::array<std::array<noi::Filter::Comb, 6>, 2> m_combs = {{{
			noi::Filter::Comb(2.f),
			noi::Filter::Comb(2.f),
			noi::Filter::Comb(2.f),
			noi::Filter::Comb(2.f),
			noi::Filter::Comb(2.f),
			noi::Filter::Comb(2.f)},
			{noi::Filter::Comb(2.f),
			noi::Filter::Comb(2.f),
			noi::Filter::Comb(2.f),
			noi::Filter::Comb(2.f),
			noi::Filter::Comb(2.f),
			noi::Filter::Comb(2.f)}}
};
	std::array<noi::Filter::Allpass, 2> m_allpasses{
			noi::Filter::Allpass(0.006),
			noi::Filter::Allpass(0.006)};
	noi::Reverb::StereoMoorer::Parameters m_params;
	std::array<std::array<float, 6>, 2> m_combs_status;
	std::array<float, 6> m_pan_coefs;
};/*StereoMoorer*/
}/*Reverb*/
}/*noi*/