#include "filter.hpp"
#include <vector>
#include <array>
#include <math.h>

namespace noi {

namespace Reverb {

	class Schroeder{
	private:
		float m_rt60 = 4.;
		float m_DryWet = 1.0;
		noi::Filter::Allpass ap1;
		noi::Filter::Allpass ap2;
		noi::Filter::Comb cb1;
		noi::Filter::Comb cb2;
		noi::Filter::Comb cb3;
		noi::Filter::Comb cb4;
		
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
void SetSampleRate(float sample_rate);
std::array<float, 2> processStereo(std::array<float, 2> inputs);

private:
std::array<std::array<noi::Filter::Comb, 6>, 2> m_combs = {{{
			noi::Filter::Comb(),
			noi::Filter::Comb(),
			noi::Filter::Comb(),
			noi::Filter::Comb(),
			noi::Filter::Comb(),
			noi::Filter::Comb()},
			{noi::Filter::Comb(),
			noi::Filter::Comb(),
			noi::Filter::Comb(),
			noi::Filter::Comb(),
			noi::Filter::Comb(),
			noi::Filter::Comb()}}
};
	std::array<noi::Filter::Allpass, 2> m_allpasses{
			noi::Filter::Allpass(),
			noi::Filter::Allpass()};
	noi::Reverb::StereoMoorer::Parameters m_params = {false, 1.0, 0.01, 0.1, 10.0};
	std::array<std::array<float, 6>, 2> m_combs_status = {{{0., 0., 0., 0., 0., 0.},{0., 0., 0., 0., 0., 0.}}} ;
	std::array<float, 6> m_pan_coefs = {{0., 0., 0., 0., 0., 0.}};
};/*StereoMoorer*/
}/*Reverb*/
}/*noi*/