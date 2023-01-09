#pragma once

#ifndef OUTILS
#define OUTILS
#include <vector>
const float cheappi{ 3.14159265359 };

namespace noi {

	namespace Outils {

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

		inline float process(float x) {
			float y = x * alpha + yz * (1 - alpha);
			yz = y;
			return y;
		}

		inline LPF(float freq) { setParam(freq); }

	};/*LPF*/

		inline int convertMsToSample(float time) { int temp = static_cast <int> (48000.f * time); return temp; }
	
		inline int mapValueFloatToInt(float inMin, float inMax, float value, int outMin, int outMax) {
			float ratio = ((outMax - outMin) / (inMax - inMin));
			float offset{ outMin - (inMin * ratio) };
			int output = static_cast<int> (value * ratio + offset);
			return output;
		}

		inline float mapValue(float value, float inMin, float inMax, float outMin, float outMax) {
			float ratio{ (outMax - outMin) / (inMax - inMin) };
			float offset{ outMin - (inMin * ratio) };
			float output{ value * ratio + offset };
			return output;
		}

		template <class T>

		inline void clipRef(T& value, T min, T max) {
			if (value < min) { value = min; }
			if (value > max) { value = max; }
		}
		template <class T>

		inline T clip(T value, T min, T max) {
			if (value < min) { value = min; }
			if (value > max) { value = max; }
			return value;
		}

		inline float spliter(float target, float state, float diff) {
			if (state < target) { state += (target - state) * 2; }
			float slope = 0 - (1 / diff);
			state -= target;
			float coef = 1 + slope * state;
			if (coef < 0) { return 0; }
			else return coef;
		}

		class RingBuffer{
		private:
		bool m_repitch;
		float m_actual_duration;
		float m_max_duration;
		int m_read;
		float m_pitch_read;
		int m_pitch_step{4};
		int m_write;
		int m_buffer_end;
		bool m_upsampled{false};
		std::vector<float> m_buffer;
		std::vector<float> m_buffer_upsample;
		noi::Outils::LPF m_aafilter{10000};
		public:
		inline void upSample(){
			if (m_upsampled){return;}
			m_buffer_upsample.resize(m_buffer.size()*4);
			std::fill(m_buffer_upsample.begin(), m_buffer_upsample.end(), 0.f);
			int size = m_buffer_upsample.size();
			for (int i = 0, j = 0;  i < size; i += 4, j++){
				m_buffer_upsample[i] = m_buffer[j];
			}
			for (auto i : m_buffer_upsample){
				i = m_aafilter.process(i);
			}
			m_upsampled = true;
			m_read *= 4;
		}
		inline int getStep(){return m_pitch_step;}

		inline void setSize(float time){
		if (time == m_actual_duration) {return;} 
		m_actual_duration = clip(time, 0.f, m_max_duration);
		m_buffer_end = noi::Outils::convertMsToSample(m_actual_duration);
		m_repitch = false;
		m_upsampled = false;
		}
		inline void repitch(float time){
		if (time == m_actual_duration){return;}
		m_repitch = true;
		m_pitch_step = static_cast<int> (m_actual_duration / time *4);
		}
		inline float read(){
		if (m_repitch){
			upSample();
			m_read = (m_read + m_pitch_step) % m_buffer_end;
		return m_buffer_upsample[m_read];
		}
		else{
		m_read = (m_read + 1) % m_buffer_end;
		return m_buffer[m_read];
		}
		}
		inline void write(float newSample){
		m_write = (m_write + 1) % m_buffer_end;
		m_buffer[m_write] = newSample;
		}
		//erase data
		inline void setMaxSize(float maxTime){
		m_buffer_end = noi::Outils::convertMsToSample(maxTime);
		m_read = 0; 
		m_write = m_buffer_end; 
		m_max_duration = maxTime;
		m_buffer.resize(m_buffer_end); }	
		
		inline RingBuffer(float maxtime){setMaxSize(maxtime);}
		};


	}/*Outils*/
}/*noi*/

#endif /*OUTILS*/