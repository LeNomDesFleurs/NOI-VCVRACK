#include "buffer.hpp"

namespace noi{
namespace buffer{

	RingBuffer::RingBuffer(float max_time){
		m_buffer_size = noi::Outils::convertMsToSample(max_time, m_sample_rate);
		size_t buffer_int = static_cast<size_t> (m_buffer_size);
		m_buffer.resize(buffer_int);
		m_read = 0.f;
		m_write = m_buffer_size/2.f;
		m_step = 1.f;
		std::fill(m_buffer.begin(), m_buffer.end(), 0.);
	}
	void RingBuffer::clearBuffer(){
		m_buffer.clear();
	}
	void RingBuffer::setSampleRate(float sample_rate){
		m_sample_rate = sample_rate;
	}
	float RingBuffer::read(){
		m_read += m_step;
		m_read = fmod(m_read, m_buffer_size);
		float tmp;
		float fractional;
		fractional = std::modf(m_read, &tmp);
		int int_read = static_cast<int> (tmp);
		if (int_read < 0)
			int_read = 0;
		if (int_read > m_buffer_size -2)
			int_read = m_buffer_size - 2;
			// get previous sample
			float sample1 = m_buffer[int_read];
			// get next sample
			float sample2 = m_buffer[int_read + 1];
			// linear interpolation
			float output = ((sample2 - sample1) * fractional) + sample1;
			// workaround for the bug
			if (output > 10. || output < -10.)
			{
				clearBuffer();
		}
		return output;
	}
	void RingBuffer::write(float new_sample){
		m_write += 1.f;
		m_write = fmod(m_write , m_buffer_size);
		int int_write = static_cast<int>(m_write);
		m_buffer[int_write] = new_sample;
	}
	float RingBuffer::getSize(){
		float size = m_write - m_read;
		if (m_write < m_read){
			size = m_write + (m_buffer_size - m_read);
		}
		return size;
	}
	void RingBuffer::setStep(float step){m_step = step;}
	
	void RingBuffer::setSizeWoRepitch(float size){
		m_read = 0;
		m_write = noi::Outils::convertMsToSample(size, m_sample_rate);
	}
	void RingBuffer::setSize(float new_time){
		if (m_freeze){
			m_step = m_time / new_time;
			return;
		}
		m_time = new_time;
		float size_goal = noi::Outils::convertMsToSample(new_time, m_sample_rate);
		size_goal = rack::math::clamp(size_goal, 0.f,  (m_buffer_size-2.f));
		float actual_size = getSize();
		m_actual_size = actual_size;
		//slows down if the buffer is supposed to be smaller
		//speeds up if the buffer is suppoed to be bigger
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
	float RingBuffer::getActualSize(){
		return m_actual_size;
	}
	void RingBuffer::setFreeze(bool statut){
		m_freeze = statut;
	}
}
}