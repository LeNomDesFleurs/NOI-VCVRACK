#include "plugin.hpp"
#include "outils.hpp"
#include "tugowarbuf.hpp"
#include <array>
#include <math.h>

struct Marcotage : Module {

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
	FREEZE_CV_INPUT,
	VARIATION_CV_INPUT,
	TIME_CV_INPUT,
	SIZE_CV_INPUT,
	L_INPUT,
	R_INPUT,
	DRYWET_INPUT,
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

	std::array<float, 2> signal_outputs = {0, 0};
	std::array<float, 2> signal_inputs = {0, 0};
    TugBuffer tugbuffer{4000};

    void onSampleRateChange(const SampleRateChangeEvent & e) override{
	tugbuffer.setSampleRate(e.sampleRate);
	}

	Marcotage()
	 {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configParam(VARIATION_PARAM, 0.f, 0.2f, 0.1, "Variation");
		configParam(SIZE_PARAM, 0.010f, 1.00f, 0.01f, "Size", "s", 4, 0.3, -0.3);
		configParam(TIME_PARAM, 0.1f, 20.f, 10.f, "Time", "s");
		configParam(DRYWET_PARAM, 0.f, 1.f, 1.f, "Drywet", "%", 0.f, 100.f);
		configParam(FREEZE_PARAM, 0, 1, 0, "Freeze");

		configParam(TIME_CV_PARAM, -1.f, 1.f, 0.f, "Time CV", "%", 0,-100.f, 100.f);
		configParam(SIZE_CV_PARAM, -0.01f, 0.01f, 0.f, "Size CV", "%", 0, 10000.f);
		configParam(VARIATION_CV_PARAM, -0.1f, 0.1f, 0.f, "Variation CV", "%",0, 1000.f);

		configInput(FREEZE_CV_INPUT, "Freeze CV");
		configInput(VARIATION_CV_INPUT, "Variation CV");
		configInput(SIZE_CV_INPUT, "Size CV");
		configInput(TIME_CV_INPUT, "Time CV");
		configInput(L_INPUT, "Left");
		configInput(R_INPUT, "Right");
		configInput(DRYWET_INPUT, "Dry/Wet");

		configOutput(L_OUTPUT, "Left");
		configOutput(R_OUTPUT, "Right");
		configOutput(TEST_OUTPUT, "TEST");
	}


	void process(const ProcessArgs& args) override {
		// freeze
		//buffer size
		float combTime_cv = inputs[SIZE_CV_INPUT].getVoltage()*params[SIZE_CV_PARAM].getValue()*10.f;
		float comb_feedback_time = 0.3 * pow(3.f, params[SIZE_PARAM].getValue()) - 0.3;
		comb_feedback_time += combTime_cv;
		//variation
		float variation_cv = inputs[VARIATION_CV_INPUT].getVoltage() * params[VARIATION_CV_PARAM].getValue();
		//time
		float time_cv = inputs[TIME_CV_INPUT].getVoltage() * params[TIME_CV_PARAM].getValue();
		//drywet
		float dry_wet = params[DRYWET_PARAM].getValue() + (inputs[DRYWET_INPUT].getVoltage()/5);
		// input

		signal_inputs[0] = inputs[L_INPUT].getVoltage();
 		signal_inputs[1] = inputs[R_INPUT].getVoltage();

        float output = tugbuffer.process(signal_inputs);
        output = output > 6.f ? 6.f : output;
        output = output < -6.f ? -6.f : output;
        // for(int i = 0; i < 2; i++){
        // signal_outputs[i] = signal_outputs[i] < -6.f ? -6.f : signal_outputs[i];
        // signal_outputs[i] = signal_outputs[i] > 6.f ? 6.f : signal_outputs[i];
        // }

        outputs[L_OUTPUT].setVoltage(output);
		// outputs[R_OUTPUT].setVoltage(signal_outputs[1]);

		// outputs[R_OUTPUT].setVoltage(test_out2);
		// outputs[TEST_OUTPUT].setVoltage(test_out);
	}
};


struct MarcotageWidget : ModuleWidget {
	MarcotageWidget(Marcotage* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Hellebore.svg"), asset::plugin(pluginInstance, "res/HelleboreDark.svg")));


// auto FREEZE_PARAMpos = Vec(25.4, 71.18);
// auto SIZE_PARAMpos = Vec(8.214, 46.133);
// auto TIME_PARAMpos = Vec(25.4, 28.742);
// auto VARIATION_PARAMpos = Vec(43.222, 46.133);
// auto TIME_CV_PARAMpos = Vec(25.4, 46.192);
// auto VARIATION_CV_PARAMpos = Vec(40.906, 62.362);
// auto SIZE_CV_PARAMpos = Vec(10.174, 62.362);
// auto DRYWET_PARAMpos = Vec(25.4, 96.69);

auto FREEZE_PARAMpos = Vec(25.4, 71.176);
auto SIZE_PARAMpos = Vec(8.214, 46.534);
auto TIME_PARAMpos = Vec(25.4, 29.46);
auto VARIATION_PARAMpos = Vec(43.222, 46.534);
auto TIME_CV_PARAMpos = Vec(25.4, 46.432);
auto VARIATION_CV_PARAMpos = Vec(40.906, 62.601);
auto SIZE_CV_PARAMpos = Vec(10.174, 62.601);
auto DRYWET_PARAMpos = Vec(25.4, 97.09);

auto TIME_CV_INPUTpos =Vec(25.4, 60.556);
auto VARIATION_CV_INPUTpos =Vec(41.88, 80.539);
auto SIZE_CV_INPUTpos =Vec(8.796, 80.539);
auto FREEZE_CV_INPUTpos =Vec(25.4, 82.72);
auto R_INPUTpos =Vec(6.956, 95.456);
auto L_INPUTpos =Vec(6.956, 109.792);
auto DRYWET_INPUTpos =Vec(25.40, 110.72);

auto R_OUTPUTpos = Vec(44.202, 95.749);
auto L_OUTPUTpos = Vec(44.202, 109.792);




addParam(createParamCentered<RoundBlackKnob>(mm2px(SIZE_PARAMpos), module, Marcotage::SIZE_PARAM));
addParam(createParamCentered<Trimpot>(mm2px(VARIATION_CV_PARAMpos), module, Marcotage::VARIATION_CV_PARAM));
addParam(createParamCentered<RoundBlackKnob>(mm2px(DRYWET_PARAMpos), module, Marcotage::DRYWET_PARAM));
addParam(createParamCentered<Trimpot>(mm2px(SIZE_CV_PARAMpos), module, Marcotage::SIZE_CV_PARAM));
addParam(createParamCentered<RoundBlackKnob>(mm2px(VARIATION_PARAMpos), module, Marcotage::VARIATION_PARAM));
addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(TIME_PARAMpos), module, Marcotage::TIME_PARAM));
addParam(createParamCentered<Trimpot>(mm2px(TIME_CV_PARAMpos), module, Marcotage::TIME_CV_PARAM));
addParam(createLightParamCentered<VCVLightBezelLatch<>>(mm2px(FREEZE_PARAMpos), module, Marcotage::FREEZE_PARAM, Marcotage::FREEZE_LIGHT));


addInput(createInputCentered<PJ301MPort>(mm2px(R_INPUTpos), module, Marcotage::R_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(L_INPUTpos), module, Marcotage::L_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(SIZE_CV_INPUTpos), module, Marcotage::SIZE_CV_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(VARIATION_CV_INPUTpos), module, Marcotage::VARIATION_CV_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(FREEZE_CV_INPUTpos), module, Marcotage::FREEZE_CV_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(TIME_CV_INPUTpos), module, Marcotage::TIME_CV_INPUT));
addInput(createInputCentered<PJ301MPort>(mm2px(DRYWET_INPUTpos), module, Marcotage::DRYWET_INPUT));

addOutput(createOutputCentered<PJ301MPort>(mm2px(R_OUTPUTpos), module, Marcotage::R_OUTPUT));
addOutput(createOutputCentered<PJ301MPort>(mm2px(L_OUTPUTpos), module, Marcotage::L_OUTPUT));

//addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10,10)), module, Marcotage::TEST_OUTPUT));
}
};

Model* modelMarcotage = createModel<Marcotage, MarcotageWidget>("Marcotage");