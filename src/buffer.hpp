#include "outils.hpp"
#include "filter.hpp"
#include <math.h>

namespace noi{
namespace buffer{
class RingBuffer{
	private:
 	float m_read;
	int m_write;
	float m_buffer_end;
	int m_step;
	float m_time;
	bool m_freeze{false};
	std::vector<float> m_buffer;
	public:
	inline RingBuffer(float time){
		float tmp = noi::Outils::convertMsToSample(time);
		m_buffer.resize(tmp);
		m_buffer_end = static_cast<float> (m_buffer.size());
		m_write = m_buffer_end;
		m_read = 0;

	}
	 inline float read(){
		//upSample();
		float tmp;
		float fractional;
		fractional = std::modf(m_read, &tmp);
		int int_read = static_cast<int> (tmp);
		float sample1 = m_buffer[int_read];
		float sample2 = m_buffer[int_read+1];

		float output = ((sample2-sample1) * fractional) + sample1;
		m_read = std::fmod((m_read+m_step), m_buffer_end);
		return output;
	}
	// inline float read(){
	// 	m_read = std::fmod((m_read + 1), m_buffer_end);
	// 	int int_read = static_cast<int> (m_read);
	// 	return m_buffer[int_read];
	// }
	inline void write(float newSample){
		if (m_freeze){return;}
		m_write = std::fmod((m_write + 1) , m_buffer_end);
		int int_write = static_cast<int>(m_write);
		m_buffer[int_write] = newSample;
	}
	inline float  getSize(){
		float size = m_read - m_write;
		if (size < 0.f){size += m_buffer_end;}
		size = m_buffer_end - size;
		return size;
	}

	inline void setSize(float new_time){
		if (m_freeze){
			m_step = m_time / new_time;
			return;
		}
		float size_goal = noi::Outils::convertMsToSample(new_time);
		float actual_size = getSize();
		//ralenti si le buffer doit être plus petit
		//accelere si le buffer doit être plus grand
		//approxime si il est a plus ou moins 0.5ms (20,000 smpl)
		if (actual_size < (size_goal - 20000.f)){
			m_step = 0.8;
		}
		else if (actual_size > (size_goal + 20000.f)){
			m_step = 1.2;
		}
		else {
			m_step = 1.f;}

		m_time = new_time;
	}
	inline void setFreeze(bool statut){
		m_freeze = statut;
	}
	inline void setStep(int step){m_step = step;}

};
}
}