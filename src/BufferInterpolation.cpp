#include "plugin.hpp"
#include "RingBuffer.hpp"

struct BufferInterpolation : Module {
private:

	float switchControl, compA, compB, switchA, switchB, comparatorSwitch,
		continuousVoltage,
		switchOut,
		compOut,
		lightSwitchA,
		lightSwitchB,
		lightComp;

RingBuffer m_ringbuffer = RingBuffer(2, 1);

public:

	enum ParamId {
		PARAM_CONTINUOUS,
		PARAMS_LEN
	};
	enum InputId {
		INPUT_COMPA,
		INPUT_COMPB,
		INPUT_SWITCH,
		INPUT_SWITCHA,
		INPUT_SWITCHB,

		INPUTS_LEN
	};
	enum OutputId {
		OUTPUT_SWITCH,
		OUTPUT_COMP,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHT_COMP,
		LIGHT_SWITCHA,
		LIGHT_SWITCHB,
		LIGHTS_LEN
	};

	BufferInterpolation() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PARAM_CONTINUOUS, -5.f, 5.f, 0.f, "Voltage");
		configInput(INPUT_COMPA, "Comparator A");
		configInput(INPUT_COMPB, "Comparator B");
		configOutput(OUTPUT_COMP, "Comparator");

		configInput(INPUT_SWITCHA, "Switch A");
		configInput(INPUT_SWITCHB, "Switch B");
		configInput(INPUT_SWITCH, "Switch Control");
		configOutput(OUTPUT_SWITCH, "Switch");

	}


	void process(const ProcessArgs& args) override {

		continuousVoltage = params[PARAM_CONTINUOUS].getValue();


		//uses continuous voltage if disconnected
		compA = inputs[INPUT_COMPA].isConnected() ? inputs[INPUT_COMPA].getVoltage() : continuousVoltage;
		compB = inputs[INPUT_COMPB].isConnected() ? inputs[INPUT_COMPB].getVoltage() : continuousVoltage;

		float input = inputs[INPUT_COMPA].getVoltage();
		m_ringbuffer.writeSample(input);
		float output = m_ringbuffer.readSample();

		lights[LIGHT_SWITCHA].setBrightness(lightSwitchA);
		lights[LIGHT_SWITCHB].setBrightness(lightSwitchB);
		lights[LIGHT_COMP].setBrightness(lightComp);
		outputs[OUTPUT_COMP].setVoltage(output);
	}
};


struct BufferInterpolationWidget : ModuleWidget {
	BufferInterpolationWidget(BufferInterpolation* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Dicotyledon.svg")));

		//
		//			INPUT
		//
		addInput(createInputCentered<PJ301MPort>
			(mm2px(Vec(6.5087495, 45.702858)), module, BufferInterpolation::INPUT_COMPA));
		addInput(createInputCentered<PJ301MPort>
			(mm2px(Vec(23.971251, 45.702854)), module, BufferInterpolation::INPUT_COMPB));

		addInput(createInputCentered<PJ301MPort>
			(mm2px(Vec(6.5087495, 79.195808)), module, BufferInterpolation::INPUT_SWITCHA));
		addInput(createInputCentered<PJ301MPort>
			(mm2px(Vec(23.971251, 79.195808)), module, BufferInterpolation::INPUT_SWITCHB));

		addInput(createInputCentered<PJ301MPort>
			(mm2px(Vec(15.24, 94.932816)), module, BufferInterpolation::INPUT_SWITCH));

		//
		//			OUTPUT
		//
		addOutput(createOutputCentered<PJ301MPort>
			(mm2px(Vec(15.24, 59.228657)), module, BufferInterpolation::OUTPUT_COMP));
		addOutput(createOutputCentered<PJ301MPort>
			(mm2px(Vec(15.24, 108.32097)), module, BufferInterpolation::OUTPUT_SWITCH));
		//
		//			PARAM
		//
		addParam(createParamCentered<RoundBlackKnob>
			(mm2px(Vec(15.24, 27.929842)), module, BufferInterpolation::PARAM_CONTINUOUS));
		//
		//			LIGHT
		//
		addChild(createLightCentered<SmallLight<WhiteLight>>
			(mm2px(Vec(6.5087495, 86.75219)), module, BufferInterpolation::LIGHT_SWITCHA));
		addChild(createLightCentered<SmallLight<WhiteLight>>
			(mm2px(Vec(23.971251, 86.75219)), module, BufferInterpolation::LIGHT_SWITCHB));
		addChild(createLightCentered<SmallLight<WhiteLight>>
			(mm2px(Vec(15.24, 50.504276)), module, BufferInterpolation::LIGHT_COMP));

	
	}
};


Model* modelBufferInterpolation = createModel<BufferInterpolation, BufferInterpolationWidget>("BufferInterpolation");