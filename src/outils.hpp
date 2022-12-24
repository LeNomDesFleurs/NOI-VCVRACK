#pragma once

#ifndef OUTILS
#define OUTILS

namespace noi {

	namespace Outils {

		inline int MsToSample(float time) { int temp = static_cast <int> (48000.f * time); return temp; }
	
		inline int MapValueFloatToInt(float inMin, float inMax, float value, int outMin, int outMax) {
			float ratio = ((outMax - outMin) / (inMax - inMin));
			float offset{ outMin - (inMin * ratio) };
			int output = static_cast<int> (value * ratio + offset);
			return output;
		}

		inline float MapValue(float value, float inMin, float inMax, float outMin, float outMax) {
			float ratio{ (outMax - outMin) / (inMax - inMin) };
			float offset{ outMin - (inMin * ratio) };
			float output{ value * ratio + offset };
			return output;
		}

		template <class T>

		inline void Clip(T* value, T min, T max) {
			if (*value < min) { *value = min; }
			if (*value > max) { *value = max; }
		}

		inline float spliter(float target, float state, float diff) {
			if (state < target) { state += (target - state) * 2; }
			float slope = 0 - (1 / diff);
			state -= target;
			float coef = 1 + slope * state;
			if (coef < 0) { return 0; }
			else return coef;
		}




	}/*Outils*/
}/*noi*/

#endif /*OUTILS*/