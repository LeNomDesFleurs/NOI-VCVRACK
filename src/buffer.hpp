#pragma once

#include "outils.hpp"
#include <math.h>
#include <rack.hpp>

namespace noi{
namespace buffer{
class RingBuffer{

	private:
	float m_sample_rate;
	std::vector<float> m_buffer;
	float m_read{0.};
	float m_write;
	float m_buffer_size;
	float m_step{1.f};
	float m_time;
	bool m_freeze{false};
	float m_actual_size;
public:
	RingBuffer(float max_time, float initial_delay, int _sample_rate);
	void clearBuffer();
	void reset(float max_time, float initial_delay, int _sample_rate);
	float read();
	void write(float new_sample);
	void setStep(float step);
	void setSampleRate(float sample_rate);
	void setSizeWoRepitch(float size);
	void setSize(float new_time);
	float getActualSize();
	void setFreeze(bool statut);
	float getSize();
};
}
}