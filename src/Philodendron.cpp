/*
  ==============================================================================

    Hellebore.cpp
    Created: 11 Mar 2023 2:28:30pm
    Author:  thoma

  ==============================================================================
*/

#include "Philodendron.h"
static const float MAX_BUFFER_SIZE = 8.f;

namespace noi {
using noi::StereoRingBuffer;

Philodendron::Philodendron(noi::Philodendron::Parameters parameters, int sample_rate)
: m_ring_buffer { StereoRingBuffer(MAX_BUFFER_SIZE, 2.f, sample_rate) }
, hpf {FirstOrderFilter(20.f, 44100.f)}
, m_parameters {parameters}
, m_old_parameters {parameters}
{
  updateParameters(parameters);
  // m_allpasses[0].setGain(0.9);
  // m_allpasses[1].setGain(0.9);
}

void Philodendron::reset(noi::Philodendron::Parameters parameters, int sample_rate){
  m_ring_buffer.reset(8.f, 2.f, sample_rate);
  hpf.setSampleRate(sample_rate);
  updateParameters(parameters);
}

void Philodendron::updateParameters(noi::Philodendron::Parameters parameters) {
  m_parameters = parameters;
  // setTime();     // time
  setFreeze();   // freeze
  resize();  // read_speed + comb

  prev_offset = noi::Outils::slewValue(m_parameters.read_offset, prev_offset, 0.95);


    m_ring_buffer.setReadOffset(prev_offset);
    m_ring_buffer.active_heads = m_parameters.nb_head;
    m_ring_buffer.setHeadsReadSpeed(m_parameters.read_speed, m_parameters.head_ratio);
    // setPan();      // read_speed
    m_old_parameters = m_parameters;
}

void Philodendron::setFreeze() {
      m_ring_buffer.setFreezed(m_parameters.freeze);
}


void Philodendron::resize() {
    float time = m_parameters.comb_time;
    m_ring_buffer.setDelayTime(time);
}

void Philodendron::setSampleRate(float sample_rate) {
    m_ring_buffer.setSampleRate(sample_rate);
}


std::array<float, 5> Philodendron::process(float input) {
  
    m_outputs = m_ring_buffer.readSample();

    float feedback;
      feedback = input + (m_outputs[channel::MIX] * m_parameters.feedback);
      feedback = hpf.processhpf(feedback);
      if (feedback <= -1.f) feedback = -1.f;
      if (feedback >= 1.f) feedback = 1.f;
      if (m_outputs[channel::MIX] <= -1.f) m_outputs[channel::MIX] = -1.f;
      if (m_outputs[channel::MIX] >= 1.f) m_outputs[channel::MIX] = 1.f;

    if (!m_parameters.freeze){
      m_ring_buffer.writeSample(feedback);
    }
    
  return m_outputs;
}

}  // namespace noi
