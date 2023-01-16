#include "plugin.hpp"
#include "reverb.hpp"
#include "outils.hpp"

struct Hellebore : Module {

	enum ParamId {
	SIZE_PARAM,
	VARIATION_PARAM,
	TIME_PARAM,
	DRYWET_PARAM,
	FREEZE_PARAM,

	SIZE_CV_PARAM,
	VARIATION_CV_PARAM,
	TIME_CV_PARAM,

	PARAMS_LEN
	};
	enum InputId {
	VARIATION_CV_INPUT,
	TIME_CV_INPUT,
	SIZE_CV_INPUT,
	L_INPUT,
	R_INPUT,
	INPUTS_LEN
	};
	enum OutputId {
	L_OUTPUT,
	R_OUTPUT,
	TEST_OUTPUT,
	OUTPUTS_LEN
	};
	enum LightId {
	FREEZE_LIGHT,
	LIGHTS_LEN
	};

	Hellebore() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configParam(VARIATION_PARAM, 0.f, 0.2f, 0.1, "Variation");
		configParam(SIZE_PARAM, 0.010f, 1.00f, 0.01f, "Size");
		configParam(TIME_PARAM, 0.050f, 20.f, 10.f, "Time");
		configParam(DRYWET_PARAM, 0.f, 1.f, 1.f, "Drywet");
		configParam(FREEZE_PARAM, 0, 1, 0, "Freeze");

		configParam(TIME_CV_PARAM, -1.f, 1.f, 0.f, "Time CV");
		configParam(SIZE_CV_PARAM, -1.f, 1.f, 0.f, "Size CV");
		configParam(VARIATION_CV_PARAM, -1.f, 1.f, 0.f, "Variation CV");

		configInput(VARIATION_CV_INPUT, "Variation CV");
		configInput(SIZE_CV_INPUT, "Size CV");
		configInput(TIME_CV_INPUT, "Time CV");
		configInput(L_INPUT, "");
		configOutput(L_OUTPUT, "");
		configOutput(TEST_OUTPUT, "TEST");
	}

	noi::Reverb::Moorer moorer{};
	noi::Filter::LPF SlewLPF{20};
	noi::buffer::RingBuffer test_buf {2.f};

	void process(const ProcessArgs& args) override {
		//freeze
		bool freeze_statut = params[FREEZE_PARAM].getValue() > 0;
		//buffer size
		float combTime_cv = inputs[SIZE_CV_INPUT].getVoltage()*params[SIZE_CV_PARAM].getValue()*10.f;
		combTime_cv = SlewLPF.process(combTime_cv);
		float comb_feedback_time = params[SIZE_PARAM].getValue()+combTime_cv;
		float variation = params[VARIATION_PARAM].getValue();
		//time
		float time_cv = inputs[TIME_CV_INPUT].getVoltage()*params[TIME_CV_PARAM].getValue();
		//float time = params[TIME_PARAM].getValue() + time_cv;
		float time = params[TIME_PARAM].getValue();
		//drywet
		float drywet = params[DRYWET_PARAM].getValue();
		//input
 		float input = inputs[L_INPUT].getVoltage();

		moorer.setFreeze(freeze_statut);
		moorer.resizeComb(comb_feedback_time, variation);
		moorer.setTime(time, variation);
		moorer.setDryWet(drywet);
		float output = moorer.process(input);
		



		//buffer.setFreeze(freeze_statut);
		//buffer.setSize(comb_feedback_time);
		//float output = buffer.read();
		//buffer.write(input);
		outputs[L_OUTPUT].setVoltage(output);
		// outputs[R_OUTPUT].setVoltage(test_out2);
		// outputs[TEST_OUTPUT].setVoltage(test_out);
		lights[FREEZE_LIGHT].setBrightness(freeze_statut? 1.f: 0.f);
	}
};


struct HelleboreWidget : ModuleWidget {
	HelleboreWidget(Hellebore* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Hellebore.svg")));

auto FREEZE_PARAMpos = Vec(38.294, 79.612);
auto SIZE_PARAMpos = Vec(19.319, 75.356);
auto VARIATION_CV_PARAMpos = Vec(31.357, 28.627);
auto TIME_CV_PARAMpos = Vec(43.028, 38.12);
auto VARIATION_PARAMpos = Vec(24.01, 42.546);
auto DRYWET_PARAMpos = Vec(8.198, 57.377);
auto TIME_PARAMpos = Vec(38.094, 62.24);
auto SIZE_CV_PARAMpos = Vec(38.269, 94.029);

auto VARIATION_CV_INPUTpos =Vec(26.475, 15.843);
auto L_INPUTpos =Vec(7.445, 16.123);
auto TIME_CV_INPUTpos =Vec(43.143, 16.691);
auto R_INPUTpos =Vec(17.361, 28.846);
auto SIZE_CV_INPUTpos =Vec(29.056, 109.843);

auto R_OUTPUTpos = Vec(16.739, 95.669);
auto L_OUTPUTpos = Vec(8.694, 108.391);






addParam(createParamCentered<RoundBlackKnob>(mm2px(SIZE_PARAMpos), module, Hellebore::SIZE_PARAM));
addParam(createParamCentered<Trimpot>(mm2px(VARIATION_CV_PARAMpos), module, Hellebore::VARIATION_CV_PARAM));
addParam(createParamCentered<Trimpot>(mm2px(TIME_CV_PARAMpos), module, Hellebore::TIME_CV_PARAM));
addParam(createParamCentered<RoundBlackKnob>(mm2px(VARIATION_PARAMpos), module, Hellebore::VARIATION_PARAM));
addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(TIME_PARAMpos), module, Hellebore::TIME_PARAM));
addParam(createParamCentered<RoundBlackKnob>(mm2px(DRYWET_PARAMpos), module, Hellebore::DRYWET_PARAM));
addParam(createParamCentered<Trimpot>(mm2px(SIZE_CV_PARAMpos), module, Hellebore::SIZE_CV_PARAM));
addParam(createLightParamCentered<VCVLightBezelLatch<>>(mm2px(FREEZE_PARAMpos), module, Hellebore::FREEZE_PARAM, Hellebore::FREEZE_LIGHT));


addInput(createInputCentered<PJ301MPort>(mm2px(VARIATION_CV_INPUTpos), module, Hellebore::VARIATION_CV_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(TIME_CV_INPUTpos), module, Hellebore::TIME_CV_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(L_INPUTpos), module, Hellebore::L_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(R_INPUTpos), module, Hellebore::R_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(SIZE_CV_INPUTpos), module, Hellebore::SIZE_CV_INPUT));

addOutput(createOutputCentered<PJ301MPort>(mm2px(L_OUTPUTpos), module, Hellebore::L_OUTPUT));
addOutput(createOutputCentered<PJ301MPort>(mm2px(R_OUTPUTpos), module, Hellebore::R_OUTPUT));
addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10,10)), module, Hellebore::TEST_OUTPUT));
}
};


Model* modelHellebore = createModel<Hellebore, HelleboreWidget>("Hellebore");