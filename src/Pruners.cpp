#include "plugin.hpp"
#include <vector>

struct Pruners : Module {
private:
    // float input, read_speed;
	int counter=0;

    std::vector <float> RingBuffer;

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
		configParam(PLAYSPEED_PARAM, -4.f, 4.f, 0.f, "Play");
		configParam(REC_PARAM, 0, 1, 0, "Rec");
		configParam(CLEAR_PARAM, 0, 1, 0, "Clear");
		configParam(LOOP_PARAM, 0, 1, 0, "Loop");
		configParam(PLAY_PARAM, 0, 1, 0, "Play");

		configInput(AUDIO_INPUT, "Audio");
		configInput(PLAYSPEED_CV_INPUT, "Play speed cv");

		configOutput(AUDIO_OUTPUT, "Audio");
	}


	void process(const ProcessArgs& args) override {
		bool clear_param = params[CLEAR_PARAM].getValue();
		bool play_param = params[PLAY_PARAM].getValue();
		bool loop_param = params[LOOP_PARAM].getValue();
		bool rec_param = params[REC_PARAM].getValue();

		if (not (clear_param||play_param||loop_param||rec_param)){
			counter = 0;
		}

		if (rec_param){
			if (counter == 0){
			RingBuffer.clear();
			}
			counter++;
			RingBuffer.push_back(inputs[AUDIO_INPUT].getVoltage());
		}

		if (play_param){
			outputs[AUDIO_OUTPUT].setVoltage(RingBuffer[counter]);
			counter++;
		}
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

		//
		//			INPUT
		//
	addInput(createInputCentered<PJ301MPort>(mm2px(AUDIO_INPUTpos), module, Pruners::AUDIO_INPUT));
	addOutput(createOutputCentered<PJ301MPort>(mm2px(AUDIO_OUTPUTpos), module, Pruners::AUDIO_OUTPUT));
		

	addParam(createLightParamCentered<VCVLightBezelLatch<>>(mm2px(REC_PARAMpos), module, Pruners::REC_PARAM, Pruners::REC_LIGHT));
	addParam(createLightParamCentered<VCVLightBezelLatch<>>(mm2px(PLAY_PARAMpos), module, Pruners::PLAY_PARAM, Pruners::PLAY_LIGHT));
	addParam(createLightParamCentered<VCVLightBezelLatch<>>(mm2px(CLEAR_PARAMpos), module, Pruners::CLEAR_PARAM, Pruners::CLEAR_LIGHT));
	addParam(createLightParamCentered<VCVLightBezelLatch<>>(mm2px(LOOP_PARAMpos), module, Pruners::LOOP_PARAM, Pruners::LOOP_LIGHT));
	
	}
};


Model* modelPruners = createModel<Pruners, PrunersWidget>("Pruners");