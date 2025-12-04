#include "tugowarbuf.hpp"

TugBuffer::TugBuffer(int size) 
        : sample_rate{48000}
        , size{size}
        , buffer(size, 0)
        , positive_counter{0}
        , negative_counter{size+1}
        {}

        float TugBuffer::process(std::array<float, 2> input_samples){

            this->positive_counter = (this->positive_counter + 1) % (this->size);
            this->read_head = (this->read_head + 1) % (this->size-200); //size offset to create evolving patterns
            
            this->negative_counter = this->negative_counter - 1;
            if (this->negative_counter < 2500)
                this->negative_counter = size;
            this->buffer[this->positive_counter] = input_samples[0];
            this->buffer[this->negative_counter] = input_samples[1];

            float output = buffer[read_head];
            return output;
        }

        void TugBuffer::setSampleRate(float sample_rate){
            this->sample_rate = sample_rate;
        }