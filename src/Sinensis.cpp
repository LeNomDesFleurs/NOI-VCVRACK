#include "plugin.hpp"
#include "filter.hpp"
#include "outils.hpp"


struct Sinensis : Module {
private:
	noi::Filter::Biquad bpf[6]{ {"BPF"}, {"BPF"}, {"BPF"}, {"BPF"}, {"BPF"}, {"BPF"} };
	//PARAM
	float frequence, Q, numberOfBand, ratio;
	float freq_cv;
	dsp::ClockDivider oneInFour;
		float debug=1000;


public:

	// void onSampleRateChange(const SampleRateChangeEvent & e) override{
	// 	float sample_rate = e.sampleRate;
	// 	for (auto filter : bpf)
	// 	{
	// 		filter.setSampleRate(sample_rate);
	// 	}
	// }

	// void onSampleRateChange() override{
		
	// 	float sample_rate = APP->engine->getSampleRate();;
	// 	for (auto filter : bpf)
	// 	{
	// 		filter.setSampleRate(sample_rate);
		
	// 	}
	// }

	enum ParamId {
		FREQ_PARAM,
		Q_PARAM,
		BAND_PARAM,
		RATIO_PARAM,
		RATIO_CV_PARAM,

		FREQ_CV_PARAM,
		Q_CV_PARAM,
		BAND_CV_PARAM,

		PARAMS_LEN
	};
	enum InputId {
		SIGNAL_INPUT,

		FREQ_CV_INPUT,
		Q_CV_INPUT,
		BAND_CV_INPUT,
		RATIO_CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		MIX_OUTPUT,
		// DEBUG,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};



	Sinensis() {
		
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(FREQ_PARAM, -54.f, 54.F, 0.f, "Cutoff frequency", " Hz", dsp::FREQ_SEMITONE, dsp::FREQ_C4);
		configParam(Q_PARAM, 1.f, 40.f, 20.f, "Q");
		configParam(BAND_PARAM, 1, 6.0, 3.0, "Number of band");
		configParam(RATIO_PARAM, 0, 2.f, 1.5, "Ratio");

		configParam(FREQ_CV_PARAM, -200, 200, 0, "Frequence CV Attenuverter", "%", 0, 0.5);
		configParam(RATIO_CV_PARAM, -1, 1, 0, "Ratio CV Attenuverter");
		configParam(Q_CV_PARAM, -2, 2, 0, "Q CV Attenuverter");
		configParam(BAND_CV_PARAM, -2, 2, 0, "Number of band CV Attenuverter");

		configInput(FREQ_CV_INPUT, "Frequence CV");
		configInput(RATIO_CV_INPUT, "Ratio CV");
		configInput(Q_CV_INPUT, "Q CV");
		configInput(BAND_CV_INPUT, "Band CV");

		configInput(SIGNAL_INPUT, "Audio");
		configOutput(MIX_OUTPUT, "Audio");

		oneInFour.setDivision(4);
	}

		void onSampleRateChange(const SampleRateChangeEvent & e) override{

		float sample_rate = e.sampleRate;
		for (auto &filter : bpf)
		{
			filter.setSampleRate(sample_rate);
		}
	}

	void process(const ProcessArgs& args) override {
		float output = 0;
		float input = inputs[SIGNAL_INPUT].getVoltage();

		//update param 1 in 4 sample
		if (oneInFour.process()) {
			//retrieve param
			//Q
			Q = params[Q_PARAM].getValue();
			Q += inputs[Q_CV_INPUT].getVoltage() * params[Q_CV_PARAM].getValue();
			if (Q < 0.1) { Q = 0.1; }
			//Number of band
			numberOfBand = params[BAND_PARAM].getValue();
			numberOfBand += params[BAND_CV_PARAM].getValue() * inputs[BAND_CV_INPUT].getVoltage();
			numberOfBand = rack::math::clamp(numberOfBand, 1.F, 6.f);
			//Ratio
			ratio = params[RATIO_PARAM].getValue();
			ratio += params[RATIO_CV_PARAM].getValue() * inputs[RATIO_CV_INPUT].getVoltage();
			ratio = rack::math::clamp(ratio, 0.f, 2.f);
			//frequence
			frequence = dsp::FREQ_C4 * pow(dsp::FREQ_SEMITONE,params[FREQ_PARAM].getValue());
			freq_cv = params[FREQ_CV_PARAM].getValue();
			frequence += (freq_cv * inputs[FREQ_CV_INPUT].getVoltage());

			if (frequence > debug)
				debug = frequence;

			if (ratio < 0) { ratio = 1 / -ratio; }
			for (int i = 0; i < numberOfBand; i++) {
				//multiply frequence by ratio
				float thisBandFreq = frequence;
				for (int j = 1; j <= i; j++) { thisBandFreq *= ratio; }
				thisBandFreq = noi::Outils::truncate(thisBandFreq);
				//frequency folding
				while (thisBandFreq > 15000.f || thisBandFreq < 30.f) {
					if (thisBandFreq > 15000.F) { thisBandFreq = 15000.f - (thisBandFreq - 15000.f); }
					if (thisBandFreq < 30.f) { thisBandFreq = 30.f + (30.f - thisBandFreq); }
				}
				//setParam only compute if params are different
				bpf[i].setParam(thisBandFreq, Q);
			}
		}
		
		//process filter each clock ticks
		output = 0;
		for (int i = 0; i < numberOfBand; i++) {
			float j = static_cast<float> (i);
			if (numberOfBand - j < 1) {
				output += (bpf[i].process(input) / numberOfBand) * (numberOfBand - j);
			}
			else {
				output += bpf[i].process(input) / (numberOfBand);
			}
		}
	
		// set output
		outputs[MIX_OUTPUT].setVoltage(rack::math::clamp(output, -5.f, 5.f));
		// outputs[MIX_OUTPUT].setVoltage(bpf[0].process(0));
		// outputs[DEBUG].setVoltage(debug);
	}
};


struct SinensisWidget : ModuleWidget {
	SinensisWidget(Sinensis* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Sinensis.svg")));

		auto FREQ_CV_PARAMpos = Vec(6.435, 29.906);
		auto Q_CV_PARAMpos = Vec(45.333, 29.906);
		auto FREQ_PARAMpos = Vec(12.824, 50.361);
		auto Q_PARAMpos = Vec(38.274, 50.361);
		auto BAND_PARAMpos = Vec(12.353, 76.6);
		auto RATIO_PARAMpos = Vec(38.499, 76.72);
		auto BAND_CV_PARAMpos = Vec(8.312, 94.375);
		auto RATIO_CV_PARAMpos = Vec(43.214, 94.375);

		auto FREQ_CV_INPUTpos = Vec(16.637, 16.861);
		auto Q_CV_INPUTpos = Vec(34.706, 16.861);
		auto SIGNAL_INPUTpos = Vec(25.613, 34.015);
		auto BAND_CV_INPUTpos = Vec(15.357, 110.305);
		auto RATIO_CV_INPUTpos = Vec(36.599, 110.305);

		auto MIX_OUTPUTpos = Vec(25.613, 95.792);


		//PARAM
		addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(FREQ_PARAMpos), module, Sinensis::FREQ_PARAM));
		addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(Q_PARAMpos), module, Sinensis::Q_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(BAND_PARAMpos), module, Sinensis::BAND_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(RATIO_PARAMpos), module, Sinensis::RATIO_PARAM));
		//CV TRIMPOT
		addParam(createParamCentered<Trimpot>(mm2px(BAND_CV_PARAMpos), module, Sinensis::BAND_CV_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(RATIO_CV_PARAMpos), module, Sinensis::RATIO_CV_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(FREQ_CV_PARAMpos), module, Sinensis::FREQ_CV_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Q_CV_PARAMpos), module, Sinensis::Q_CV_PARAM));
		//INPUT
		addInput(createInputCentered<PJ301MPort>(mm2px(SIGNAL_INPUTpos), module, Sinensis::SIGNAL_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Q_CV_INPUTpos), module, Sinensis::Q_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(FREQ_CV_INPUTpos), module, Sinensis::FREQ_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(BAND_CV_INPUTpos), module, Sinensis::BAND_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(RATIO_CV_INPUTpos), module, Sinensis::RATIO_CV_INPUT));
		//OUTPUT
		addOutput(createOutputCentered<PJ301MPort>(mm2px(MIX_OUTPUTpos), module, Sinensis::MIX_OUTPUT));
		// addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10, 10)), module, Sinensis::DEBUG));
	}
};


Model* modelSinensis = createModel<Sinensis, SinensisWidget>("Sinensis");