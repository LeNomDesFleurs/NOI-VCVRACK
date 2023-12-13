#include "outils.hpp"
#include "filter.hpp"
#include <math.h>

namespace noi{
namespace buffer{
class RingBuffer{

private:
 	float m_read=0.;
	float m_write=0.1;
	float m_buffer_size=0.;
	float m_step=0.;
	float m_time=0.;
	bool m_freeze{false};
	float m_actual_size;
	std::vector<float> m_buffer;
public:
	RingBuffer(float max_time);
	void clearBuffer();
	float read();
	void write(float new_sample);
	void setStep(float step);

	void setSizeWoRepitch(float size);
	void setSize(float new_time);
	float getActualSize();
	void setFreeze(bool statut);
	float getSize();
};
}
}