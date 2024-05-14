#pragma once

#include "outils.hpp"
#include <math.h>
#include <rack.hpp>

namespace noi{
namespace buffer{
class RingBuffer{

private:
	float m_sample_rate{48000};
	float m_read = 0.;
	float m_write=0.1;
	float m_buffer_size=0.;
	float m_step=0.;
	float m_time=0.;
	bool m_freeze{false};
	float m_actual_size = 0.;
	std::vector<float> m_buffer;
public:
	RingBuffer(float max_time);
	void clearBuffer();
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