#include "plugin.hpp"
#include <vector>
#include <string>
#include <cmath>                     
#include "outils.hpp"
#include "Pruners.hpp"

struct Pruners : Module {
private:
    // float input, read_speed;
	float counter = 0;

    // std::vector <float> RingBuffer = {0.};
	PrunersRingBuffer ring_buffer = PrunersRingBuffer(20 * 48000);

	// void fadeBuffer(){
	// 	float fade_time = 1000;
	// 	for (int i = fade_time; i >= 0; i--){
	// 		RingBuffer[i]=RingBuffer[i]*(i/fade_time)+RingBuffer[buffer_size-(fade_time-i)]*(1-(i/fade_time));
	// 		RingBuffer.pop_back();
	// 	}
	// 	for (int i = 0; i < 4; i++){
	// 	counter[i]=0;
	// 	}
	// }
enum Statut{recording, playing, thru};
bool rec_end;


public:

	enum ParamId {
		HEAD_PARAM,
		RATIO_PARAM,
		HEAD_CV_PARAM,
		RATIO_CV_PARAM,
		PLAYSPEED_PARAM,
		PLAYSPEED_CV_PARAM,
		REC_PARAM,
		BUFFER_SIZE_PARAM,
		BUFFER_SIZE_CV_PARAM,
		LOCK_PARAM,
		// CLEAR_PARAM,
		// LOOP_PARAM,
		PLAY_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		HEAD_CV_INPUT,
		REC_CV_INPUT,
		BUFFER_SIZE_CV_INPUT,
		RATIO_CV_INPUT,
		AUDIO_INPUT,
		PLAYSPEED_CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		HEAD1_OUTPUT,
		HEAD2_OUTPUT,
		HEAD3_OUTPUT,
		HEAD4_OUTPUT,
		MIX_OUTPUT,
		DEBUG_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		REC_LIGHT,
		LOCK_LIGHT,
		// CLEAR_LIGHT,
		// LOOP_LIGHT,
		PLAY_LIGHT,
		LIGHTS_LEN
	};

	Pruners() {
		ring_buffer.clearBuffer();
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(HEAD_PARAM, 1.f, 4.f, 1.f, "number of head in mix");
		configParam(HEAD_CV_PARAM, -2.f, 2.f, 0.f, "Head CV Attenuverter");
		configParam(BUFFER_SIZE_CV_PARAM, -2.f, 2.f, 0.f, "Buffer Size CV Attenuverter");
		configParam(RATIO_CV_PARAM, -2.f, 2.f, 0.f, "Ratio CV Attenuverter");
		configParam(RATIO_PARAM, -4.f, 4.f, 1.f, "ratio");
		configParam(PLAYSPEED_PARAM, -4.f, 4.f, 1.f, "Playspeed");
		configParam(PLAYSPEED_CV_PARAM, -1.f, 1.f, 1.f, "Playspeed CV Attenuverter");
		configParam(REC_PARAM, 0, 1, 0, "Freeze");
		configParam(BUFFER_SIZE_PARAM, 1, 19, 2, "Buffer Size");
		// configParam(CLEAR_PARAM, 0, 1, 0, "Clear");
		// configParam(LOOP_PARAM, 0, 1, 0, "Loop");
		// configParam(PLAY_PARAM, 0, 1, 0, "Play");

		configInput(AUDIO_INPUT, "Audio");
		configInput(PLAYSPEED_CV_INPUT, "Play speed cv");
		configParam(LOCK_PARAM, 0, 1, 0, "Lock");

		configOutput(MIX_OUTPUT, "Audio");
		configOutput(HEAD1_OUTPUT, "head 1");
		configOutput(HEAD2_OUTPUT, "head 2");
		configOutput(HEAD3_OUTPUT, "head 3");
		configOutput(HEAD4_OUTPUT, "head 4");

	}


	void process(const ProcessArgs& args) override {

		
		float ratio_param = params[RATIO_PARAM].getValue();
		float head_param = params[HEAD_PARAM].getValue();
		float head_cv_param = params[HEAD_CV_PARAM].getValue();
		float head_cv = inputs[HEAD_CV_INPUT].getVoltage();

		head_param += head_cv * head_cv_param;

		float ratio_value = ratio_param + (inputs[RATIO_CV_INPUT].getVoltage() * params[RATIO_CV_PARAM].getValue());

		float play_speed_value = params[PLAYSPEED_PARAM].getValue() + (inputs[PLAYSPEED_CV_INPUT].getVoltage() * params[PLAYSPEED_CV_PARAM].getValue());

		float buffer_size = params[BUFFER_SIZE_PARAM].getValue() * 48000;
		float buffer_cv = params[BUFFER_SIZE_CV_PARAM].getValue() * inputs[BUFFER_SIZE_CV_INPUT].getVoltage();
		buffer_size += buffer_cv;
		// buffer_size = noi::Outils::clip(buffer_size, 1.f, 20.f);

		ring_buffer.setParameters(play_speed_value, ratio_value, buffer_size, params[LOCK_PARAM].getValue());

		float output = 0;
		float debug = 0.;

		// buffer_size=static_cast<float>(RingBuffer.size());

		//reset counter
		if (not params[REC_PARAM].getValue()){
			counter = 0;
			ring_buffer.writeSample(inputs[AUDIO_INPUT].getVoltage());
		}
			for(int i = 0; i < 4 ; i++){
			float sample = ring_buffer.readHead(i);
			output += sample * (i <= head_param);
			outputs[i].setVoltage(sample);
			}
			output /= 4.;

		//Fade end of buffer on beginning of buffer
		//executer a chaque sample ???
		if (rec_end && buffer_size > 1000){
			rec_end=false;
		}		

		// outputs[0].setVoltage(recording);
		// outputs[1].setVoltage(playing);
		outputs[MIX_OUTPUT].setVoltage(output);
		outputs[DEBUG_OUTPUT].setVoltage(debug);
		// lights[PLAY_LIGHT].setBrightness(playing);
		lights[REC_LIGHT].setBrightness(params[REC_PARAM].getValue());
		lights[LOCK_LIGHT].setBrightness(params[LOCK_PARAM].getValue());
		// lights[LOOP_LIGHT].setBrightness(loop_param);
		// lights[CLEAR_LIGHT].setBrightness(clear_param);

	}
};


struct PrunersWidget :
				ModuleWidget{
					PrunersWidget(Pruners * module){
						setModule(module);
				setPanel(createPanel(asset::plugin(pluginInstance, "res/Pruners.svg")));
//      ./SVG.py createmodule position res/Pruners.svg  
				

auto REC_PARAMpos = Vec(5.189, 29.906);
auto BUFFER_SIZE_CV_PARAMpos = Vec(45.333, 29.906);
auto PLAYSPEED_CV_PARAMpos = Vec(25.302, 34.404);
auto BUFFER_SIZE_PARAMpos = Vec(41.944, 40.413);
auto PLAYSPEED_PARAMpos = Vec(25.302, 50.361);
auto HEAD_PARAMpos = Vec(9.98, 76.123);
auto RATIO_PARAMpos = Vec(40.608, 76.123);
auto HEAD_CV_PARAMpos = Vec(16.242, 92.089);
auto RATIO_CV_PARAMpos = Vec(34.349, 92.089);

auto REC_CV_INPUTpos =Vec(9.458, 16.861);
auto PLAY_CV_INPUTpos =Vec(41.064, 16.861);
auto PLAYSPEED_CV_INPUTpos =Vec(25.261, 20.415);
auto AUDIO_INPUTpos =Vec(25.375, 67.609);
auto HEAD_CV_INPUTpos =Vec(5.817, 100.242);
auto RATIO_CV_INPUTpos =Vec(44.934, 100.242);

auto HEAD1_OUTPUTpos = Vec(5.817, 112.03);
auto HEAD2_OUTPUTpos = Vec(15.596, 112.03);
auto MIX_OUTPUTpos = Vec(25.375, 112.03);
auto HEAD3_OUTPUTpos = Vec(35.155, 112.03);
auto HEAD4_OUTPUTpos = Vec(44.934, 112.03);

auto LOCK_PARAM_PARAMpos = Vec(42.857, 58.902);

				addParam(createParamCentered<Trimpot>(mm2px(PLAYSPEED_CV_PARAMpos), module, Pruners::PLAYSPEED_CV_PARAM));
				addParam(createParamCentered<Trimpot>(mm2px(RATIO_CV_PARAMpos), module, Pruners::RATIO_CV_PARAM));
				addParam(createParamCentered<Trimpot>(mm2px(HEAD_CV_PARAMpos), module, Pruners::HEAD_CV_PARAM));

				addParam(createParamCentered<Trimpot>(mm2px(BUFFER_SIZE_CV_PARAMpos), module, Pruners::BUFFER_SIZE_CV_PARAM));
				addParam(createParamCentered<RoundBlackKnob>(mm2px(BUFFER_SIZE_PARAMpos), module, Pruners::BUFFER_SIZE_PARAM));

				addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(PLAYSPEED_PARAMpos), module, Pruners::PLAYSPEED_PARAM));
				addParam(createParamCentered<RoundBlackKnob>(mm2px(HEAD_PARAMpos), module, Pruners::HEAD_PARAM));
				addParam(createParamCentered<RoundBlackKnob>(mm2px(RATIO_PARAMpos), module, Pruners::RATIO_PARAM));
				addParam(createLightParamCentered<VCVLightBezelLatch<>>(mm2px(REC_PARAMpos), module, Pruners::REC_PARAM, Pruners::REC_LIGHT));
				addParam(createLightParamCentered<VCVLightBezelLatch<>>(mm2px(LOCK_PARAM_PARAMpos), module, Pruners::LOCK_PARAM, Pruners::LOCK_LIGHT));
				// addParam(createLightParamCentered<VCVLightBezelLatch<>>(mm2px(PLAY_PARAMpos), module, Pruners::PLAY_PARAM, Pruners::PLAY_LIGHT));

				// addInput(createInputCentered<PJ301MPort>(mm2px(Q_CV_INPUTpos), module, Pruners::Q_CV_INPUT));
				// addInput(createInputCentered<PJ301MPort>(mm2px(Q_CV_INPUTpos), module, Pruners::Q_CV_INPUT));
				addInput(createInputCentered<PJ301MPort>(mm2px(PLAYSPEED_CV_INPUTpos), module, Pruners::PLAYSPEED_CV_INPUT));
				addInput(createInputCentered<PJ301MPort>(mm2px(AUDIO_INPUTpos), module, Pruners::AUDIO_INPUT));
				addInput(createInputCentered<PJ301MPort>(mm2px(RATIO_CV_INPUTpos), module, Pruners::RATIO_CV_INPUT));
				addInput(createInputCentered<PJ301MPort>(mm2px(HEAD_CV_INPUTpos), module, Pruners::HEAD_CV_INPUT));
				addInput(createInputCentered<PJ301MPort>(mm2px(REC_CV_INPUTpos), module, Pruners::REC_CV_INPUT));
				addInput(createInputCentered<PJ301MPort>(mm2px(PLAY_CV_INPUTpos), module, Pruners::BUFFER_SIZE_CV_INPUT));

				addOutput(createOutputCentered<PJ301MPort>(mm2px(MIX_OUTPUTpos), module, Pruners::MIX_OUTPUT));
				addOutput(createOutputCentered<PJ301MPort>(mm2px(HEAD1_OUTPUTpos), module, Pruners::HEAD1_OUTPUT));
				addOutput(createOutputCentered<PJ301MPort>(mm2px(HEAD2_OUTPUTpos), module, Pruners::HEAD2_OUTPUT));
				addOutput(createOutputCentered<PJ301MPort>(mm2px(HEAD3_OUTPUTpos), module, Pruners::HEAD3_OUTPUT));
				addOutput(createOutputCentered<PJ301MPort>(mm2px(HEAD4_OUTPUTpos), module, Pruners::HEAD4_OUTPUT));
	}
};


Model* modelPruners = createModel<Pruners, PrunersWidget>("Pruners");