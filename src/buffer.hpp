#include "outils.hpp"
#include "filter.hpp"
#include <math.h>

namespace noi{
namespace buffer{
class RingBuffer{
	private:
 	float m_read{0};
	float m_write;
	float m_buffer_size;
	float m_step{1.f};
	float m_time;
	bool m_freeze{false};
	std::vector<float> m_buffer;
	public:
	inline RingBuffer(float time){
		m_buffer_size = noi::Outils::convertMsToSample(time);
		m_buffer.resize(m_buffer_size);
		m_write = m_buffer_size;
	}
	 inline float read(){
		m_read = std::fmod((m_read+m_step), m_buffer_size);
		float tmp;
		float fractional;
		fractional = std::modf(m_read, &tmp);
		int int_read = static_cast<int> (tmp);
		float sample1 = m_buffer[int_read];
		float sample2 = m_buffer[int_read+1];
		//linear interpolation
		float output = ((sample2-sample1) * fractional) + sample1;
		return output;
	}
	// inline float read(){
	// 	m_read = std::fmod((m_read + 1), m_buffer_end);
	// 	int int_read = static_cast<int> (m_read);
	// 	return m_buffer[int_read];
	// }
	inline void write(float new_sample){
		if (m_freeze){return;}
		m_write = std::fmod((m_write + 1.f) , m_buffer_size);
		int int_write = static_cast<int>(m_write);
		m_buffer[int_write] = new_sample;
	}
	inline float getSize(){
		float size = m_read - m_write;
		if (size < 0.f){size += m_buffer_size;}
		size = m_buffer_size - size;
		return size;
	}

	inline void setSize(float new_time){
		if (m_freeze){
			m_step = m_time / new_time;
			return;
		}
		m_time = new_time;
		float actual_size = getSize();
		float size_goal = noi::Outils::convertMsToSample(new_time);
		//ralenti si le buffer doit être plus petit
		//accelere si le buffer doit être plus grand
		//approxime si il est a plus ou moins 0.5ms (20,000 smpl)
		if (actual_size < (size_goal - 1.f)){
			m_step = 0.8;
		}
		else if (actual_size > (size_goal + 1.f)){
			m_step = 1.2;
		}
		else {
			m_step = 1.f;}

	}
	inline void setFreeze(bool statut){
		m_freeze = statut;
	}
};
}
}