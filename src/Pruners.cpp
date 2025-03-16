#include "plugin.hpp"
#include <vector>
#include <string>
#include <cmath>                     
#include "outils.hpp"
#include "Philodendron.h"






struct Pruners : Module {
private:
	noi::Philodendron::Parameters settings{
		false, // freeze
		0.5F,  // drywet
		0.01f, // comb time
		0.1f,  // read_speed
		0.1f,  // feedback
		1.f,   // nb_head
		1.f,   // head_ratio
		0.f	   // read_offset
	};
	noi::Philodendron philodendron;
	// float prev_offset{};
	std::array<float, 5> m_outputs{0., 0., 0., 0., 0.};

public:

	enum ParamId {
		HEAD_PARAM,
		HEAD_CV_PARAM,

		BUFFER_SIZE_PARAM,
		BUFFER_SIZE_CV_PARAM,

		RATIO_PARAM,
		RATIO_CV_PARAM,

		PLAYSPEED_PARAM,
		PLAYSPEED_CV_PARAM,

		FEEDBACK_PARAM,
		FEEDBACK_CV_PARAM,

		POSITION_PARAM,
		POSITION_CV_PARAM,

		PARAMS_LEN
	};
	enum InputId {
		HEAD_CV_INPUT,
		BUFFER_SIZE_CV_INPUT,
		RATIO_CV_INPUT,
		PLAYSPEED_CV_INPUT,
		FEEDBACK_CV_INPUT,
		POSITION_CV_INPUT,
		AUDIO_INPUT,
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
		FREEZE_LIGHT,
		LIGHTS_LEN
	};

	void onSampleRateChange(const SampleRateChangeEvent &e)override{
		// TODO
		philodendron.reset(settings, e.sampleRate);
	}

		Pruners() : philodendron{settings, 48000}
	{

		philodendron.reset(settings, APP->engine->getSampleRate());

		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(HEAD_PARAM, 1.f, 4.f, 1.f, "Heads");
		configParam(RATIO_PARAM, -4.f, 4.f, 0.f, "Ratio");
		configParam(PLAYSPEED_PARAM, -4.f, 4.f, 1.f, "Speed");
		configParam(BUFFER_SIZE_PARAM, 0.0001f, 3.9f, 1.f, "Size");
		configParam(FEEDBACK_PARAM, 0.f, 1.f, 0.f, "Feedback");
		configParam(POSITION_PARAM, -1., 1.f, 0.f, "Position");
		
		configParam(RATIO_CV_PARAM, -2.f, 2.f, 0.f, "Ratio CV Attenuverter");
		configParam(HEAD_CV_PARAM, -1.f, 1.f, 0.f, "Head CV Attenuverter");
		configParam(PLAYSPEED_CV_PARAM, -2.f, 2.f, 0.f, "Speed CV Attenuverter");
		configParam(BUFFER_SIZE_CV_PARAM, -1.4f, 1.4f, 0.f, "Size CV Attenuverter");
		configParam(FEEDBACK_CV_PARAM, -2.f, 2.f, 0.f, "Feedback CV Attenuverter");
		configParam(POSITION_CV_PARAM, -0.5, 0.5f, 0.f, "Position CV Attenuverter");

		configInput(PLAYSPEED_CV_INPUT, "Play speed CV");
		configInput(RATIO_CV_INPUT, "Ratio CV");
		configInput(BUFFER_SIZE_CV_INPUT, "Size CV");
		configInput(FEEDBACK_CV_INPUT, "Feedback CV");
		configInput(HEAD_CV_INPUT, "Head CV");
		configInput(POSITION_CV_INPUT, "Position CV");

		configInput(AUDIO_INPUT, "Audio In");
		

		configOutput(MIX_OUTPUT, "Mix");
		configOutput(HEAD1_OUTPUT, "Head 1");
		configOutput(HEAD2_OUTPUT, "Head 2");
		configOutput(HEAD3_OUTPUT, "Head 3");
		configOutput(HEAD4_OUTPUT, "Head 4");
	}

	void process(const ProcessArgs& args) override {

 float ratio_param = params[RATIO_PARAM].getValue();
 float ratio_cv = inputs[RATIO_CV_INPUT].getVoltage() * params[RATIO_CV_PARAM].getValue();
 ratio_param += ratio_cv;
 ratio_param = noi::Outils::clipValue(ratio_param, -4.f, 4.f);

 float head_param = params[HEAD_PARAM].getValue();
 float head_cv = inputs[HEAD_CV_INPUT].getVoltage() * params[HEAD_CV_PARAM].getValue();
 head_param += head_cv;
 head_param = noi::Outils::clipValue(head_param, 0.f, 4.f);

 float play_speed_param = params[PLAYSPEED_PARAM].getValue();
 float play_speed_cv = inputs[PLAYSPEED_CV_INPUT].getVoltage() * params[PLAYSPEED_CV_PARAM].getValue();
 play_speed_param += play_speed_cv;
 play_speed_param = noi::Outils::clipValue(play_speed_param, -4.f, 4.f);

 float buffer_size = params[BUFFER_SIZE_PARAM].getValue();
 float buffer_cv = params[BUFFER_SIZE_CV_PARAM].getValue() * inputs[BUFFER_SIZE_CV_INPUT].getVoltage();
 buffer_size += buffer_cv;
 buffer_size = noi::Outils::clipValue(buffer_size, 0.0001f, 3.9f);

 float feedback = params[FEEDBACK_PARAM].getValue();
 float feedback_cv = params[FEEDBACK_CV_PARAM].getValue() * inputs[FEEDBACK_CV_INPUT].getVoltage();
 feedback += feedback_cv;
 feedback = noi::Outils::clipValue(feedback, 0.f, 1.f);

 float position = params[POSITION_PARAM].getValue();
 float position_cv = params[POSITION_CV_PARAM].getValue() * inputs[POSITION_CV_INPUT].getVoltage();
 position += position_cv;
 position = noi::Outils::clipValue(position, -1.f, 1.f);

 settings.comb_time = buffer_size;
 settings.read_speed = play_speed_param - 1.f; //dirty fix
 settings.feedback = feedback;
 settings.head_ratio = ratio_param;
 settings.nb_head = head_param;
 settings.read_offset = position;
 settings.freeze = settings.feedback >= 1.0;

 philodendron.updateParameters(settings);

 float input_sample = inputs[AUDIO_INPUT].getVoltage();

 m_outputs = philodendron.process(input_sample);
 float mix = m_outputs[0];
 float output1 = m_outputs[1];
 float output2 = m_outputs[2];
 float output3 = m_outputs[3];
 float output4 = m_outputs[4];

 outputs[MIX_OUTPUT].setVoltage(mix);
 outputs[HEAD1_OUTPUT].setVoltage(output1);
 outputs[HEAD2_OUTPUT].setVoltage(output2);
 outputs[HEAD3_OUTPUT].setVoltage(output3);
 outputs[HEAD4_OUTPUT].setVoltage(output4);

 lights[FREEZE_LIGHT].setBrightness(settings.freeze);


	}
};


struct PrunersWidget :
				ModuleWidget{
					PrunersWidget(Pruners * module){
						setModule(module);
				setPanel(createPanel(asset::plugin(pluginInstance, "res/Pruners.svg")));
//      ./SVG.py createmodule position res/Pruners.svg  
		
auto POSITION_CV_PARAMpos = Vec(8.871, 17.076);
auto PLAYSPEED_CV_PARAMpos = Vec(77.848, 17.076);
auto POSITION_PARAMpos = Vec(25.445, 26.868);
auto PLAYSPEED_PARAMpos = Vec(60.551, 26.733);
auto FEEDBACK_CV_PARAMpos = Vec(52.743, 44.965);
auto BUFFER_SIZE_CV_PARAMpos = Vec(32.931, 44.965);
auto BUFFER_SIZE_PARAMpos = Vec(14.281, 53.654);
auto FEEDBACK_PARAMpos = Vec(72.773, 54.417);
auto RATIO_CV_PARAMpos = Vec(6.666, 77.953);
auto HEAD_CV_PARAMpos = Vec(79.365, 77.483);
auto RATIO_PARAMpos = Vec(28.275, 84.119);
auto HEAD_PARAMpos = Vec(57.748, 84.069);

auto FEEDBACK_CV_INPUTpos =Vec(52.886, 60.367);
auto BUFFER_SIZE_CV_INPUTpos =Vec(33.153, 60.367);
auto AUDIO_INPUTpos =Vec(43.159, 17.338);
auto POSITION_CV_INPUTpos =Vec(7.3, 32.178);
auto PLAYSPEED_CV_INPUTpos =Vec(79.026, 32.178);
auto RATIO_CV_INPUTpos =Vec(6.666, 91.777);
auto HEAD_CV_INPUTpos =Vec(79.365, 91.308);

auto HEAD1_OUTPUTpos = Vec(8.02, 108.341);
auto HEAD4_OUTPUTpos = Vec(78.613, 108.153);
auto HEAD2_OUTPUTpos = Vec(24.521, 111.569);
auto HEAD3_OUTPUTpos = Vec(62.484, 111.51);
auto MIX_OUTPUTpos = Vec(43.18, 112.301);

auto FREEZE_LIGHTpos = Vec(78.714, 65.004);



addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(POSITION_PARAMpos), module, Pruners::POSITION_PARAM));
addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(PLAYSPEED_PARAMpos), module, Pruners::PLAYSPEED_PARAM));
addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(FEEDBACK_PARAMpos), module, Pruners::FEEDBACK_PARAM));
addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(RATIO_PARAMpos), module, Pruners::RATIO_PARAM));
addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(HEAD_PARAMpos), module, Pruners::HEAD_PARAM));
addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(BUFFER_SIZE_PARAMpos), module, Pruners::BUFFER_SIZE_PARAM));

addParam(createParamCentered<Trimpot>(mm2px(POSITION_CV_PARAMpos), module, Pruners::POSITION_CV_PARAM));
addParam(createParamCentered<Trimpot>(mm2px(PLAYSPEED_CV_PARAMpos), module, Pruners::PLAYSPEED_CV_PARAM));
addParam(createParamCentered<Trimpot>(mm2px(FEEDBACK_CV_PARAMpos), module, Pruners::FEEDBACK_CV_PARAM));
addParam(createParamCentered<Trimpot>(mm2px(BUFFER_SIZE_CV_PARAMpos), module, Pruners::BUFFER_SIZE_CV_PARAM));
addParam(createParamCentered<Trimpot>(mm2px(RATIO_CV_PARAMpos), module, Pruners::RATIO_CV_PARAM));
addParam(createParamCentered<Trimpot>(mm2px(HEAD_CV_PARAMpos), module, Pruners::HEAD_CV_PARAM));

addInput(createInputCentered<PJ301MPort>(mm2px(FEEDBACK_CV_INPUTpos), module, Pruners::FEEDBACK_CV_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(BUFFER_SIZE_CV_INPUTpos), module, Pruners::BUFFER_SIZE_CV_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(POSITION_CV_INPUTpos), module, Pruners::POSITION_CV_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(PLAYSPEED_CV_INPUTpos), module, Pruners::PLAYSPEED_CV_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(RATIO_CV_INPUTpos), module, Pruners::RATIO_CV_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(HEAD_CV_INPUTpos), module, Pruners::HEAD_CV_INPUT));

addInput(createInputCentered<PJ301MPort>(mm2px(AUDIO_INPUTpos), module, Pruners::AUDIO_INPUT));

addOutput(createOutputCentered<PJ301MPort>(mm2px(HEAD1_OUTPUTpos), module, Pruners::HEAD1_OUTPUT));
addOutput(createOutputCentered<PJ301MPort>(mm2px(HEAD4_OUTPUTpos), module, Pruners::HEAD4_OUTPUT));
addOutput(createOutputCentered<PJ301MPort>(mm2px(MIX_OUTPUTpos), module, Pruners::MIX_OUTPUT));
addOutput(createOutputCentered<PJ301MPort>(mm2px(HEAD3_OUTPUTpos), module, Pruners::HEAD3_OUTPUT));
addOutput(createOutputCentered<PJ301MPort>(mm2px(HEAD2_OUTPUTpos), module, Pruners::HEAD2_OUTPUT));

addChild(createLightCentered<SmallLight<WhiteLight>>(mm2px(FREEZE_LIGHTpos), module, Pruners::FREEZE_LIGHT));

	}
};


Model* modelPruners = createModel<Pruners, PrunersWidget>("Pruners");