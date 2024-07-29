#include "reverb.hpp"

namespace noi
{

	namespace Reverb
	{

		Schroeder::Schroeder()
		{
			ap1.setReadSpeed(1.0);
			ap2.setReadSpeed(1.0);
			cb1.setReadSpeed(1.0);
			cb2.setReadSpeed(1.0);
			cb3.setReadSpeed(1.0);
			cb4.setReadSpeed(1.0);
		}
		void Schroeder::setStep(float speed)
		{
			ap1.setReadSpeed(speed);
			ap2.setReadSpeed(speed);
			cb1.setReadSpeed(speed);
			cb2.setReadSpeed(speed);
			cb3.setReadSpeed(speed);
			cb4.setReadSpeed(speed);
		}
		void Schroeder::setTime(float rt60)
		{
			// if (m_rt60 == rt60) { return; }
			m_rt60 = rt60;
			ap1.overrideFeedback(rt60);
			ap2.overrideFeedback(rt60);
			cb1.overrideFeedback(rt60);
			cb2.overrideFeedback(rt60);
			cb3.overrideFeedback(rt60);
			cb4.overrideFeedback(rt60);
		}
		void Schroeder::setDryWet(float DryWet) { m_DryWet = DryWet; }
		float Schroeder::process(float input)
		{
			float b0 = input;
			float c1 = cb1.process(b0);
			float c2 = cb2.process(b0);
			float c3 = cb3.process(b0);
			float c4 = cb4.process(b0);
			float comb_sum = (c1 + c2 + c3 + c4) / 4.f;
			comb_sum = ap1.process(comb_sum);
			comb_sum = ap2.process(comb_sum);
			return noi::Outils::dryWet(input, comb_sum, m_DryWet);
		}

		StereoMoorer::StereoMoorer(noi::Reverb::StereoMoorer::Parameters params)
		{
			for (int i = 0; i < 2; i++)
			{
				m_allpasses[i].clearBuffer();
				for (int j = 0; j < 6; j++)
				{
					m_combs[i][j].clearBuffer();
				}
			}
			updateParameters(params);
		}

		void StereoMoorer::updateParameters(noi::Reverb::StereoMoorer::Parameters params)
		{
			m_params = params;
			setTime();
			setFreeze();
			resizeComb();
			setPan();
		}
		void StereoMoorer::setPan()
		{
			float variation = m_params.variation;
			for (int i = 2; i < 6; i++)
			{
				m_pan_coefs[i] = std::min(variation, 1.0f);
				variation *= 1.5;
			}
		}
		void StereoMoorer::setTime()
		{
			float rt60 = m_params.rt60;
			float variation = m_params.variation + 1.f;
			for (int i = 0; i < 2; i++)
			{
				m_allpasses[i].setGain(rt60);
				for (auto &comb : m_combs[i])
				{
					comb.setGain(rt60);
					rt60 *= variation;
				}
			}
		}
		void StereoMoorer::setFreeze()
		{
			for (int i = 0; i < 2; i++)
			{
				for (auto &combs : m_combs[i])
				{
					combs.setFreeze(m_params.freeze);
				}
			}
		}
		void StereoMoorer::resizeComb()
		{
			float variation = m_params.variation + 1.f;
			for (int i = 0; i < 2; i++)
			{
				float time = m_params.comb_time;
				for (auto &comb : m_combs[i])
				{
					comb.resize(time);
					time *= variation;
				}
			}
		}
		void StereoMoorer::processCombs(std::array<float, 2> inputs)
		{
			for (int i = 0; i < 2; i++)
			{
				// process combs
				if (m_params.freeze)
				{
					for (int j = 0; j < 6; j++)
					{
						m_combs_status[i][j] = m_combs[i][j].processFreezed();
					}
				}
				else
				{
					for (int j = 0; j < 6; j++)
					{
						m_combs_status[i][j] = m_combs[i][j].process(inputs[i]);
					}
				}
			}
		}

		std::array<float, 2> StereoMoorer::processStereo(std::array<float, 2> inputs)
		{
			std::array<float, 2> outputs = {0, 0};
			processCombs(inputs);
			for (int i = 0; i < 6; i++)
			{
				// pan combs output, mix the two channels
				float in_left = m_combs_status[0][i];
				float in_right = m_combs_status[1][i];
				// out_left = in_left * pan + in_right * (1 - pan);
				m_combs_status[0][i] = noi::Outils::dryWet(in_left, in_right, m_pan_coefs[i]);
				m_combs_status[1][i] = noi::Outils::dryWet(in_left, in_right, (1.f - m_pan_coefs[i]));
			}
			for (int i = 0; i < 2; i++)
			{
				float comb_sum = 0.f;
				for (auto j : m_combs_status[i])
				{
					comb_sum += j;
				}
				comb_sum /= 6.f;
				if (!m_params.freeze)
					comb_sum -= inputs[i];
				float to_allpass = noi::Outils::dryWet(inputs[i], comb_sum, m_params.dry_wet);
				outputs[i] = m_allpasses[i].process(to_allpass);
			}
			return outputs;
		}

		void StereoMoorer::SetSampleRate(float sample_rate)
		{
			for (int i = 0; i < 2; i++)
			{
				m_allpasses[i].setGain(sample_rate);
				for (auto &comb : m_combs[i])
				{
					comb.setGain(sample_rate);
				}
			}
		}

	} /*Reverb*/
} /*noi*/