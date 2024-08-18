#pragma once

#include <cmath>
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
		void setParam(float freq);

		float process(float x);

		LPF(float freq);
		}; /*LPF*/

			float truncate(float input);
			float modulo(float input, float max);

			// float modulo(float input, float max, float min = 0.){
			// 	if (input>max) input = input - max;
			// 	if (input<min) input = max+input;
			// 	return input;
			// }

			float decimal(float input);

			float convertMsToSample(float time, float m_sampling_rate);

			int mapValueFloatToInt(float inMin, float inMax, float value, int outMin, int outMax);

			float mapValue(float value, float inMin, float inMax, float outMin, float outMax);
			template <class T>

			void clipRef(T &value, T min, T max);
			
			template <class T>

			T clip(T value, T min, T max);

			float spliter(float target, float state, float diff);

			/// @brief take two signals and return the crossfade
			/// @param dry dry signal
			/// @param wet wet signal
			/// @param parameter 0 full dry / 1 full wet
			/// @return
			float dryWet(float dry, float wet, float parameter);

	}/*Outils*/
}/*noi*/
