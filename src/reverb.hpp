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
			float c1 = cb1.process(input);
			float c2 = cb2.process(input);
			float c3 = cb3.process(input);
			float c4 = cb4.process(input);
			return (input * (1 - m_DryWet) + m_DryWet * (c1 + c2 + c3 + c4) / 4);
		}
	};/*Schroeder*/
	
class Moorer{
private:
std::array<noi::Filter::Comb, 6> m_comb = {
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02)
};
noi::Filter::Allpass m_allpass{ 0.006 };
		float m_DryWet{1.f},
				m_RT60{5.f}, 
				m_variation{0.5}, 
				m_combTime{0.01};
		bool m_freeze {false};

public:
	inline Moorer(){}
	inline void setTime(float rt60, float variation) {
		//if (m_RT60 == rt60 && variation == m_variation) { return; }
		m_RT60 = rt60;
		m_variation = variation;
		variation += 1.f;
		m_allpass.setGain(rt60);
		for (auto& comb:m_comb){
			comb.setGain(rt60);
			rt60*=variation;
		}

	}
	inline void setDryWet(float DryWet) { m_DryWet = DryWet; }
	inline float getDryWet(){return m_DryWet;}
	inline void resizeComb(float time, float variation) {
		//if (m_combTime == time && m_variation==variation) { return; }
		m_combTime = time;
		m_variation = variation;
		variation += 1.f;
		for (auto& comb : m_comb){
			comb.resize(time);
			time *= variation;
		}			
	}

	inline void setFreeze(bool statut){
		for (auto& comb : m_comb){
			comb.setFreeze(statut);
		}
		m_freeze = statut;
	}
	inline float process(float input) {
		float comb_sum = 0;
		//process combs
		if (m_freeze){
		for (auto& comb : m_comb){
			comb_sum +=comb.processFreezed();
		}}
		else{
		for (auto& comb : m_comb){
			comb_sum +=comb.process(input);
		}}
		comb_sum /= 6.f;
		//process allpass
		float output = (m_allpass.process(comb_sum)*m_DryWet);
		output += (input * (1.f-m_DryWet));
		//+ (input * (1.f - m_DryWet)); 
		return output;
	}
	inline float * processStereo(float inputL, float inputR){
		float inputs[2] = {inputL, inputR};
		return inputs; 
	}
		
};
class StereoMoorer{
private:
std::array<noi::Filter::Comb, 6> m_combL = {
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02)
};
noi::Filter::Allpass m_allpassL{ 0.006 };
std::array<noi::Filter::Comb, 6> m_combR = {
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02)
};
noi::Filter::Allpass m_allpassR{ 0.006 };

public:
inline float * processStereo(float inputL, float inputR){

}
inline float processMono(float input){

	return output;
}
};
}/*Reverb*/
}/*noi*/