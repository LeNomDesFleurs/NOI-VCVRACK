#include "outils.hpp"
#include <vector>

class TugBuffer{

    private:
        float sample_rate;
        int size; //samples
        std::vector<float> buffer;
        int positive_counter;
        int negative_counter;
        int read_head;

    public:
        TugBuffer(int size);
        float process(std::array<float, 2> input_samples);
        void setSampleRate(float sample_rate);
};