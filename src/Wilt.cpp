#include "plugin.hpp"
#include "filter.hpp"
//#include "outils.hpp"
#include "reverb.hpp"



struct Wilt : Module {
private:
	int counter{};
	noi::Filter::Biquad lpf{ "LPF", 10000.f, 0.707 };
	noi::Reverb::Schroeder schroeder;
	float redux_input;
	float redux_output;
	float rm_input;
	float rm_cv_input;
	float rm_cv;
	float rm_output;
	float filter_input;
	float filter_output;
	float rvb_output;
	float rvb_input;
	float RM;
	float rt60;
	float drywet;
	bool clip;
	bool intFloat;
	float redux_mod;
	float redux;
	float repeats;
	float frequence;
	float Q{ 0.707 };
	//temp

public:

	enum ParamId {
		RT60_PARAM,
		DW_PARAM,
		EXP_PARAM,
		REDUX_PARAM,
		REDUX_MOD_PARAM,
		INTFLOAT_PARAM,
		FREQ_PARAM,
		TONE_PARAM,
		RMTRIM_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		SIGNAL_INPUT,
		REDUX_MOD_INPUT,
		RM_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		MIX_OUTPUT,
		TEST_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		TEST_LIGHT,
		INTFLOAT_LIGHT,
		EXP_LIGHT,
		RM_LIGHT,
		LIGHTS_LEN
	};

	Wilt() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configInput(SIGNAL_INPUT, "");
		configInput(RM_INPUT, "rm input");
		configInput(REDUX_MOD_INPUT, "redux mod");
		configParam(REDUX_PARAM, 0.f, 100.f, 0.f, "Redux");
		configParam(REDUX_MOD_PARAM, -1.f, 1.f, 0.f, "Redux CV", "%", 0, 100);
		configParam(INTFLOAT_PARAM, 0, 1, 1, "INTFLOAT");
		configParam(EXP_PARAM, 0, 1, 1, "EXP");
		configParam(RMTRIM_PARAM, -2.f, 2.f, 0.f, "RM CV");
		configParam(TONE_PARAM, 20.f, 10000.f, 10000.f, "Tone");
		configParam(RT60_PARAM, 0.f, 10.f, 0.f, "RT60", "s");
		configParam(DW_PARAM, 0, 1, 0.f, "Dry/Wet", "%", 0, 100);

		configOutput(MIX_OUTPUT, "");
	}
	float test_out;
	void process(const ProcessArgs& args) override {


		//redux
		redux_input = inputs[SIGNAL_INPUT].getVoltage();
		redux_mod = inputs[REDUX_MOD_INPUT].getVoltage();
		redux_mod *= params[REDUX_MOD_PARAM].getValue();
		if (params[INTFLOAT_PARAM].getValue()) {
			redux_mod = truncf(redux_mod);
		}
		redux_mod = noi::Outils::mapValue(redux_mod, -5, 5, -50, 50);
		repeats = params[REDUX_PARAM].getValue() + redux_mod;
		repeats = rack::math::clamp(repeats, 0.f, 100.f);
		if (counter >= repeats) { counter = 0; redux_output = redux_input; }
		counter++;
		lights[INTFLOAT_LIGHT].setBrightness(params[INTFLOAT_PARAM].getValue());


		//Ring Modulation

		rm_output = redux_output;
		if (inputs[RM_INPUT].isConnected()) {
			rm_cv = inputs[RM_INPUT].getVoltage();
			rm_cv *= params[RMTRIM_PARAM].getValue();
			rm_cv *= 0.2;
			test_out = rm_cv;
			rm_output *= rm_cv;
		}


		//filter

		filter_input = rm_output;	
		lpf.setParam(params[TONE_PARAM].getValue());
		filter_output = lpf.process(filter_input);


		//schroeder reverb

		rvb_input = filter_output;
		schroeder.setDryWet(params[DW_PARAM].getValue());
		schroeder.setParam(params[RT60_PARAM].getValue());
		rvb_output = schroeder.process(rvb_input);

		//set output
		outputs[MIX_OUTPUT].setVoltage(rack::math::clamp(rvb_output, -5.f, 5.f));
		outputs[TEST_OUTPUT].setVoltage(test_out);
	}
};


struct WiltWidget : ModuleWidget {
	WiltWidget(Wilt* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Wilt.svg")));

		//addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		//addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		//addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


		auto REDUX_MOD_PARAMpos = Vec(52.544, 24.405);
		auto INTFLOAT_PARAMpos = Vec(52.552, 42.065);
		auto REDUX_PARAMpos = Vec(31.441, 45.341);
		auto RMTRIM_PARAMpos = Vec(29.715, 73.82);
		auto TONE_PARAMpos = Vec(10.078, 82.31);
		auto DW_PARAMpos = Vec(45.856, 86.061);
		auto RT60_PARAMpos = Vec(21.319, 100.949);

		auto REDUX_MOD_INPUTpos = Vec(35.506, 18.014);
		auto SIGNAL_INPUTpos = Vec(14.15, 25.083);
		auto RM_INPUTpos = Vec(48.022, 62.736);

		auto MIX_OUTPUTpos = Vec(51.336, 107.156);


		addParam(createParamCentered<Trimpot>(mm2px(REDUX_MOD_PARAMpos), module, Wilt::REDUX_MOD_PARAM));
		addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(REDUX_PARAMpos), module, Wilt::REDUX_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(RMTRIM_PARAMpos), module, Wilt::RMTRIM_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(TONE_PARAMpos), module, Wilt::TONE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(DW_PARAMpos), module, Wilt::DW_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(RT60_PARAMpos), module, Wilt::RT60_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(REDUX_MOD_INPUTpos), module, Wilt::REDUX_MOD_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(SIGNAL_INPUTpos), module, Wilt::SIGNAL_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(RM_INPUTpos), module, Wilt::RM_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(MIX_OUTPUTpos), module, Wilt::MIX_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10, 10)), module, Wilt::TEST_OUTPUT));
		addParam(createLightParamCentered<VCVLightBezelLatch<>>(mm2px(INTFLOAT_PARAMpos), module, Wilt::INTFLOAT_PARAM, Wilt::INTFLOAT_LIGHT));
	}
};


Model* modelWilt = createModel<Wilt, WiltWidget>("Wilt");