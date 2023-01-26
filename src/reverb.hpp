#include "filter.hpp"
#include <vector>
#include <array>

namespace noi {

namespace Reverb {

	class Schroeder{
	private:
		float m_RT60;
		float m_DryWet;
		float m_b0;
		noi::Filter::Allpass ap1{ 0.0017 };
		noi::Filter::Allpass ap2{ 0.005 };
		noi::Filter::Comb cb1{ 0.0297 };
		noi::Filter::Comb cb2{ 0.0371 };
		noi::Filter::Comb cb3{ 0.0411 };
		noi::Filter::Comb cb4{ 0.0437 };
		
	public:
		Schroeder(){}
		void setParam(float rt60) {
			if (m_RT60 == rt60) { return; }
			m_RT60 = rt60;
			ap1.setGain(rt60);
			ap2.setGain(rt60);
			cb1.setGain(rt60);
			cb2.setGain(rt60);
			cb3.setGain(rt60);
			cb4.setGain(rt60);
		}
		void setDryWet(float DryWet) { m_DryWet = DryWet; }
		float process(float input) {
			m_b0 = input;
			m_b0 = ap1.process(m_b0);
			m_b0 = ap2.process(m_b0);
			float c1 = cb1.process(m_b0);
			float c2 = cb2.process(m_b0);
			float c3 = cb3.process(m_b0);
			float c4 = cb4.process(m_b0);
			float comb_sum = (c1+c2+c3+c4)/4.f;
			return noi::Outils::dryWet(input, comb_sum, m_DryWet);
		}
	};/*Schroeder*/
class StereoMoorer{
public:
struct Parameters{
	bool freeze;
	float dry_wet,
		comb_time,
		variation,
		rt60;
};
private:
std::array<std::array<noi::Filter::Comb, 6>, 2> m_combs = {{{
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02)},

			{noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02)}}
};
std::array<noi::Filter::Allpass, 2> m_allpasses{
			noi::Filter::Allpass(0.006),
			noi::Filter::Allpass(0.006)};
noi::Reverb::StereoMoorer::Parameters m_params;
std::array<std::array<float, 6>, 2> m_combs_status;
std::array<float, 6> m_pan_coefs;
public:
StereoMoorer(noi::Reverb::StereoMoorer::Parameters params){
	updateParameters(params);
};
	inline void updateParameters(noi::Reverb::StereoMoorer::Parameters params){
	m_params = params;
	setTime();
	setFreeze();
	resizeComb();
	setPan();
	}
	inline void setPan(){
		float variation = m_params.variation;
		for (int i = 2; i<6; i++){
			m_pan_coefs[i] = variation;
			variation *= 2;
		}
	}
	inline void setTime() {
		float rt60 = m_params.rt60;
		float variation = m_params.variation + 1.f;
		for(int i=0; i<2; i++){
		m_allpasses[i].setGain(rt60);
		for (auto& comb:m_combs[i]){
			comb.setGain(rt60);
			rt60*=variation;
		}}
	}
	inline void setFreeze(){
		for (int i = 0; i<2; i++){
			for (auto& combs : m_combs[i]){
			combs.setFreeze(m_params.freeze);}
		}
	}
	inline void resizeComb() {
		float variation = m_params.variation + 1.f;
		for (int i = 0; i<2; i++){
			float time = m_params.comb_time;
		for (auto& comb : m_combs[i]){
			comb.resize(time);
			time *= variation;
			}
		}			
	}
	inline void processCombs(std::array<float, 2> inputs){
		for (int i = 0; i<2; i++){	
			//process combs
			if(m_params.freeze){
			for (int j = 0; j < 6 ; j++){
				m_combs_status[i][j] =m_combs[i][j].processFreezed();
			}}
			else{
			for (int j = 0; j < 6 ; j++){
				m_combs_status[i][j] =m_combs[i][j].process(inputs[i]);
			}}
		}
	}

inline std::array<float, 2> processStereo(std::array<float, 2> inputs){
	std::array<float, 2> outputs = {0, 0};
	processCombs(inputs);
	for (int i = 0; i<6; i++){
		//pan combs output, mix the two channels
		float in_left = m_combs_status[0][i];
		float in_right = m_combs_status[1][i];
		//out_left = in_left * pan + in_right * (1 - pan);
		m_combs_status[0][i] = noi::Outils::dryWet(in_left, in_right, m_pan_coefs[i]);
		m_combs_status[1][i] = noi::Outils::dryWet(in_left, in_right, (1.f - m_pan_coefs[i]));
	}
	for (int i = 0; i < 2; i++){
		float comb_sum = 0.f;
		for (auto j : m_combs_status[i]){
			comb_sum += j;
		}
		comb_sum /= 6.f;
		float to_allpass = noi::Outils::dryWet(inputs[i], comb_sum, m_params.dry_wet);
		outputs[i] = m_allpasses[i].process(to_allpass);
	}
	return outputs;
}

};/*StereoMoorer*/
}/*Reverb*/
}/*noi*/