#pragma once



#ifndef FILTER
#define FILTER

#include <string>
#include <math.h>
#include <vector>
#include "outils.hpp"
#include <rack.hpp>
#include "buffer.hpp"



namespace noi {

namespace Filter {
	//alpass biquad
	class BPF {
	private:
		// float m_b1=0.;
		float xh[2]={0., 0.};
		float m_fs{ 48000 };
		float m_c=0., m_d=0.;
		float m_fc{ 320 }, m_fb{ 100 };
	public:
		inline float process(float b0) {
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
		float alpha=0;
		float yz=0;
		float m_freq{ 8000 };
	public:
		inline void setParam(float freq) {
			if (m_freq == freq) { return; }
			m_freq = freq;
			float omega = 2 * cheappi * freq;
			alpha = 1 / ((48000 / omega) + 1);
		}

		inline float process(float x) {
			float y = x * alpha + yz * (1 - alpha);
			yz = y;
			return y;
		}

		inline LPF(float freq) { setParam(freq); }

	};/*LPF*/


	 class Biquad {

	private:
		std::string m_type;
		float m_b[3]={0., 0., 0.};
		float m_a[3]={0., 0., 0.};
		float m_b_gain[3]={0., 0., 0.};
		float m_a_gain[3]={0., 0., 0.};
		float m_fc;
		float m_Q;
		float m_omega;
		float m_cosomega;
		float m_sinomega;
		float m_alpha;
		// float m_Fe;
		float m_G;
		float m_fS{ 48000 };

	public:
		inline void computeLPFCoef(){
			m_a_gain[0] = 1 + m_alpha;

			m_b_gain[0] = (1 - m_cosomega) / 2;
			m_b_gain[0] /= m_a_gain[0];

			m_b_gain[1] = 1 - m_cosomega;
			m_b_gain[1] /= m_a_gain[0];

			m_b_gain[2] = m_b_gain[0];
			m_b_gain[2] /= m_a_gain[0];

			m_a_gain[1] = -2 * m_cosomega;
			m_a_gain[1] /= m_a_gain[0];

			m_a_gain[2] = 1 - m_alpha;
			m_a_gain[2] /= m_a_gain[0];

		}
		inline void computeHPFCoef(){
			m_a_gain[0] = 1 + m_alpha;

			m_b_gain[0] = (1 + m_cosomega) / 2;
			m_b_gain[0] /= m_a_gain[0];

			m_b_gain[1] = -(1 + m_cosomega);
			m_b_gain[1] /= m_a_gain[0];

			m_b_gain[2] = m_b_gain[0];
			m_b_gain[2] /= m_a_gain[0];

			m_a_gain[1] = -2 * m_cosomega;
			m_a_gain[1] /= m_a_gain[0];

			m_a_gain[2] = 1 - m_alpha;
			m_a_gain[2] /= m_a_gain[0];
		}
		inline void computeBPFCoef() {
			m_a_gain[0] = 1 + m_alpha;

			m_b_gain[0] = m_alpha * m_Q;
			m_b_gain[0] /= m_a_gain[0];

			m_b_gain[1] = 0;
			m_b_gain[1] /= m_a_gain[0];

			m_b_gain[2] = -m_Q * m_alpha;
			m_b_gain[2] /= m_a_gain[0];

			m_a_gain[1] = -2 * m_cosomega;
			m_a_gain[1] /= m_a_gain[0];

			m_a_gain[2] = 1 - m_alpha;
			m_a_gain[2] /= m_a_gain[0];
		}
		inline void computePEAKCoef() {
			float V0 = powf(10, m_G / 20);
			float K = tanf(cheappi * m_fc / m_fS);
			float k2 = K * K;
			float divide = (1 + (1 / m_Q) * K + k2);

			m_b_gain[0] = 1 + (V0 / m_Q) * K + k2;
			m_b_gain[0] /= divide;
			m_b_gain[1] = 2 * (k2 - 1);
			m_b_gain[1] /= divide;
			m_a_gain[1] = m_b_gain[1];
			m_b_gain[2] = 1 - (V0 / m_Q) * K + k2;
			m_b_gain[2] /= divide;
			m_a_gain[2] = 1 - (1 / m_Q) * K + k2;
			m_a_gain[2] /= divide;

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

		inline float process(float b0) {
			//feedback & clipping
			float feedback = m_a[0];
			//1500 chosed by experimentation w/ sinensis, self osc around Q = 38
			feedback *= (m_Q / 1500.F);
			if (feedback < -4.5 || feedback > 4.5) {
				feedback /= 10.f;
			}
			feedback = rack::math::clamp(feedback, -5.f, 5.f);
			b0 += feedback;
			//shift new value in
			m_b[2] = rack::math::clamp(m_b[1], -100.f, 100.f);
			m_b[1] = rack::math::clamp(m_b[0], -100.f, 100.f);
			m_b[0] = b0;
			m_a[2] = rack::math::clamp(m_a[1], -100.f, 100.f);
			m_a[1] = rack::math::clamp(m_a[0], -100.f, 100.f);

			// m_b[2] = m_b[1];
			// m_b[1] = m_b[0];
			// m_b[0] = b0;
			// m_a[2] = m_a[1];
			// m_a[1] = m_a[0];

			m_a[0] = m_b[0] * m_b_gain[0]
				+ m_b[1] * m_b_gain[1]
				+ m_b[2] * m_b_gain[2]
				- m_a[1] * m_a_gain[1]
				- m_a[2] * m_a_gain[2];

			return m_a[0];
		}
		inline void setType(std::string type) { m_type = type; }
		inline Biquad(std::string type, float freq, float Q) { setType(type); setParam(freq, Q); }
		inline Biquad(std::string type) { setType(type); }
	};/*Biquad*/

	class Allpass {
	private:
		noi::buffer::RingBuffer m_buffer{0.2};
		float m_gain=0.;
		float m_looptime=0.;
	public:
	inline void clearBuffer(){
			m_buffer.clearBuffer();
			}
			inline void setReadSpeed(float ratio)
			{
				m_buffer.setStep(ratio);
			}
		inline void setGain(float rt60) {
			m_gain = -60 * m_looptime / rt60;
			m_gain = pow(10, (m_gain / 20));
		}
		inline void overrideFeedback(float feedback){
			m_gain = feedback;
		}
		inline float process(float input) {
			float delay = m_buffer.read();
			delay = rack::math::clamp(delay, -5.f, 5.f);
			float y = ((input + delay * m_gain) * (-m_gain)) + delay;
			m_buffer.write(y);
			return y;
		}
		inline void resize (float time){
			m_buffer.setSize(time);
			m_looptime = time;
		}
		inline 	Allpass(float time) {
			printf("allpass \n");
			// m_buffer = noi::buffer::RingBuffer(time);
		}
	};

	class Comb {
	private:
		float m_gain=0.;
		float m_looptime=0.;
		noi::buffer::RingBuffer m_buffer{2.f};
	public:
	inline void clearBuffer(){
			m_buffer.clearBuffer();
			}
		inline void setReadSpeed(float ratio){
			m_buffer.setStep(ratio);
		} 
		inline void setGain(float rt60) {
			m_gain = -60.f * m_looptime / rt60;
			m_gain = pow(10.f, (m_gain / 20.f));
		}
		inline void overrideFeedback(float feedback){
			m_gain = feedback;
		}
		inline float process(float input) {
			float delay = m_buffer.read();
			delay = rack::math::clamp(delay, -5.f, 5.f);
			float y = delay * m_gain + input;
			m_buffer.write(y);
			return y;
		}
		inline float processFreezed(){
			return m_buffer.read();
		}
		inline void resize(float time){
			//if (m_looptime == time){return;}
			m_buffer.setSize(time);
			m_looptime = time;
		}
		inline void setFreeze(bool statut){
			m_buffer.setFreeze(statut);
		}
		inline Comb(float time) {
			printf("comb \n");
			// m_buffer = noi::buffer::RingBuffer(time);
		}
	};/*Comb*/


}/*Filter*/

}/*noi*/

#endif /*FILTER*/