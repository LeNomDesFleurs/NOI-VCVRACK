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
	SIGNAL_INPUT,
	INPUTS_LEN
	};
	enum OutputId {
	MIX_OUTPUT,
	TEST_OUTPUT,
	OUTPUTS_LEN
	};
	enum LightId {
	FREEZE_LIGHT,
	LIGHTS_LEN
	};

	Hellebore() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configParam(VARIATION_PARAM, 0.f, 1.f, 0.f, "Variation");
		configParam(SIZE_PARAM, 0.003, 0.200f, 0.01f, "Size");
		configParam(TIME_PARAM, 0.010f, 20.f, 0.020f, "Time");
		configParam(DRYWET_PARAM, 0.f, 1.f, 1.f, "Drywet");
		configParam(FREEZE_PARAM, 0, 1, 0, "Freeze");

		configParam(TIME_CV_PARAM, -1.f, 1.f, 0.f, "Time CV");
		configParam(SIZE_CV_PARAM, -1.f, 1.f, 0.f, "Size CV");
		configParam(VARIATION_CV_PARAM, -1.f, 1.f, 0.f, "Variation CV");

		configInput(VARIATION_CV_INPUT, "Variation CV");
		configInput(SIZE_CV_INPUT, "Size CV");
		configInput(TIME_CV_INPUT, "Time CV");
		configInput(SIGNAL_INPUT, "");
		configOutput(MIX_OUTPUT, "");
		configOutput(TEST_OUTPUT, "TEST");
	}

	noi::Reverb::Moorer moorer{};
	noi::Filter::LPF SlewLPF{20};
	float time;
	float new_time;

	void process(const ProcessArgs& args) override {
		bool freeze_statut = params[FREEZE_PARAM].getValue()>0;
		float combTime_cv = inputs[SIZE_CV_INPUT].getVoltage()*params[SIZE_CV_PARAM].getValue()*10.f;
		combTime_cv = SlewLPF.process(combTime_cv);
		float time_cv = inputs[TIME_CV_INPUT].getVoltage()*params[TIME_CV_PARAM].getValue();
		float comb_feedback_time = params[SIZE_PARAM].getValue()+combTime_cv;
		float variation = params[VARIATION_PARAM].getValue();
		float time = params[TIME_PARAM].getValue() + time_cv;
		float input = inputs[SIGNAL_INPUT].getVoltage();
		float drywet = params[DRYWET_PARAM].getValue();

		moorer.setFreeze(freeze_statut);
		moorer.resizeComb(comb_feedback_time, variation);
		moorer.setParam(time, variation);
		moorer.setDryWet(drywet);
		float output = moorer.process(input);
		outputs[MIX_OUTPUT].setVoltage(output);
		outputs[TEST_OUTPUT].setVoltage(moorer.getDryWet());
		lights[FREEZE_LIGHT].setBrightness(freeze_statut? 1.f: 0.f);
	}
};


struct HelleboreWidget : ModuleWidget {
	HelleboreWidget(Hellebore* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Hellebore.svg")));

auto FREEZE_PARAMpos = Vec(22.918, 98.876);
auto SIZE_PARAMpos = Vec(26.58, 84.331);
auto VARIATION_CV_PARAMpos = Vec(33.994, 25.946);
auto TIME_CV_PARAMpos = Vec(20.39, 40.365);
auto VARIATION_PARAMpos = Vec(41.801, 41.362);
auto DRYWET_PARAMpos = Vec(8.198, 57.377);
auto TIME_PARAMpos = Vec(38.094, 62.24);
auto SIZE_CV_PARAMpos = Vec(42.537, 93.275);

auto SIGNAL_INPUTpos =Vec(7.445, 16.123);
auto VARIATION_CV_INPUTpos =Vec(43.705, 15.781);
auto TIME_CV_INPUTpos =Vec(23.124, 18.374);
auto SIZE_CV_INPUTpos =Vec(36.599, 110.305);

auto MIX_OUTPUTpos = Vec(8.694, 108.391);




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
addInput(createInputCentered<PJ301MPort>(mm2px(SIGNAL_INPUTpos), module, Hellebore::SIGNAL_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(SIZE_CV_INPUTpos), module, Hellebore::SIZE_CV_INPUT));

addOutput(createOutputCentered<PJ301MPort>(mm2px(MIX_OUTPUTpos), module, Hellebore::MIX_OUTPUT));
addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10,10)), module, Hellebore::TEST_OUTPUT));
}
};


Model* modelHellebore = createModel<Hellebore, HelleboreWidget>("Hellebore");