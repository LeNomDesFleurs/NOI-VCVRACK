#include "plugin.hpp"
#include <cmath>
#include <vector>



struct Sunflower : Module {

	struct strip {
		int index{};
		float coef{};
		strip(int temp) { index = temp;}
	};
	std::vector<strip> fullMixer;
	std::vector<strip> Mixer;
	float output, controlSignal;
	float diffusion{ 1.5 };
	float difference;
	float out_test;
	float param_offset;
	float param_amp;
	float fMixerSize;
	float test_out;
	dsp::ClockDivider oneIn16;

	enum ParamId {
		PARAM_A,
		PARAM_B,
		PARAM_C,
		PARAM_D,
		PARAM_E,
		PARAM_F,
		PARAM_G,
		PARAM_H,
		PARAM_I,
		PARAM_J,
		PARAM_K,
		PARAM_L,
		PARAM_SELECTION,
		PARAM_DIFFUSION,
		PARAM_AMP,
		PARAM_ONLYCONNECTED,
		PARAMS_LEN
	};
	enum InputId {
		INPUT_A,
		INPUT_B,
		INPUT_C,
		INPUT_D,
		INPUT_E,
		INPUT_F,
		INPUT_G,
		INPUT_H,
		INPUT_I,
		INPUT_J,
		INPUT_K,
		INPUT_L,
		INPUT_CONTROL,
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUT,
		OUT_TEST,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHT_A,
		LIGHT_B,
		LIGHT_C,
		LIGHT_D,
		LIGHT_E,
		LIGHT_F,
		LIGHT_G,
		LIGHT_H,
		LIGHT_I,
		LIGHT_J,
		LIGHT_K,
		LIGHT_L,
		LIGHT_ONLYCONNECTED,
		LIGHTS_LEN
	};

	Sunflower() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		for (int i = 0; i < 12; i++) {
			configParam(PARAM_A+i, 0.f, 2.f, 1.f, "Amp ");
		}
		configParam(PARAM_SELECTION, 6.f, -6.f, 0.f, "Selection");
		configParam(PARAM_DIFFUSION, 1.f, 4.f, 1.5f, "Diffusion");
		configParam(PARAM_AMP, -1.f, 1.f, 0.f, "Selection CV Attenuverter");
		configInput(INPUT_CONTROL, "Position CV");
		configParam(PARAM_ONLYCONNECTED, 0.f, 1.f, 0.f, "Only connected");
		oneIn16.setDivision(16);
		for (int i = 0; i < 12; i++) {
			fullMixer.push_back(strip(i));
		}
		Mixer = fullMixer;
	}

	void process(const ProcessArgs& args) override {

		output = 0;

		//if (outputs[OUTPUT].isConnected()) {

			//update mixer strips every 16 samples

			//if (oneIn16.process()) {
				diffusion = params[PARAM_DIFFUSION].getValue();
				Mixer.clear();
				//retrieve all connected input if the param is on
				if (params[PARAM_ONLYCONNECTED].getValue()) {
					for (int i = 0; i < 12; i++) {
						if (inputs[i].isConnected()) {
							Mixer.push_back(strip(i));
						}
						else { lights[i].setBrightness(0); }
					}
				}
				else {Mixer = fullMixer;}
				fMixerSize = static_cast<float> (Mixer.size());
			//}
		
			//compute CV if CV is connected
			controlSignal = 6 + params[PARAM_SELECTION].getValue();
			if (inputs[INPUT_CONTROL].isConnected()) {
				controlSignal += inputs[INPUT_CONTROL].getVoltage()*params[PARAM_AMP].getValue();
				//cycling control signal

			}
			
			//conpute the coef from the difference between control signal position and input index
			// -6 and 6 are 6 o'clock, 0 is 12 o'clock
			for (unsigned int i = 0; i < Mixer.size(); i++) {

				while (controlSignal > fMixerSize || controlSignal < 0) {
					if (controlSignal < 0) { controlSignal = controlSignal + fMixerSize; }
					if (controlSignal > fMixerSize) { controlSignal = controlSignal - fMixerSize; }
				}
				float finputWhoseCoefIsBeingCalculated = static_cast<float> (i);
				float difference = controlSignal - finputWhoseCoefIsBeingCalculated;
				float tempControl = (difference < 0) ? (fMixerSize + difference) :  difference;
				difference = (tempControl < (fMixerSize / 2.f)) ? tempControl : (fMixerSize - tempControl);
				//fontion affine decroissante avec f(0)=1 et f(x) = -x/diff +1
				Mixer[i].coef = difference * -(1 / diffusion) + 1;
				Mixer[i].coef = rack::math::clamp(Mixer[i].coef, 0.f, 1.f);
			}

			for (unsigned int i = 0; i < Mixer.size(); i++) {
				output += inputs[Mixer[i].index].getVoltage()*params[Mixer[i].index].getValue() * Mixer[i].coef;
				lights[Mixer[i].index].setBrightness(Mixer[i].coef);
			}

			
		//}
		outputs[OUTPUT].setVoltage(output);
		//latch light
		lights[LIGHT_ONLYCONNECTED].setBrightness(params[PARAM_ONLYCONNECTED].getValue());

	}
};


struct SunflowerWidget : ModuleWidget {
	SunflowerWidget(Sunflower* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Sunflower.svg")));

		//
		//					PARAM
		//
		addParam(createParamCentered<RoundHugeBlackKnob>
			(mm2px(Vec(16.042891, 34.756668)), module, Sunflower::PARAM_SELECTION));
		addParam(createParamCentered<RoundHugeBlackKnob>
			(mm2px(Vec(56.117672, 35.177231)), module, Sunflower::PARAM_DIFFUSION));
		addParam(createParamCentered<Trimpot>
			(mm2px(Vec(35.56, 34.87)), module, Sunflower::PARAM_AMP));

		addParam(createParamCentered<Trimpot>
			(mm2px(Vec(35.56002592289244, 64.43645038308394)), module, Sunflower::PARAM_A));
		addParam(createParamCentered<Trimpot>
			(mm2px(Vec(25.983582, 67.002457)), module, Sunflower::PARAM_B));
		addParam(createParamCentered<Trimpot>
			(mm2px(Vec(18.975088059059733, 74.01397004353751)), module, Sunflower::PARAM_C));
		addParam(createParamCentered<Trimpot>
			(mm2px(Vec(16.409396927568505, 83.58929376066973)), module, Sunflower::PARAM_D));
		addParam(createParamCentered<Trimpot>
			(mm2px(Vec(18.97510918968731, 93.16461492582965)), module, Sunflower::PARAM_E));
		addParam(createParamCentered<Trimpot>
			(mm2px(Vec(25.98390964626912, 100.17450820302693)), module, Sunflower::PARAM_F));
		addParam(createParamCentered<Trimpot>
			(mm2px(Vec(35.5592134031279, 102.74027872111489)), module, Sunflower::PARAM_G));
		addParam(createParamCentered<Trimpot>
			(mm2px(Vec(45.13455736198345, 100.17465434331262)), module, Sunflower::PARAM_H));
		addParam(createParamCentered<Trimpot>
			(mm2px(Vec(52.14689045827015, 93.16573673045261)), module, Sunflower::PARAM_I));
		addParam(createParamCentered<Trimpot>
			(mm2px(Vec(54.71288604921778, 83.5892835340284)), module, Sunflower::PARAM_J));
		addParam(createParamCentered<Trimpot>
			(mm2px(Vec(52.14687700000001, 74.01284)), module, Sunflower::PARAM_K));
		addParam(createParamCentered<Trimpot>
			(mm2px(Vec(45.1364750736936, 67.00245190220349)), module, Sunflower::PARAM_L));
		//
		//					INPUT
		//
		addInput(createInputCentered<PJ301MPort>
			(mm2px(Vec(35.56002330078503, 55.6413789247072)), module, Sunflower::INPUT_A));
		addInput(createInputCentered<PJ301MPort>
			(mm2px(Vec(21.586042, 59.3857)), module, Sunflower::INPUT_B));
		addInput(createInputCentered<PJ301MPort>
			(mm2px(Vec(11.356482984833766, 69.61534982623817)), module, Sunflower::INPUT_C));
		addInput(createInputCentered<PJ301MPort>
			(mm2px(Vec(7.612180841708783, 83.58928034524848)), module, Sunflower::INPUT_D));
		addInput(createInputCentered<PJ301MPort>
			(mm2px(Vec(11.356488971018173, 97.5632114102869)), module, Sunflower::INPUT_E));
		addInput(createInputCentered<PJ301MPort>
			(mm2px(Vec(21.585221275465976, 107.79307239874848)), module, Sunflower::INPUT_F));
		addInput(createInputCentered<PJ301MPort>
			(mm2px(Vec(35.55912232027082, 111.5374943649854)), module, Sunflower::INPUT_G));
		addInput(createInputCentered<PJ301MPort>
			(mm2px(Vec(49.533090295065755, 107.7933118499466)), module, Sunflower::INPUT_H));
		addInput(createInputCentered<PJ301MPort>
			(mm2px(Vec(59.763644387557235, 97.56326712472773)), module, Sunflower::INPUT_I));
		addInput(createInputCentered<PJ301MPort>
			(mm2px(Vec(63.50795614156911, 83.58928127794638)), module, Sunflower::INPUT_J));
		addInput(createInputCentered<PJ301MPort>
			(mm2px(Vec(59.76363400000001, 69.615303)), module, Sunflower::INPUT_K));
		addInput(createInputCentered<PJ301MPort>
			(mm2px(Vec(49.534009811493206, 59.38569569612148)), module, Sunflower::INPUT_L));
		
		addInput(createInputCentered<PJ301MPort>
			(mm2px(Vec(35.44, 22.66)), module, Sunflower::INPUT_CONTROL));
		//
		//					LIGHT
		//
		addChild(createLightCentered<SmallLight<WhiteLight>>
			(mm2px(Vec(35.56003484760835, 71.11817292502253)), module, Sunflower::LIGHT_A));
		addChild(createLightCentered<SmallLight<WhiteLight>>
			(mm2px(Vec(29.324451, 72.788994)), module, Sunflower::LIGHT_B));
		addChild(createLightCentered<SmallLight<WhiteLight>>
			(mm2px(Vec(24.759148047856513, 77.3533814183163)), module, Sunflower::LIGHT_C));
		addChild(createLightCentered<SmallLight<WhiteLight>>
			(mm2px(Vec(23.088245502269057, 83.58927885051651)), module, Sunflower::LIGHT_D));
		addChild(createLightCentered<SmallLight<WhiteLight>>
			(mm2px(Vec(24.759154268330867, 89.8251777259079)), module, Sunflower::LIGHT_E));
		addChild(createLightCentered<SmallLight<WhiteLight>>
			(mm2px(Vec(29.323372602107405, 94.39047541277506)), module, Sunflower::LIGHT_F));
		addChild(createLightCentered<SmallLight<WhiteLight>>
			(mm2px(Vec(35.55925587968064, 96.06142970500136)), module, Sunflower::LIGHT_G));
		addChild(createLightCentered<SmallLight<WhiteLight>>
			(mm2px(Vec(41.79517070374327, 94.39058112060593)), module, Sunflower::LIGHT_H));
		addChild(createLightCentered<SmallLight<WhiteLight>>
			(mm2px(Vec(46.36034545842536, 89.8248791904378)), module, Sunflower::LIGHT_I));
		addChild(createLightCentered<SmallLight<WhiteLight>>
			(mm2px(Vec(48.0311650072792, 83.58928986066809)), module, Sunflower::LIGHT_J));
		addChild(createLightCentered<SmallLight<WhiteLight>>
			(mm2px(Vec(46.36034000000001, 77.353706)), module, Sunflower::LIGHT_K));
		addChild(createLightCentered<SmallLight<WhiteLight>>
			(mm2px(Vec(41.795619771947386, 72.78899537884318)), module, Sunflower::LIGHT_L));
		addParam(createLightParamCentered<VCVLightBezelLatch<>>
			(mm2px(Vec(35.44, 43.14)), module, Sunflower::PARAM_ONLYCONNECTED, Sunflower::LIGHT_ONLYCONNECTED));
		//
		//					OUTPUT
		//
			addOutput(createOutputCentered<PJ301MPort>
				(mm2px(Vec(35.572552, 83.596596)), module, Sunflower::OUTPUT));
	}
};


Model* modelSunflower = createModel<Sunflower, SunflowerWidget>("Sunflower");