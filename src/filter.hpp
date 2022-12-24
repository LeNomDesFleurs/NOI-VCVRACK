#pragma once



#ifndef FILTER
#define FILTER

#include <string>
#include <math.h>
#include <vector>
#include "outils.hpp"

const float cheappi{ 3.14159265359 };

namespace noi {

namespace Filter {
	//alpass biquad
	class BPF {
	private:
		float m_b1;
		float xh[2];
		float m_fs{ 48000 };
		float m_c, m_d;
		float m_fc{ 320 }, m_fb{ 100 };
	public:
		inline float processFilter(float b0) {
			float xh_new = b0 - m_d * (1 - m_c) * xh[0] + m_c * xh[1];
			float ap_y = -m_c * xh_new + m_d * (1 - m_c) * xh[0] + xh[1];
			xh[1] = xh[0];
			xh[0] = xh_new;
			return 0.5 * (b0 - ap_y);
		}
		inline void setParam(float fc, float fb) {
			if (m_fc == fc && m_fb == fb) { return; }
			m_fc = fc;
			m_fb = fb;
			float Wb = 2 * fb / m_fs;
			float Wc = 2 * fc / m_fs;
			m_c = (tanf(cheappi * Wb / 2) - 1) / (tanf(cheappi * Wb / 2) + 1);
			m_d = -cosf(cheappi * Wc);
		}
	};
	//first order LPF
	class LPF {
	private:
		float alpha;
		float yz;
		float m_freq{ 8000 };
	public:
		inline void setParam(float freq) {
			if (m_freq == freq) { return; }
			m_freq = freq;
			float omega = 2 * cheappi * freq;
			alpha = 1 / ((48000 / omega) + 1);
		}

		inline float processFilter(float x) {
			float y = x * alpha + yz * (1 - alpha);
			yz = y;
			return y;
		}

		inline LPF(float freq) { setParam(freq); }

	};/*LPF*/


	 class Biquad {

	private:
		std::string m_type;
		float 
		m_b0,
		m_b1,
		m_b2,
		m_a0,
		m_a1,
		m_a2;
		float 
		m_b0gain,
		m_b1gain,
		m_b2gain,
		m_a0gain,
		m_a1gain,
		m_a2gain;
		float m_fc;
		float m_Q;
		float m_omega;
		float m_cosomega;
		float m_sinomega;
		float m_alpha;
		float m_Fe;
		float m_G;
		float m_fS{ 48000 };

	public:

		inline void setGain(float b0, float b1, float b2, float a1, float a2) { m_b0gain = b0; m_b1gain = b1; m_b2gain = b2; m_a1gain = a1; m_a2gain = a2; }
		inline void computeLPFCoef(){
			m_a0gain = 1 + m_alpha;

			m_b0gain = (1 - m_cosomega) / 2;
			m_b0gain /= m_a0gain;

			m_b1gain = 1 - m_cosomega;
			m_b1gain /= m_a0gain;

			m_b2gain = m_b0gain;

			m_b2gain /= m_a0gain;

			m_a1gain = -2 * m_cosomega;
			m_a1gain /= m_a0gain;

			m_a2gain = 1 - m_alpha;
			m_a2gain /= m_a0gain;
		}
		inline void computeHPFCoef(){
			m_a0gain = 1 + m_alpha;

			m_b0gain = (1 + m_cosomega) / 2;
			m_b0gain /= m_a0gain;

			m_b1gain = -(1 + m_cosomega);
			m_b1gain /= m_a0gain;

			m_b2gain = m_b0gain;
			m_b2gain /= m_a0gain;

			m_a1gain = -2 * m_cosomega;
			m_a1gain /= m_a0gain;

			m_a2gain = 1 - m_alpha;
			m_a2gain /= m_a0gain;
		}
		inline void computeBPFCoef() {
			m_a0gain = 1 + m_alpha;

			m_b0gain = m_alpha * m_Q;
			m_b0gain /= m_a0gain;

			m_b1gain = 0;
			m_b1gain /= m_a0gain;

			m_b2gain = -m_Q * m_alpha;
			m_b2gain /= m_a0gain;

			m_a1gain = -2 * m_cosomega;
			m_a1gain /= m_a0gain;

			m_a2gain = 1 - m_alpha;
			m_a2gain /= m_a0gain;
		}
		inline void computePEAKCoef() {
			float V0 = powf(10, m_G / 20);
			float K = tanf(cheappi * m_fc / m_fS);
			float k2 = K * K;
			float divide = (1 + (1 / m_Q) * K + k2);

			m_b0gain = 1 + (V0 / m_Q) * K + k2;
			m_b0gain /= divide;
			m_b1gain = 2 * (k2 - 1);
			m_b1gain /= divide;
			m_a1gain = m_b1gain;
			m_b2gain = 1 - (V0 / m_Q) * K + k2;
			m_b2gain /= divide;
			m_a2gain = 1 - (1 / m_Q) * K + k2;
			m_a2gain /= divide;

		}

		inline void setParam(float fc, float Q, float G) {
			if (m_fc == fc && m_Q == Q && m_G == G) { return; }
			m_fc = fc;
			m_G = G;
			m_Q = Q;
			if (m_type == "PEAK") { computePEAKCoef();}
		}
	
		inline void setParam(float frequence, float Q) {
			if (m_fc == frequence && m_Q == Q) { return; }
			m_fc = frequence;
			m_Q = Q;
			m_omega = 2.f * cheappi * (m_fc / 48000.f);
			m_cosomega = cos(m_omega);
			m_sinomega = sin(m_omega);
			m_alpha = m_sinomega / (2 * m_Q);
			if (m_type == "LPF") { computeLPFCoef(); }
			else if (m_type == "HPF") { computeHPFCoef(); }
			else if (m_type == "BPF") { computeBPFCoef(); }
		}
		inline void setParam(float frequence) {
			if (m_fc == frequence) { return; }
			m_fc = frequence;
			m_omega = 2.f * cheappi * (m_fc / 48000.f);
			m_cosomega = cos(m_omega);
			m_sinomega = sin(m_omega);
			m_alpha = m_sinomega / (2.f * m_Q);
			if (m_type == "LPF") { computeLPFCoef(); }
			else if (m_type == "HPF") { computeHPFCoef(); }
			else if (m_type == "BPF") { computeBPFCoef(); }
			else if (m_type == "PEAK") { computePEAKCoef(); }
		}
		inline std::string getType() { return m_type; }

		inline float processFilter(float b0) {
			//feedback & clipping
			float feedback = m_a0;
			//1500 chosed by experimentation w/ sinensis, self osc around Q = 38
			feedback *= (m_Q / 1500.F);
			if (feedback < -4.5 || feedback > 4.5) {
				feedback /= 10.f;
			}
			feedback = rack::math::clamp(feedback, -5.f, 5.f);
			b0 += feedback;
			m_b2 = m_b1;
			m_b1 = m_b0;
			m_b0 = b0;
			m_a2 = m_a1;
			m_a1 = m_a0;

		
			//b0 += (m_a1 * m_Q / 20);
			//b0 = rack::math::clamp(b0, -5.f, 5.f);
			m_a0 = m_b0 * m_b0gain
				+ m_b1 * m_b1gain
				+ m_b2 * m_b2gain
				- m_a1 * m_a1gain
				- m_a2 * m_a2gain;

			return m_a0;
		}
		inline void setType(std::string type) { m_type = type; }
		inline Biquad(std::string type, float freq, float Q) { m_type = type; setParam(freq, Q); }
		inline Biquad(std::string type) { m_type = type; }
	};/*Biquad*/

	class Allpass {

	private:
		int m_size;
		std::vector<float> m_buffer;
		int m_read;
		int m_write;
		float m_gain;
		float m_looptime;

	public:

		inline Allpass(float time) { m_looptime = time; m_size = noi::Outils::MsToSample(time); m_read = 0; m_write = m_size; m_buffer.resize(m_size); }
		inline void SetGain(float rt60) {
			m_gain = -60 * m_looptime / rt60;
			m_gain = pow(10, (m_gain / 20));
		}


		inline float ProcessFilter(float input) {
			float delay = m_buffer[m_read];
			m_read = (m_read + 1) % m_size;

			float y = ((input + delay * m_gain) * (-m_gain)) + delay;

			m_buffer[m_write] = y;
			m_write = (m_write + 1) % m_size;

			return y;
		}
	};

	

	class Comb {
	private:
		int m_size;
		std::vector<float> m_buffer;
		int m_read;
		int m_write;
		float m_gain;
		float m_looptime;

	public:
		inline 	Comb(float time) { m_looptime = time; m_size = noi::Outils::MsToSample(time); m_read = 0; m_write = m_size; m_buffer.resize(m_size); }
		inline void SetGain(float rt60) {
			m_gain = -60 * m_looptime / rt60;
			m_gain = pow(10, (m_gain / 20));
		}


		inline float ProcessFilter(float input) {
			float delay = m_buffer[m_read];
			m_read = (m_read + 1) % m_size;


			float y = delay * m_gain + input;
			m_buffer[m_write] = y;
			m_write = (m_write + 1) % m_size;

			return y;
		}
	};/*Comb*/


}/*Filter*/

}/*noi*/

#endif /*FILTER*/