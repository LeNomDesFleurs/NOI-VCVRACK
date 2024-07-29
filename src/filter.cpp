
#include "filter.hpp"

namespace noi
{

	namespace Filter
	{
		// alpass biquad
		float BPF::process(float b0)
		{
			float xh_new = b0 - m_d * (1 - m_c) * xh[0] + m_c * xh[1];
			float ap_y = -m_c * xh_new + m_d * (1 - m_c) * xh[0] + xh[1];
			xh[1] = xh[0];
			xh[0] = xh_new;
			return 0.5 * (b0 - ap_y);
		}
		void BPF::setParam(float fc, float fb)
		{
			if (m_fc == fc && m_fb == fb)
			{
				return;
			}
			m_fc = fc;
			m_fb = fb;
			float Wb = 2 * fb / m_fs;
			float Wc = 2 * fc / m_fs;
			m_c = (tanf(cheappi * Wb / 2) - 1) / (tanf(cheappi * Wb / 2) + 1);
			m_d = -cosf(cheappi * Wc);
		}

		// first order LPF

		void LPF::setParam(float freq)
		{
			if (m_freq == freq)
			{
				return;
			}
			m_freq = freq;
			float omega = 2 * cheappi * freq;
			alpha = 1 / ((48000 / omega) + 1);
		}

		float LPF::process(float x)
		{
			float y = x * alpha + yz * (1 - alpha);
			yz = y;
			return y;
		}

		LPF::LPF(float freq) { setParam(freq); }

		void Biquad::computeLPFCoef()
		{
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
		void Biquad::computeHPFCoef()
		{
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
		void Biquad::computeBPFCoef()
		{
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
		void Biquad::computePEAKCoef()
		{
			float V0 = powf(10, m_G / 20);
			float K = tanf(cheappi * m_fc / m_sample_rate);
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

		void Biquad::computeCoef()
		{
			m_omega = 2.f * cheappi * (m_fc / m_sample_rate);
			m_cosomega = cos(m_omega);
			m_sinomega = sin(m_omega);
			m_alpha = m_sinomega / (2 * m_Q);
			if (m_type == "PEAK")
			{
				computePEAKCoef();
			}
			else if (m_type == "LPF")
			{
				computeLPFCoef();
			}
			else if (m_type == "HPF")
			{
				computeHPFCoef();
			}
			else if (m_type == "BPF")
			{
				computeBPFCoef();
			}
		}

		void Biquad::setParam(float fc, float Q, float G)
		{
			if (m_fc == fc && m_Q == Q && m_G == G)
			{
				return;
			}
			m_fc = fc;
			m_G = G;
			m_Q = Q;
			computeCoef();
		}

		void Biquad::setParam(float frequence, float Q)
		{
			if (m_fc == frequence && m_Q == Q)
			{
				return;
			}
			m_fc = frequence;
			m_Q = Q;
			computeCoef();
		}
		void Biquad::setParam(float frequence)
		{
			if (m_fc == frequence)
			{
				return;
			}
			computeCoef();
		}
		std::string Biquad::getType() { return m_type; }

		void Biquad::setSampleRate(float sample_rate)
		{
			m_sample_rate = sample_rate;
			computeCoef();
		}

		float Biquad::process(float b0)
		{
			// feedback & clipping
			float feedback = m_a[0];
			// 1500 chosed by experimentation w/ sinensis, self osc around Q = 38
			feedback *= (m_Q / 1500.F);
			if (feedback < -4.5 || feedback > 4.5)
			{
				feedback /= 10.f;
			}
			feedback = rack::math::clamp(feedback, -5.f, 5.f);
			b0 += feedback;
			// shift new value in
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

			m_a[0] = m_b[0] * m_b_gain[0] + m_b[1] * m_b_gain[1] + m_b[2] * m_b_gain[2] - m_a[1] * m_a_gain[1] - m_a[2] * m_a_gain[2];

			return m_a[0];
			// return m_sample_rate;
		}
		void Biquad::setType(std::string type) { m_type = type; }
		Biquad::Biquad(std::string type, float freq, float Q)
		{
			setType(type);
			setParam(freq, Q);
		}
		Biquad::Biquad(std::string type) { setType(type); }

		void FeedbackFilter::clearBuffer()
		{
			m_buffer.clearBuffer();
		}
		void FeedbackFilter::setReadSpeed(float ratio)
		{
			m_buffer.setStep(ratio);
		}
		void FeedbackFilter::setGain(float rt60)
		{
			m_gain = -60 * m_looptime / rt60;
			m_gain = pow(10, (m_gain / 20));
			m_gain = m_gain > 1. ? 0.99 : m_gain;
		}
		void FeedbackFilter::overrideFeedback(float feedback)
		{
			m_gain = feedback;
		}

		void FeedbackFilter::resize(float time)
		{
			m_buffer.setSize(time);
			m_looptime = time;
		}

		void FeedbackFilter::setSampleRate(float sample_rate)
		{
			m_buffer.setSampleRate(sample_rate);
		}

		float Allpass::process(float input)
		{
			float delay = m_buffer.read();
			delay = rack::math::clamp(delay, -5.f, 5.f);
			float y = ((input + delay * m_gain) * (-m_gain)) + delay;
			m_buffer.write(y);
			return y;
		}

		float Comb::process(float input)
		{
			float delay = m_buffer.read();
			delay = rack::math::clamp(delay, -5.f, 5.f);
			float y = delay * m_gain + input;
			m_buffer.write(y);
			//volume tend to lower when the looptime is high
			float compensate_gain = std::min(1.0f + (m_looptime / 2.0f), 2.0f);
			float output = y * compensate_gain;
			return output;
		}

		float Comb::processFreezed()
		{
			float compensate_gain = std::min(1.0f + (m_looptime / 2.0f), 2.0f);
			return m_buffer.read()*compensate_gain;
		}

		void Comb::setFreeze(bool statut)
		{
			m_buffer.setFreeze(statut);
		}

	} /*Filter*/

} /*noi*/
