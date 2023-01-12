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

		class Moorer {
		private:
			noi::Filter::Comb m_comb[6]{ 0.0050 , 0.0056, 0.0061, 0.0068, 0.0072, 0.0078 };
			noi::Filter::Allpass ap1{ 0.006 };
			float m_DryWet{1.f},
				m_RT60{5.f}, 
				m_disp{0.5}, 
				m_combTime{0.01};
		public:
		inline Moorer(){}
			inline void setTime(float rt60, float variation) {
				if (m_RT60 == rt60 && variation == m_disp) { return; }
				m_RT60 = rt60;
				m_disp = variation;
				ap1.setGain(rt60);
				m_comb[0].setGain(rt60+variation);
				m_comb[1].setGain(rack::math::clamp(rt60-variation, 0.F, 100.f));
				m_comb[2].setGain(rt60+(variation/2.f));
				m_comb[3].setGain(rack::math::clamp(rt60-(variation/2.f), 0.F, 100.f));
				m_comb[4].setGain(rt60);
				m_comb[5].setGain(rt60);
			}
			inline void setDryWet(float DryWet) { m_DryWet = DryWet; }
			inline float getDryWet(){return m_DryWet;}
			inline void resizeComb(float time, float variation) {
				if (m_combTime == time && m_disp==variation) { return; }
				m_combTime = time;
				m_disp = variation;
				variation /= 20.f;
			
				m_comb[0].resize(rack::math::clamp(time + variation, 0.006f, 1.f ));
				time *= 1.12;
				m_comb[1].resize(rack::math::clamp((time - variation), 0.006f, 1.f));
				time *= 1.12;
				m_comb[2].resize(rack::math::clamp(time + (variation/2.f), 0.006f, 1.f));
				time *= 1.12;
				m_comb[3].resize(rack::math::clamp((time - (variation/2.f)), 0.006f, 1.f));
				time *= 1.12;
				m_comb[4].resize(rack::math::clamp(time, 0.006f, 1.f));
				time *= 1.12;
				m_comb[5].resize(rack::math::clamp(time, 0.006f, 1.f));
			}

			inline void setFreeze(bool statut){
				for (auto i : m_comb){
					i.setFreeze(statut);
				}
			}
			inline float process(float input) {
				float comb_sum = 0;
				//process combs
				for (auto i : m_comb){comb_sum += i.process(input);}
				comb_sum /= 6;
				//process allpass
				float reverb_out = ap1.process(comb_sum);
				float out = (reverb_out * m_DryWet);
				//+ (input * (1.f - m_DryWet)); 
				return out;
			}

		};/*Moorer*/
class MoorerTest{
private:
std::array<noi::Filter::Comb, 6> m_comb = {
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02),
			noi::Filter::Comb(0.02)
};
noi::Filter::Allpass ap1{ 0.006 };
		float m_DryWet{1.f},
				m_RT60{5.f}, 
				m_disp{0.5}, 
				m_combTime{0.01};
public:
inline MoorerTest(){}
	inline void setTime(float rt60, float variation) {
				if (m_RT60 == rt60 && variation == m_disp) { return; }
				m_RT60 = rt60;
				m_disp = variation;
			ap1.setGain(rt60);
			for (auto& comb:m_comb){
				comb.setGain(rt60);
				rt60*=1.1;
			}

			}
			inline void setDryWet(float DryWet) { m_DryWet = DryWet; }
			inline float getDryWet(){return m_DryWet;}
			inline void resizeComb(float time, float variation) {
				if (m_combTime == time && m_disp==variation) { return; }
				m_combTime = time;
				m_disp = variation;
				variation /= 20.f;
				for (auto& comb : m_comb){
					comb.resize(time);
					time *= 1.1;
				}				
			}

			inline void setFreeze(bool statut){
				for (auto& comb : m_comb){
					comb.setFreeze(statut);
				}
			}
			inline float process(float input) {
				float comb_sum = 0;
				//process combs
				for (auto& comb : m_comb){
					comb_sum +=comb.process(input);
				}
				comb_sum /=6.f;
				//process allpass
				float output = ap1.process(comb_sum)*m_DryWet;
				output += input * (1.f-m_DryWet);
				//+ (input * (1.f - m_DryWet)); 
				return output;
			}
		
};
	}/*Reverb*/
}/*noi*/