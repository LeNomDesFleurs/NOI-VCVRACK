#pragma once

#ifndef OUTILS
#define OUTILS
#include <vector>
const float cheappi{ 3.14159265359 };
#include "outils.hpp"

namespace noi {

	namespace Outils {

			class LPF {
	private:
		float alpha;
		float yz;
		float m_freq{ 8000 };
	public:
		void setParam(float freq) {
			if (m_freq == freq) { return; }
			m_freq = freq;
			float omega = 2 * cheappi * freq;
			alpha = 1 / ((48000 / omega) + 1);
		}

		float process(float x) {
			float y = x * alpha + yz * (1 - alpha);
			yz = y;
			return y;
		}

		LPF(float freq) { setParam(freq); }

	};/*LPF*/

	float truncate(float input){
		int x = (int)input;
		float output = (float)x;
		return output;
	}

	float modulo(float input, float max){
		if (input>max) input = input - max;
		if (input<0.) input = max+input;
		return input;
	}

	// float modulo(float input, float max, float min = 0.){
	// 	if (input>max) input = input - max;
	// 	if (input<min) input = max+input;
	// 	return input;
	// }

	float decimal(float input){
		return input - truncate(input);
	}

		float convertMsToSample(float time) { 
			float temp = truncf(48000.f * time); 
			return temp; }
	
		int mapValueFloatToInt(float inMin, float inMax, float value, int outMin, int outMax) {
			float ratio = ((outMax - outMin) / (inMax - inMin));
			float offset{ outMin - (inMin * ratio) };
			int output = static_cast<int> (value * ratio + offset);
			return output;
		}

		float mapValue(float value, float inMin, float inMax, float outMin, float outMax) {
			float ratio{ (outMax - outMin) / (inMax - inMin) };
			float offset{ outMin - (inMin * ratio) };
			float output{ value * ratio + offset };
			return output;
		}

		template <class T>

		void clipRef(T& value, T min, T max) {
			if (value < min) { value = min; }
			if (value > max) { value = max; }
		}
		template <class T>

		T clip(T value, T min, T max) {
			if (value < min) { value = min; }
			if (value > max) { value = max; }
			return value;
		}

		float spliter(float target, float state, float diff) {
			if (state < target) { state += (target - state) * 2; }
			float slope = 0 - (1 / diff);
			state -= target;
			float coef = 1 + slope * state;
			if (coef < 0) { return 0; }
			else return coef;
		}
		/// @brief take two signals and return the crossfade
		/// @param dry dry signal
		/// @param wet wet signal
		/// @param parameter 0 full dry / 1 full wet
		/// @return 
		float dryWet(float dry, float wet, float parameter){
			return (dry * (1-parameter)) + (wet * parameter);
		}
	

	}/*Outils*/
}/*noi*/

#endif /*OUTILS*/