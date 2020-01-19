#include "plugin.hpp"


struct MASTER : Module {
	enum ParamIds {
		GAIN_PARAM,
		ON_PARAM,
		ONSTATE_PARAM,
		NUM_PARAMS
	};
	enum InputIds {

		NUM_INPUTS
	};
	enum OutputIds {
		SEND2_OUTPUT,
		SEND1_OUTPUT,
		LEFT_OUTPUT,
		RIGHT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		ON_LIGHT,
		ENUMS(LEVEL_LIGHTS, 11),
		ENUMS(LEVEL2_LIGHTS, 11),
		NUM_LIGHTS
	};

float messages[2][8] = {{NAN,NAN,NAN,NAN,NAN,NAN,NAN,NAN},{NAN,NAN,NAN,NAN,NAN,NAN,NAN,NAN}};
bool expanded = false;

dsp::SchmittTrigger onTrigger;

float SIGNAL = 0.0 ;
int lightState[11] = {};

float SIGNAL2 = 0.0 ;
int lightState2[11] = {};


	MASTER() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(GAIN_PARAM, 0.0f, 2.0f, 1.0f, "Gain");
		configParam(ON_PARAM, 0.0f, 1.0f, 0.0f);
		configParam(ONSTATE_PARAM, 0.0f, 1.0f, 1.0f);
		leftExpander.producerMessage  = messages[0];
		leftExpander.consumerMessage  = messages[1];
	}
void process(const ProcessArgs &args) override {

/////////////////////////////////////////////////////////////////////////////////////////////////MESSAGES
		float *message = (float *)(leftExpander.producerMessage);

			//if (expanded & (clkout>0)) {message[9]=10;clkout-=1;} else message[9]=0;
			//if (expanded & (rstout>0)) {message[11]=10;rstout-=1;} else message[11]=0;

		leftExpander.messageFlipRequested = true;

		if (leftExpander.module) {
			if (leftExpander.module->model == modelTRACK) { 
				expanded=true;
				outputs[LEFT_OUTPUT].setVoltage(message[0]);
				outputs[RIGHT_OUTPUT].setVoltage(message[1]);
				outputs[SEND1_OUTPUT].setVoltage(message[2]);
				outputs[SEND2_OUTPUT].setVoltage(message[3]);
				
			} else {expanded=false;}
		} else {expanded=false;}

/////////////////////////////////////////////////////////////////////////////////////////////////
	if (onTrigger.process(params[ON_PARAM].getValue()))
			{if (params[ONSTATE_PARAM].getValue() == 0) params[ONSTATE_PARAM].setValue(1); 
						else params[ONSTATE_PARAM].setValue(0);}
	if (params[ONSTATE_PARAM].getValue()==1) lights[ON_LIGHT].setBrightness(1); else lights[ON_LIGHT].setBrightness(0);

/////////////////////////////////////////////////////////////////////////////////////////////////

	SIGNAL = (outputs[RIGHT_OUTPUT].getVoltage()*params[GAIN_PARAM].getValue()) *params[ONSTATE_PARAM].getValue();
	for (int i = 0; i < 11; i++) {
		if (SIGNAL> i) {if (i<10) lightState[i]=5000;else lightState[i]=20000;}
	}
	for (int i = 0; i < 11; i++) {
		if (lightState[i]> 0) {lightState[i]=lightState[i]-1;lights[LEVEL_LIGHTS + i].setBrightness(1);} 
			else lights[LEVEL_LIGHTS + i].setBrightness(0);
	}
outputs[RIGHT_OUTPUT].setVoltage(SIGNAL);

	SIGNAL2 = (outputs[LEFT_OUTPUT].getVoltage()*params[GAIN_PARAM].getValue()) *params[ONSTATE_PARAM].getValue();
	for (int i = 0; i < 11; i++) {
		if (SIGNAL2> i) {if (i<10) lightState2[i]=5000;else lightState2[i]=20000;}
	}
	for (int i = 0; i < 11; i++) {
		if (lightState2[i]> 0) {lightState2[i]=lightState2[i]-1;lights[LEVEL2_LIGHTS + i].setBrightness(1);} 
			else lights[LEVEL2_LIGHTS + i].setBrightness(0);
	}
outputs[LEFT_OUTPUT].setVoltage(SIGNAL2);

}

};

struct FaderSlider : app::SvgSlider {
	FaderSlider() {
		maxHandlePos = app::mm2px(math::Vec(0, 0).plus(math::Vec(-2.5f, 0)));
		minHandlePos = app::mm2px(math::Vec(0, 23).plus(math::Vec(-2.5f, 0)));
		setBackgroundSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LFMSlider.svg")));
	}
};

struct FaderSliderWhite : FaderSlider {
	FaderSliderWhite() {
		setHandleSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/fcap.svg")));
	}
};

struct MASTERWidget : ModuleWidget {
	MASTERWidget(MASTER *module) {
setModule(module);
setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/MASTER.svg")));

	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));

	addParam(createParam<FaderSliderWhite>(Vec(33.5, 190), module, MASTER::GAIN_PARAM));

     	addParam(createParam<LEDButton>(Vec(36, 208-55), module, MASTER::ON_PARAM));
	addChild(createLight<MediumLight<BlueLight>>(Vec(40.4, 212.4-55), module, MASTER::ON_LIGHT));

	addOutput(createOutput<PJ301MPort>(Vec(9, 321-10), module, MASTER::SEND2_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(9, 321+15), module, MASTER::SEND1_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(56, 321-10), module, MASTER::LEFT_OUTPUT));
	addOutput(createOutput<PJ301MPort>(Vec(56, 321+15), module, MASTER::RIGHT_OUTPUT));

	for (int i = 0; i < 11; i++) {
		if (i<10) addChild(createLight<MediumLight<BlueLight>>(Vec(64, 159-i*11), module, MASTER::LEVEL_LIGHTS + i));
			else addChild(createLight<MediumLight<RedLight>>(Vec(64, 159-i*11), module, MASTER::LEVEL_LIGHTS + i));
	}
	for (int i = 0; i < 11; i++) {
		if (i<10) addChild(createLight<MediumLight<BlueLight>>(Vec(16, 159-i*11), module, MASTER::LEVEL2_LIGHTS + i));
			else addChild(createLight<MediumLight<RedLight>>(Vec(16, 159-i*11), module, MASTER::LEVEL2_LIGHTS + i));
	}
}
};


Model *modelMASTER = createModel<MASTER, MASTERWidget>("MASTER");
