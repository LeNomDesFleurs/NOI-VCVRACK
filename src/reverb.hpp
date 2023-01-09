#include "filter.hpp"


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
			float m_DryWet,
				m_RT60, 
				m_disp, 
				m_combTime;
		public:
			inline void setParam(float rt60, float disp) {
				if (m_RT60 == rt60 && disp == m_disp) { return; }
				m_RT60 = rt60;
				m_disp = disp;
				ap1.setGain(rt60);
				m_comb[0].setGain(rt60+disp);
				m_comb[1].setGain(noi::Outils::clip(rt60-disp, 0.F, 100.f));
				m_comb[2].setGain(rt60+(disp/2.f));
				m_comb[3].setGain(noi::Outils::clip(rt60-(disp/2.f), 0.F, 100.f));
				m_comb[4].setGain(rt60);
				m_comb[5].setGain(rt60);
			}
			inline void setDryWet(float DryWet) { m_DryWet = DryWet; }
			inline void repitchComb(float time, float variation){
				if (m_combTime == time && m_disp==variation) { return; }
				m_combTime = time;
				m_disp = variation;
				variation /= 20.f;


				m_comb[0].repitch(noi::Outils::clip(time + variation, 0.006f, 0.1f ));
				time *= 1.12;
				m_comb[1].repitch(noi::Outils::clip((time - variation), 0.006f, 0.1f));
				time *= 1.12;
				m_comb[2].repitch(noi::Outils::clip(time + (variation/2.f), 0.006f, 0.1f));
				time *= 1.12;
				m_comb[3].repitch(noi::Outils::clip((time - (variation/2.f)), 0.006f, 0.1f));
				time *= 1.12;
				m_comb[4].repitch(noi::Outils::clip(time, 0.006f, 0.1f));
				time *= 1.12;
				m_comb[5].repitch(noi::Outils::clip(time, 0.006f, 0.1f));
			}
			inline void resizeComb(float time, float variation) {
				if (m_combTime == time && m_disp==variation) { return; }
				m_combTime = time;
				m_disp = variation;
				variation /= 20.f;
			
				m_comb[0].resize(noi::Outils::clip(time + variation, 0.006f, 0.1f ));
				time *= 1.12;
				m_comb[1].resize(noi::Outils::clip((time - variation), 0.006f, 0.1f));
				time *= 1.12;
				m_comb[2].resize(noi::Outils::clip(time + (variation/2.f), 0.006f, 0.1f));
				time *= 1.12;
				m_comb[3].resize(noi::Outils::clip((time - (variation/2.f)), 0.006f, 0.1f));
				time *= 1.12;
				m_comb[4].resize(noi::Outils::clip(time, 0.006f, 0.1f));
				time *= 1.12;
				m_comb[5].resize(noi::Outils::clip(time, 0.006f, 0.1f));
			}

			inline float process(float input) {
				float sum = 0;
				//process combs
				for (auto i : m_comb){sum += i.process(input);}
				sum /= 6;
				//process allpass
				float reverb_out = ap1.process(sum);
				float out = (reverb_out * m_DryWet) + input * (1.f - m_DryWet); 
				return out;
			}
			inline float processFreeze(float input){
				float sum = 0;
				for (auto i : m_comb){sum += i.process(input);}
				sum /= 6;
				float reverb_out = ap1.processFreeze(sum);
				float out = reverb_out * m_DryWet + input * (1.f - m_DryWet); 
				return out;


			}

		};/*Moorer*/

	}/*Reverb*/
}/*noi*/