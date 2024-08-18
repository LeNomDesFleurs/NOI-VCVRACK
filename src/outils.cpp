

#include "outils.hpp"

namespace noi
{

	namespace Outils
	{

		float truncate(float input)
		{
			int x = (int)input;
			float output = (float)x;
			return output;
		}

		float modulo(float input, float max)
		{
			if (input > max)
				input = input - max;
			if (input < 0.)
				input = max + input;
			return input;
		}

		// float modulo(float input, float max, float min = 0.){
		// 	if (input>max) input = input - max;
		// 	if (input<min) input = max+input;
		// 	return input;
		// }

		float decimal(float input)
		{
			return input - truncate(input);
		}

		float convertMsToSample(float time, float sampling_rate)
		{
			float temp = truncate(sampling_rate * time);
			return temp;
		}

		int mapValueFloatToInt(float inMin, float inMax, float value, int outMin, int outMax)
		{
			float ratio = ((outMax - outMin) / (inMax - inMin));
			float offset{outMin - (inMin * ratio)};
			int output = static_cast<int>(value * ratio + offset);
			return output;
		}

		float mapValue(float value, float inMin, float inMax, float outMin, float outMax)
		{
			float ratio{(outMax - outMin) / (inMax - inMin)};
			float offset{outMin - (inMin * ratio)};
			float output{value * ratio + offset};
			return output;
		}

		template <class T>

		void clipRef(T &value, T min, T max)
		{
			if (value < min)
			{
				value = min;
			}
			if (value > max)
			{
				value = max;
			}
		}
		template <class T>

		T clip(T value, T min, T max)
		{
			if (value < min)
			{
				value = min;
			}
			if (value > max)
			{
				value = max;
			}
			return value;
		}

		float spliter(float target, float state, float diff)
		{
			if (state < target)
			{
				state += (target - state) * 2;
			}
			float slope = 0 - (1 / diff);
			state -= target;
			float coef = 1 + slope * state;
			if (coef < 0)
			{
				return 0;
			}
			else
				return coef;
		}
		/// @brief take two signals and return the crossfade
		/// @param dry dry signal
		/// @param wet wet signal
		/// @param parameter 0 full dry / 1 full wet
		/// @return
		float dryWet(float dry, float wet, float parameter)
		{
			parameter = 1. - parameter;
			parameter = (parameter - 0.5) * 2;
			float volumes_dry = std::sqrt(0.5f * (1.f + parameter));
			float volumes_wet = std::sqrt(0.5f * (1.f - parameter));
			return (dry * volumes_dry) + (wet * volumes_wet);
		}

	} /*Outils*/
} /*noi*/
