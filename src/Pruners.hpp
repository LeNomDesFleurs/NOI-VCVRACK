#include <vector>
#include "outils.hpp"

class PrunersRingBuffer{
    private:
        float counter[4] = {0, 0, 0, 0};
        float play_speed[4] = {1, 1, 1, 1};
        float write_index = 0;
        std::vector<float> buffer;
        float buffer_max_size;
        float buffer_size;

    public:
        PrunersRingBuffer(float setmaxSize){
            buffer_max_size = setmaxSize;
            buffer.resize(buffer_max_size);
            buffer_max_size--;
        }

        void setParameters(float play_speed_value, float ratio, float buffer_size_value, bool lock)
        {
            if (lock){
                play_speed_value *= 4.f;
                play_speed_value = noi::Outils::truncate(play_speed_value);
                play_speed_value /= 4.f;
                ratio *= 4.f;
                ratio = noi::Outils::truncate(ratio);
                ratio /= 4.f;
            }
            play_speed[0] = play_speed_value;
		    for (int i=1; i<4; i++){
		    play_speed[i]=play_speed[i-1]*ratio;}
            buffer_size = buffer_size_value;
        }

        void writeSample(float sample){
            write_index = noi::Outils::modulo(write_index + 1, buffer_size);
            buffer[write_index] = sample;
        }

        void clearBuffer(){
            buffer.clear();
            }
            float readSample(float &headCounter, float play_speed)
            {
            float min = noi::Outils::modulo(write_index - buffer_size, buffer_max_size);
            // float max = write_index;
            //fonctionnement par difference, write et read index (write - size) sont les extremums auxquels j'ajoute les conteurs de chaque tete
            headCounter = noi::Outils::modulo(headCounter + play_speed, buffer_size);

            float index = noi::Outils::modulo(min + headCounter, buffer_max_size);

            
            float prev_sample = noi::Outils::modulo(noi::Outils::truncate(index), buffer_size - 1.);
            float next_sample = noi::Outils::modulo((prev_sample + 1.), buffer_size - 1.);
		    float coef = noi::Outils::decimal(index);
		    float out = (buffer[next_sample] * coef) + (buffer[prev_sample] * (1. - coef));
		    return out;
            }

        float readHead(int headIndex){
            return readSample(counter[headIndex], play_speed[headIndex]);
        }
};