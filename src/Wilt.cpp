#include "plugin.hpp"
#include "filter.hpp"
//#include "outils.hpp"
#include "reverb.hpp"
#include <math.h>



struct Wilt : Module {
private:
	int counter{};
	noi::Filter::Biquad lpf{ "LPF", 10000.f, 0.707 };
	noi::Reverb::Schroeder schroeder;
	float redux_input,
	redux_output,
	// rm_input,
	// rm_cv_input,
	rm_cv,
	rm_output,
	filter_input,
	filter_output,
	rvb_input,
	rvb_output;

	// float RM;
	// float rt60;
	// float drywet;
	// bool clip;
	// bool intFloat;
	float redux_mod;
	// float redux;
	float repeats;
	// float frequence;
	// float Q{ 0.707 };

public:

	enum ParamId {
		RT60_PARAM,
		DW_PARAM,
		EXP_PARAM,
		REDUX_PARAM,
		REDUX_CV_PARAM,
		INTFLOAT_PARAM,
		TONE_PARAM,
		SPEED_CV_PARAM,
		SPEED_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		SIGNAL_INPUT,
		REDUX_CV_INPUT,
		SPEED_CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		MIX_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		INTFLOAT_LIGHT,
		LIGHTS_LEN
	};

	struct MyCustomParamQuantity : rack::ParamQuantity 
		{
		   std::string getDisplayValueString() override {
		      auto scaleVal = 5.0 * std::sin(getValue() * cheappi / 2.f);
		      return std::to_string(scaleVal); // or whatever formatting you want
		   }
		   void setDisplayValueString(const std::string &s)
		   {
		      auto sv = std::atof(s.c_str());
		      auto v = std::asin(sv / 5 ) * 2 / cheappi; // Do some bounds checking
		      setValue(v);
		   }
		};
	
	Wilt() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(SPEED_PARAM, -2.f, 2.f, 1.f, "Speed");
		configInput(SIGNAL_INPUT, "Audio");
		configInput(SPEED_CV_INPUT, "Speed CV");
		configInput(REDUX_CV_INPUT, "Redux CV");
		configParam(REDUX_PARAM, 0.f, 100.f, 0.f, "Redux");
		configParam(INTFLOAT_PARAM, 0, 1, 0, "INTFLOAT");
		configParam(REDUX_CV_PARAM, -1.f, 1.f, 0.f, "Redux CV Attenuverter", "%", 0.f, 100.f);
		configParam(SPEED_CV_PARAM, -0.2f, 0.2f, 0.f, "Speed CV Attenuverter", "%", 0.f, 500.f);
		configParam(TONE_PARAM, 20.f, 10000.f, 10000.f, "Tone");
		configParam(RT60_PARAM, 0.1f, 0.99f, 0.8f, "FeedBack", "s");
		configParam(DW_PARAM, 0.f, 1.f, 1.f, "Dry/Wet", "%", 0.f, 100.f);

		configOutput(MIX_OUTPUT, "");
	}
	float test_out;
	void process(const ProcessArgs& args) override {

		//redux
		redux_input = inputs[SIGNAL_INPUT].getVoltage();
		redux_mod = inputs[REDUX_CV_INPUT].getVoltage();
		redux_mod *= params[REDUX_CV_PARAM].getValue();
		if (params[INTFLOAT_PARAM].getValue()) {
			redux_mod = noi::Outils::truncate(redux_mod);
		}
		redux_mod = noi::Outils::mapValue(redux_mod, -5, 5, -50, 50);
		repeats = params[REDUX_PARAM].getValue() + redux_mod;
		repeats = rack::math::clamp(repeats, 0.f, 100.f);
		if (counter >= repeats) { counter = 0; redux_output = redux_input; }
		counter++;
		lights[INTFLOAT_LIGHT].setBrightness(params[INTFLOAT_PARAM].getValue());


		//Ring Modulation

		rm_output = redux_output;
		if (inputs[SPEED_CV_INPUT].isConnected()) {
			rm_cv = inputs[SPEED_CV_INPUT].getVoltage();
			rm_cv *= params[SPEED_CV_PARAM].getValue();
			rm_cv *= 0.2;
			rm_output *= rm_cv;
		}


		//filter

		filter_input = rm_output;	
		lpf.setParam(params[TONE_PARAM].getValue());
		filter_output = lpf.process(filter_input);


		//schroeder reverb

		rvb_input = filter_output;
		float speed_cv = inputs[SPEED_CV_INPUT].getVoltage();
		speed_cv *= params[SPEED_CV_PARAM].getValue();
		float speed = speed_cv + params[SPEED_PARAM].getValue();
		schroeder.setDryWet(params[DW_PARAM].getValue());
		schroeder.setTime(params[RT60_PARAM].getValue());
		schroeder.setStep(speed);
		rvb_output = schroeder.process(rvb_input);

		//set output
		// outputs[MIX_OUTPUT].setVoltage(rack::math::clamp(rvb_output, -5.f, 5.f));
		outputs[MIX_OUTPUT].setVoltage(rvb_output);
		
	}
};


struct WiltWidget : ModuleWidget {
	WiltWidget(Wilt* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Wilt.svg")));

	
auto REDUX_MOD_PARAMpos = Vec(52.544, 24.405);
auto INTFLOAT_PARAMpos = Vec(52.552, 42.065);
auto REDUX_PARAMpos = Vec(31.441, 45.341);
auto TONE_PARAMpos = Vec(11.101, 57.535);
auto RT60_PARAMpos = Vec(44.606, 71.836);
auto SPEED_PARAMpos = Vec(15.497, 87.317);
auto DW_PARAMpos = Vec(51.522, 93.393);

auto REDUX_MOD_INPUTpos =Vec(35.506, 18.014);
auto SIGNAL_INPUTpos =Vec(14.15, 25.083);

auto MIX_OUTPUTpos = Vec(35.34, 107.322);


		addParam(createParamCentered<Trimpot>(mm2px(REDUX_MOD_PARAMpos), module, Wilt::REDUX_CV_PARAM));
		addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(REDUX_PARAMpos), module, Wilt::REDUX_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(TONE_PARAMpos), module, Wilt::TONE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(DW_PARAMpos), module, Wilt::DW_PARAM));
		addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(RT60_PARAMpos), module, Wilt::RT60_PARAM));
		addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(SPEED_PARAMpos), module, Wilt::SPEED_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(REDUX_MOD_INPUTpos), module, Wilt::REDUX_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(SIGNAL_INPUTpos), module, Wilt::SIGNAL_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(MIX_OUTPUTpos), module, Wilt::MIX_OUTPUT));
		addParam(createLightParamCentered<VCVLightBezelLatch<>>(mm2px(INTFLOAT_PARAMpos), module, Wilt::INTFLOAT_PARAM, Wilt::INTFLOAT_LIGHT));
	}
};


Model* modelWilt = createModel<Wilt, WiltWidget>("Wilt");