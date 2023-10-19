#include "plugin.hpp"
#include <vector>
#include <cmath>
#include "outils.hpp"

struct Pruners : Module {
private:
    // float input, read_speed;
	float counter[4]={0.,0.,0.,0.};

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
		HEAD_PARAM,
		RATIO_PARAM,
		HEAD_CV_PARAM,
		RATIO_CV_PARAM,
		PLAYSPEED_PARAM,
		REC_PARAM,
		CLEAR_PARAM,
		LOOP_PARAM,
		PLAY_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		HEAD_CV_INPUT,
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
		CLEAR_LIGHT,
		LOOP_LIGHT,
		PLAY_LIGHT,
		LIGHTS_LEN
	};

	Pruners() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(HEAD_PARAM, 1.f, 4.f, 1.f, "ratio");
		configParam(RATIO_PARAM, -4.f, 4.f, 1.f, "ratio");
		configParam(PLAYSPEED_PARAM, -4.f, 4.f, 1.f, "Playspeed");
		configParam(REC_PARAM, 0, 1, 0, "Rec");
		configParam(CLEAR_PARAM, 0, 1, 0, "Clear");
		configParam(LOOP_PARAM, 0, 1, 0, "Loop");
		configParam(PLAY_PARAM, 0, 1, 0, "Play");

		configInput(AUDIO_INPUT, "Audio");
		configInput(PLAYSPEED_CV_INPUT, "Play speed cv");

		configOutput(MIX_OUTPUT, "Audio");
	}


	void process(const ProcessArgs& args) override {
		bool rec_param = params[REC_PARAM].getValue();
		bool clear_param = params[CLEAR_PARAM].getValue();
		bool loop_param = params[LOOP_PARAM].getValue();
		bool play_param = params[PLAY_PARAM].getValue();
		float ratio_param = params[RATIO_PARAM].getValue();
		float head_param = params[HEAD_PARAM].getValue();

		float play_speed[4];
		play_speed[0] = params[PLAYSPEED_PARAM].getValue();
		for (int i=1; i<4; i++){
		play_speed[i]=play_speed[i-1]*ratio_param;
		}
		float output = 0;
		float debug =0.;

		buffer_size=static_cast<float>(RingBuffer.size());


		if (buffer_size<1){
			output = inputs[AUDIO_INPUT].getVoltage();
		}

		if (not (clear_param||play_param||rec_param)){
			counter[0] = 0;
		}


		//Fade end of buffer on beginning of bugger
		if (rec==true && rec_param==false && buffer_size > 1000){
		 	float fade_time = 1000;
			for (int i = fade_time; i >= 0; i--){
				RingBuffer[i]=RingBuffer[i]*(i/fade_time)+RingBuffer[buffer_size-(fade_time-i)]*(1-(i/fade_time));
				RingBuffer.pop_back();
			}
			for (int i = 0; i < 4; i++){
			counter[i]=0;
			}

			rec=false;
		}
		//rec
		if (rec_param){
			rec=true;
			params[PLAY_PARAM].setValue(0);
			if (counter[0] == 0){
			RingBuffer.clear();
			}
			counter[0]++;
			RingBuffer.push_back(inputs[AUDIO_INPUT].getVoltage());
		}
		//play
		if (play_param){
			params[REC_PARAM].setValue(0);
			for(int i = 0; i < 4 ; i++){
			float sample = readSample(counter[i]);
			output += sample * (i <= head_param);
			outputs[i].setVoltage(sample);
			counter[i] += play_speed[i];
			counter[i]=noi::Outils::modulo(counter[i], buffer_size);
			}
		}
		else {output = inputs[AUDIO_INPUT].getVoltage();}

		output /= 4.;

			outputs[MIX_OUTPUT].setVoltage(output);
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

auto REC_PARAMpos = Vec(-5.189, 29.906);
auto PLAY_PARAMpos = Vec(45.333, 29.906);
auto PLAYSPEED_CV_PARAMpos = Vec(25.302, 34.404);
auto PLAYSPEED_PARAMpos = Vec(25.302, 50.361);
auto RATIO_PARAMpos = Vec(40.618, 58.945);
auto HEAD_PARAMpos = Vec(10.15, 59.465);
auto RATIO_CV_PARAMpos = Vec(45.333, 76.6);
auto HEAD_CV_PARAMpos = Vec(6.109, 77.24);

auto Q_CV_INPUTpos =Vec(-9.458, 16.861);
auto Q_CV_INPUTpos =Vec(41.064, 16.861);
auto PLAYSPEED_CV_INPUTpos =Vec(25.261, 20.415);
auto AUDIO_INPUTpos =Vec(25.375, 67.609);
auto RATIO_CV_INPUTpos =Vec(38.718, 92.531);
auto HEAD_CV_INPUTpos =Vec(13.154, 93.17);

auto HEAD1_OUTPUTpos = Vec(5.817, 112.03);
auto HEAD2_OUTPUTpos = Vec(15.596, 112.03);
auto AUDIO_OUTPUTpos = Vec(25.375, 112.03);
auto HEAD3_OUTPUTpos = Vec(35.155, 112.03);
auto HEAD4_OUTPUTpos = Vec(44.934, 112.03);




addParam(createParamCentered<RoundBlackKnob>(mm2px(REC_PARAMpos), module, Pruners::REC_PARAM));
addParam(createParamCentered<RoundBlackKnob>(mm2px(PLAY_PARAMpos), module, Pruners::PLAY_PARAM));
addParam(createParamCentered<RoundBlackKnob>(mm2px(PLAYSPEED_CV_PARAMpos), module, Pruners::PLAYSPEED_CV_PARAM));
addParam(createParamCentered<RoundBlackKnob>(mm2px(PLAYSPEED_PARAMpos), module, Pruners::PLAYSPEED_PARAM));
addParam(createParamCentered<RoundBlackKnob>(mm2px(RATIO_PARAMpos), module, Pruners::RATIO_PARAM));
addParam(createParamCentered<RoundBlackKnob>(mm2px(HEAD_PARAMpos), module, Pruners::HEAD_PARAM));
addParam(createParamCentered<RoundBlackKnob>(mm2px(RATIO_CV_PARAMpos), module, Pruners::RATIO_CV_PARAM));
addParam(createParamCentered<RoundBlackKnob>(mm2px(HEAD_CV_PARAMpos), module, Pruners::HEAD_CV_PARAM));

addInput(createInputCentered<PJ301MPort>(mm2px(Q_CV_INPUTpos), module, Pruners::Q_CV_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(Q_CV_INPUTpos), module, Pruners::Q_CV_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(PLAYSPEED_CV_INPUTpos), module, Pruners::PLAYSPEED_CV_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(AUDIO_INPUTpos), module, Pruners::AUDIO_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(RATIO_CV_INPUTpos), module, Pruners::RATIO_CV_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(HEAD_CV_INPUTpos), module, Pruners::HEAD_CV_INPUT));

addOutput(createOutputCentered<PJ301MPort>(mm2px(AUDIO_OUTPUTpos), module, Pruners::MIX_OUTPUT)
addOutput(createOutputCentered<PJ301MPort>(mm2px(HEAD1_OUTPUTpos), module, Pruners::HEAD1_OUTPUT));
addOutput(createOutputCentered<PJ301MPort>(mm2px(HEAD2_OUTPUTpos), module, Pruners::HEAD2_OUTPUT));
addOutput(createOutputCentered<PJ301MPort>(mm2px(HEAD3_OUTPUTpos), module, Pruners::HEAD3_OUTPUT));
addOutput(createOutputCentered<PJ301MPort>(mm2px(HEAD4_OUTPUTpos), module, Pruners::HEAD4_OUTPUT));
	}
};


Model* modelPruners = createModel<Pruners, PrunersWidget>("Pruners");