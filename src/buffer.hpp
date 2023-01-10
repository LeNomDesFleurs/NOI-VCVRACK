#include "outils.hpp"
#include "filter.hpp"
#include <math.h>
namespace noi{
namespace buffer{
class RingBufferTest{
	private:
 	float m_read;
	int m_write;
	float m_buffer_end;
	int m_step;
	float m_time;
	std::vector<float> m_buffer;
	noi::Filter::Biquad m_aafilter{"LPF", 9000, 0.707};
	public:
	inline RingBufferTest(float time){
		float tmp = noi::Outils::convertMsToSample(time);
		m_buffer.resize(tmp);
		m_buffer_end = static_cast<float> (m_buffer.size());
		m_write = m_buffer_end;
		m_read = 0;

	}
	inline float pitchRead(){
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
	inline float read(){
		m_read = std::fmod((m_read + 1), m_buffer_end);
		int int_read = static_cast<int> (m_read);
		return m_buffer[int_read];
	}
	inline void write(float newSample){
		m_write = std::fmod((m_write + 1) , m_buffer_end);
		int int_write = static_cast<int>(m_write);
		m_buffer[int_write] = newSample;
	}
	inline float getStep(){return m_step;}
	inline void setSize(float new_time){
	//m_read_f = static_cast<int>(5.f/time, )
	m_step = m_time/new_time;
	}
	inline void setStep(int step){m_step = step;}

};
}
}