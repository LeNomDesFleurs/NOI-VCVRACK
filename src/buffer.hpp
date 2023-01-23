#include "outils.hpp"
#include "filter.hpp"
#include <math.h>

namespace noi{
namespace buffer{
class RingBuffer{
	private:
 	float m_read;
	float m_write;
	float m_buffer_size;
	float m_step;
	float m_time;
	bool m_freeze{false};
	float m_actual_size;
	std::vector<float> m_buffer;
	public:
	inline RingBuffer(float max_time){
		m_buffer_size = noi::Outils::convertMsToSample(max_time);
		size_t buffer_int = static_cast<size_t> (m_buffer_size);
		m_buffer.resize(buffer_int);
		m_read = 0.f;
		m_write = m_buffer_size/2.f;
		m_step = 1.f;
	}
	 inline float read(){
		m_read += m_step;
		m_read = fmod(m_read, m_buffer_size);
		float tmp;
		float fractional;

		fractional = std::modf(m_read, &tmp);
		int int_read = static_cast<int> (tmp);
		float sample1 = m_buffer[int_read];
		float sample2 = m_buffer[int_read+1];
		//linear interpolation
		float output = ((sample2 - sample1) * fractional) + sample1;
		return output;
	}
	// inline float read(){
	// 	m_read = std::fmod((m_read + 1), m_buffer_end);
	// 	int int_read = static_cast<int> (m_read);
	// 	return m_buffer[int_read];
	// }
	inline void write(float new_sample){
		m_write += 1.f;
		m_write = fmod(m_write , m_buffer_size);
		int int_write = static_cast<int>(m_write);
		m_buffer[int_write] = new_sample;
	}
	inline float getSize(){
		float size = m_write - m_read;
		if (m_write < m_read){
			size = m_write + (m_buffer_size - m_read);
		}
		return size;
	}

	inline void setSize(float new_time){
		if (m_freeze){
			m_step = m_time / new_time;
			return;
		}
		m_time = new_time;
		float size_goal = noi::Outils::convertMsToSample(new_time);
		size_goal = rack::math::clamp(size_goal, 0.f,  (m_buffer_size-2.f));
		float actual_size = getSize();
		m_actual_size = actual_size;
		//ralenti si le buffer doit être plus petit
		//accelere si le buffer doit être plus grand
		if (actual_size < (size_goal - 2.f)){
			m_step = 0.8;
		}
		else if (actual_size< (size_goal - 4800.f)){
			m_step = 0.5;
		}
		else if (actual_size > (size_goal + 2.f)){
			m_step = 1.2;
		}
		else if (actual_size > (size_goal + 4800.f)){
			m_step = 2.f;
		}
		else {m_step = 1.f;}
	}
	// inline void setSize(float new_time){
	// 	float m_size = noi::Outils::convertMsToSample(new_time);
	// 	float actual_size = getSize();
	// 	if (actual_size < (m_size - 2.f)){
	// 		m_step = 0.8;
	// 	}
	// 	else if(actual_size > (m_size + 2.f)){
	// 		m_step = 1.2;
	// 	}
	// 	else {m_step = 1.f;}
	// 	}
	inline float getActualSize(){
		return m_actual_size;
	}
	inline void setFreeze(bool statut){
		m_freeze = statut;
	}
};
}
}