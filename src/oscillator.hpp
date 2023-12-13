#pragma once
#ifndef OSCILLATOR
#define OSCILLATOR

#include <string>
#include <math.h>
#include <vector>
#include <string>
#include "outils.hpp"
namespace noi{
	namespace Oscillator{
		//Ramp based oscillator with triangle and square output
		class RampBipolar {
		private:
			float m_statut{0};
			std::string m_type;
		public:
			float Process(float freq, float sampleRate) {
				m_statut += 10 / (sampleRate / freq);
				if (m_statut > 5) { m_statut = -5; }
				if (m_type == "SAW") { return m_statut; }
				else if (m_type == "SQR") { return (m_statut > 0) ? 5 : -5; }
				else if (m_type == "TRI") { return (abs(m_statut) - 2.5) * 2 ;}
				else{return 0;}
			}
			RampBipolar(std::string type) { m_type = type; }
		};

		class Wind {
		private:
			float m_statut{ 0 };
			float m_step{};
			float m_line_goal{ 0 };
			float m_up_or_down{1};
			float m_coef{ 0.5 }, m_size{ 10 };
			float m_max_limit{+5};
			float m_min_limit{-5};
			//time in second
			float m_time{2};

		public:
			float getUpOrDown() {return m_up_or_down;}
			void SetTime(float time) {
				m_time = time;
			}
			void setParam(float coef, float size) {
				m_coef = coef;
				m_size = size;
			}
			void NewLine() {
				//invert direction
				m_up_or_down = (m_up_or_down > 0) ? -1 : 1;
				//genere une nouvelle fin
				float new_rand = 2.0 * random::normal();
				if (m_up_or_down) { new_rand = noi::Outils::mapValue(new_rand, -5.f, +5.f, m_statut, rack::math::clamp(m_statut + m_size, -5.f, 5.f)); }
				if (!m_up_or_down) { new_rand = noi::Outils::mapValue(new_rand, -5.f, +5.f, rack::math::clamp(m_statut - m_size, -5.f, 5.f), m_statut); }
				//alternate ascend and descend
				m_line_goal = new_rand;
				float difference = (m_up_or_down > 0) ? m_line_goal - m_statut : m_statut - m_line_goal;
				m_step = difference / (m_time * 48000);
			}
			float Process() {
				//avance sur la ligne actuelle

				m_statut += m_step * m_up_or_down;
				if (m_up_or_down > 0 && m_statut > m_line_goal) { NewLine(); }
				if (m_up_or_down < 0 && m_statut < m_line_goal) { NewLine(); }
				return m_statut;
			}
			Wind() { m_max_limit = +5; m_max_limit = -5; NewLine(); }

		};

	}
}


#endif /*OSCILLATOR*/