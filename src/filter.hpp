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
		float process(float b0);
		void setParam(float fc, float fb);
	};
	//first order LPF
	class LPF {
	private:
		float alpha=0;
		float yz=0;
		float m_freq{ 8000 };
	public:
		void setParam(float freq);
		float process(float x);
		LPF(float freq);
	}; /*LPF*/

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
		void computeLPFCoef();
		void computeHPFCoef();
		void computeBPFCoef();
		void computePEAKCoef();
		void setParam(float fc, float Q, float G);
		void setParam(float frequence, float Q);
		void setParam(float frequence);
		std::string getType();
		float process(float b0);
		void setType(std::string type);
		Biquad(std::string type, float freq, float Q);
		Biquad(std::string type);
	 }; /*Biquad*/

	class Allpass {
	private:
		noi::buffer::RingBuffer m_buffer{0.2};
		float m_gain=0.;
		float m_looptime=0.;
	public:
		void clearBuffer();
		void setReadSpeed(float ratio);
		void setGain(float rt60);
		void overrideFeedback(float feedback);
		float process(float input);
		void resize(float time);
		Allpass(float time);
	};

	class Comb {
	private:
		float m_gain=0.;
		float m_looptime=0.;
		noi::buffer::RingBuffer m_buffer{2.f};
	public:
		void clearBuffer();
		void setReadSpeed(float ratio);
		void setGain(float rt60);
		void overrideFeedback(float feedback);
		float process(float input);
		float processFreezed();
		void resize(float time);
		void setFreeze(bool statut);
		Comb(float time);
	}; /*Comb*/

}/*Filter*/

}/*noi*/

#endif /*FILTER*/