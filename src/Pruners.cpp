#include "plugin.hpp"
#include <vector>
#include <cmath>
#include "outils.hpp"

struct Pruners : Module {
private:
    // float input, read_speed;
	float counter=0;

    std::vector <float> RingBuffer;
	float buffer_size;
	bool rec = false;


float readSample(float index){
	float prev_sample=noi::Outils::modulo(noi::Outils::truncate(index), buffer_size-1.);
	float next_sample=noi::Outils::modulo((prev_sample+1.), buffer_size-1.);
	float coef= noi::Outils::decimal(index);
    float out=(RingBuffer[next_sample]*coef) + (RingBuffer[prev_sample]*(1.-coef));
	return out;
}

public:

	enum ParamId {
		PLAYSPEED_PARAM,
		REC_PARAM,
		CLEAR_PARAM,
		LOOP_PARAM,
		PLAY_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		AUDIO_INPUT,
		PLAYSPEED_CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		AUDIO_OUTPUT,
		DEBUG_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		REC_LIGHT,
		CLEAR_LIGHT,
		LOOP_LIGHT,
		PLAY_LIGHT,
		LIGHTS_LEN
	};

	Pruners() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PLAYSPEED_PARAM, -4.f, 4.f, 1.f, "Playspeed");
		configParam(REC_PARAM, 0, 1, 0, "Rec");
		configParam(CLEAR_PARAM, 0, 1, 0, "Clear");
		configParam(LOOP_PARAM, 0, 1, 0, "Loop");
		configParam(PLAY_PARAM, 0, 1, 0, "Play");

		configInput(AUDIO_INPUT, "Audio");
		configInput(PLAYSPEED_CV_INPUT, "Play speed cv");

		configOutput(AUDIO_OUTPUT, "Audio");
	}


	void process(const ProcessArgs& args) override {
		bool rec_param = params[REC_PARAM].getValue();
		bool clear_param = params[CLEAR_PARAM].getValue();
		bool loop_param = params[LOOP_PARAM].getValue();
		bool play_param = params[PLAY_PARAM].getValue();

		float play_speed= params[PLAYSPEED_PARAM].getValue();
		float output = 0;
		float debug =0.;

		buffer_size=static_cast<float>(RingBuffer.size());


		if (buffer_size<1){
			output = inputs[AUDIO_INPUT].getVoltage();
		}

		if (not (clear_param||play_param||rec_param)){
			counter = 0;
		}

		if (rec==true && rec_param==false && buffer_size > 1000){
		 	float fade_time = 1000;
			for (int i = fade_time; i >= 0; i--){
				RingBuffer[i]=RingBuffer[i]*(i/fade_time)+RingBuffer[buffer_size-(fade_time-i)]*(1-(i/fade_time));
				RingBuffer.pop_back();
			}

			rec=false;
		}

		if (rec_param){
			rec=true;
			params[PLAY_PARAM].setValue(0);
			if (counter == 0){
			RingBuffer.clear();
			}
			counter++;
			RingBuffer.push_back(inputs[AUDIO_INPUT].getVoltage());
		}
		
		if (play_param){
			params[REC_PARAM].setValue(0);
    		output=readSample(counter);
			counter+=play_speed;
			if (counter > buffer_size && not loop_param){
					params[PLAY_PARAM].setValue(0);
			}
			counter=noi::Outils::modulo(counter, buffer_size);
		}

		else {output = inputs[AUDIO_INPUT].getVoltage();}

			outputs[AUDIO_OUTPUT].setVoltage(output);
			outputs[DEBUG_OUTPUT].setVoltage(debug);
			lights[PLAY_LIGHT].setBrightness(play_param);
			lights[REC_LIGHT].setBrightness(rec_param);
			lights[LOOP_LIGHT].setBrightness(loop_param);
			lights[CLEAR_LIGHT].setBrightness(clear_param);

	}
};


struct PrunersWidget : ModuleWidget {
	PrunersWidget(Pruners* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Pruners.svg")));

		auto REC_PARAMpos = Vec(6.435, 29.906);
		auto PLAY_PARAMpos = Vec(20.333, 75.906);
		auto CLEAR_PARAMpos = Vec(12.824, 50.361);
		auto LOOP_PARAMpos = Vec(20.824, 55.361);
		auto AUDIO_INPUTpos = Vec(5, 5);
		auto AUDIO_OUTPUTpos = Vec(20, 100);
		auto DEBUG_OUTPUTpos = Vec(30, 110);
		auto PLAYSPEED_PARAMpos = Vec(30, 10);
		//
		//			INPUT
		//
	addInput(createInputCentered<PJ301MPort>(mm2px(AUDIO_INPUTpos), module, Pruners::AUDIO_INPUT));

	//OUTPUT
	addOutput(createOutputCentered<PJ301MPort>(mm2px(DEBUG_OUTPUTpos), module, Pruners::DEBUG_OUTPUT));
	addOutput(createOutputCentered<PJ301MPort>(mm2px(AUDIO_OUTPUTpos), module, Pruners::AUDIO_OUTPUT));
		
	//PARAM
	addParam(createParamCentered<RoundBlackKnob>(mm2px(PLAYSPEED_PARAMpos), module, Pruners::PLAYSPEED_PARAM));
	addParam(createLightParamCentered<VCVLightBezelLatch<>>(mm2px(REC_PARAMpos), module, Pruners::REC_PARAM, Pruners::REC_LIGHT));
	addParam(createLightParamCentered<VCVLightBezelLatch<>>(mm2px(PLAY_PARAMpos), module, Pruners::PLAY_PARAM, Pruners::PLAY_LIGHT));
	addParam(createLightParamCentered<VCVLightBezelLatch<>>(mm2px(CLEAR_PARAMpos), module, Pruners::CLEAR_PARAM, Pruners::CLEAR_LIGHT));
	addParam(createLightParamCentered<VCVLightBezelLatch<>>(mm2px(LOOP_PARAMpos), module, Pruners::LOOP_PARAM, Pruners::LOOP_LIGHT));
	
	}
};


Model* modelPruners = createModel<Pruners, PrunersWidget>("Pruners");