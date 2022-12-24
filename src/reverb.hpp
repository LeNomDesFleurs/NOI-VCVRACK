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
				ap1.SetGain(rt60);
				ap2.SetGain(rt60);
				cb1.SetGain(rt60);
				cb2.SetGain(rt60);
				cb3.SetGain(rt60);
				cb4.SetGain(rt60);
			}

			void setDryWet(float DryWet) { m_DryWet = DryWet; }

			float process(float input) {
				m_b0 = input;
				m_b0 = ap1.ProcessFilter(m_b0);
				m_b0 = ap2.ProcessFilter(m_b0);
				float c1 = cb1.ProcessFilter(input);
				float c2 = cb2.ProcessFilter(input);
				float c3 = cb3.ProcessFilter(input);
				float c4 = cb4.ProcessFilter(input);
				return (input * (1 - m_DryWet) + m_DryWet * (c1 + c2 + c3 + c4) / 4);
			}

		};/*Schroeder*/

		class FND{};


	}/*Reverb*/
}/*noi*/