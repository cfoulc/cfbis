#include "plugin.hpp"


struct CHANNEL : Module {
	enum ParamIds {
		SEND2_PARAM,
		SEND1_PARAM,
		PAN_PARAM,
	        GAIN_PARAM,
		ON_PARAM,
		ONSTATE_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		LEFT_INPUT,
		RIGHT_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		LEFT_OUTPUT,
		RIGHT_OUTPUT,
		SEND1_OUTPUT,
		SEND2_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		ON_LIGHT,
		ENUMS(LEVEL_LIGHTS, 11),
		NUM_LIGHTS
	};
float messages[2][8] = {{NAN,NAN,NAN,NAN,NAN,NAN,NAN,NAN},{NAN,NAN,NAN,NAN,NAN,NAN,NAN,NAN}};
float mods[4]={0.0f,0.0f,0.0f,0.0f};
bool expanded = false;


dsp::SchmittTrigger onTrigger;
float SIGNAL = 0.0 ;
float pl = 0.0 ;
float pr = 0.0 ;
int lightState[11] = {};

	CHANNEL() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(SEND2_PARAM, 0.0f, 2.0f, 0.0f, "Send 2");
		configParam(SEND1_PARAM, 0.0f, 2.0f, 0.0f, "Send 1");
		configParam(PAN_PARAM, -1.0f, 1.0f, 0.0f, "Pan");
		configParam(ON_PARAM, 0.0f, 1.0f, 0.0f);
		configParam(ONSTATE_PARAM, 0.0f, 1.0f, 1.0f);
		configParam(GAIN_PARAM, 0.0f, 2.0f, 1.0f, "Gain");

		leftExpander.producerMessage  = messages[0];
		leftExpander.consumerMessage  = messages[1];
	}

/////////////////////////////////////////////////////////////////////////////////////////////////EXPANDER
	void processExpander(float *message) {

		//if (inputs[LEFT_INPUT].active) {
			message[0]=outputs[LEFT_OUTPUT].getVoltage();
			message[1]=outputs[RIGHT_OUTPUT].getVoltage();
			message[2]=outputs[SEND1_OUTPUT].getVoltage();
			message[3]=outputs[SEND2_OUTPUT].getVoltage();
		//} //else for (int i=0;i<4message[0]=NAN;

	}

void process(const ProcessArgs &args) override {


/////////////////////////////////////////////////////////////////////////////////////////////////MESSAGES
		float *message = (float *)(leftExpander.producerMessage);

			//if (expanded & (clkout>0)) {message[9]=10;clkout-=1;} else message[9]=0;
			//if (expanded & (rstout>0)) {message[11]=10;rstout-=1;} else message[11]=0;

		leftExpander.messageFlipRequested = true;

		if (leftExpander.module) {
			if (leftExpander.module->model == modelCHANNEL) { 
				expanded=true;
				for (int i=0; i<4; i++) mods[i]=message[i];
				
			} else {expanded=false;for (int i=0; i<4; i++) mods[i]=NAN;}
		} else {expanded=false;for (int i=0; i<4; i++) mods[i]=NAN;}



/////////////////////////////////////////////////////////////////////////////////////////////////
	if (onTrigger.process(params[ON_PARAM].getValue()))
			{if (params[ONSTATE_PARAM].getValue() == 0) params[ONSTATE_PARAM].setValue(1); 
						else params[ONSTATE_PARAM].setValue(0);}
	if (params[ONSTATE_PARAM].getValue()==1) lights[ON_LIGHT].setBrightness(1); else lights[ON_LIGHT].setBrightness(0);

/////////////////////////////////////////////////////////////////////////////////////////////////
SIGNAL = (inputs[LEFT_INPUT].getVoltage());
	if (!std::isnan(mods[2])) outputs[SEND1_OUTPUT].setVoltage((SIGNAL*params[SEND1_PARAM].getValue())+mods[2]);
		else outputs[SEND1_OUTPUT].setVoltage(SIGNAL*params[SEND1_PARAM].getValue());
	if (!std::isnan(mods[3])) outputs[SEND2_OUTPUT].setVoltage((SIGNAL*params[SEND2_PARAM].getValue())+mods[3]);
		else outputs[SEND2_OUTPUT].setVoltage(SIGNAL*params[SEND2_PARAM].getValue());

	SIGNAL = (inputs[LEFT_INPUT].getVoltage()*params[GAIN_PARAM].getValue()) *params[ONSTATE_PARAM].getValue();
	for (int i = 0; i < 11; i++) {
		if (SIGNAL> i) {if (i<10) lightState[i]=5000;else lightState[i]=20000;}
	}
	for (int i = 0; i < 11; i++) {
		if (lightState[i]> 0) {lightState[i]=lightState[i]-1;lights[LEVEL_LIGHTS + i].setBrightness(1);} 
			else lights[LEVEL_LIGHTS + i].setBrightness(0);
	}
	if (params[PAN_PARAM].getValue()>=0) {pl=1.f-params[PAN_PARAM].getValue();pr=1.f;}
			else {pr=1.f+params[PAN_PARAM].getValue();pl=1.f;}

	if (!std::isnan(mods[0])) outputs[LEFT_OUTPUT].setVoltage(SIGNAL*pl+mods[0]);
		else outputs[LEFT_OUTPUT].setVoltage(SIGNAL*pl);
	if (!std::isnan(mods[1])) outputs[RIGHT_OUTPUT].setVoltage(SIGNAL*pr+mods[1]);
		else outputs[RIGHT_OUTPUT].setVoltage(SIGNAL*pr);



		if (rightExpander.module) {
			if ((rightExpander.module->model == modelOUTS) or (rightExpander.module->model == modelCHANNEL)) {
				processExpander((float *)(rightExpander.module->leftExpander.consumerMessage));
			}
		}
}
};

struct FaderSlider : app::SvgSlider {
	FaderSlider() {
		maxHandlePos = (math::Vec(0, 0).plus(math::Vec(-2.5f, 0)));
		minHandlePos = (math::Vec(0, 68).plus(math::Vec(-2.5f, 0)));
		setBackgroundSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LFMSlider.svg")));
	}
};

struct FaderSliderWhite : FaderSlider {
	FaderSliderWhite() {
		setHandleSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/fcap.svg")));
	}
};

struct CHANNELWidget : ModuleWidget {
	CHANNELWidget(CHANNEL *module) {
setModule(module);
setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CHANNEL.svg")));

	addChild(createWidget<ScrewSilver>(Vec(15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createWidget<ScrewSilver>(Vec(15, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-30, 365)));


	addParam(createParam<Trimpot>(Vec(36, 47), module, CHANNEL::SEND1_PARAM));
	addParam(createParam<Trimpot>(Vec(36, 47+35), module, CHANNEL::SEND2_PARAM));
	addParam(createParam<Trimpot>(Vec(36, 47+70), module, CHANNEL::PAN_PARAM));
    	//addParam(createParam<RoundLargeBlackKnob>(Vec(27, 247), module, CHANNEL::GAIN_PARAM));
	addParam(createParam<FaderSliderWhite>(Vec(33.5, 190), module, CHANNEL::GAIN_PARAM));

     	addParam(createParam<LEDButton>(Vec(36, 208-55), module, CHANNEL::ON_PARAM));
	addChild(createLight<MediumLight<BlueLight>>(Vec(40.4, 212.4-55), module, CHANNEL::ON_LIGHT));

	addInput(createInput<PJ301MPort>(Vec(33, 321), module, CHANNEL::LEFT_INPUT));
	//addInput(createInput<PJ301MPort>(Vec(41, 321), module, CHANNEL::RIGHT_INPUT));

	for (int i = 0; i < 11; i++) {
		if (i<10) addChild(createLight<MediumLight<BlueLight>>(Vec(68, 159-i*11), module, CHANNEL::LEVEL_LIGHTS + i));
			else addChild(createLight<MediumLight<RedLight>>(Vec(68, 159-i*11), module, CHANNEL::LEVEL_LIGHTS + i));
	}
}
};


Model *modelCHANNEL = createModel<CHANNEL, CHANNELWidget>("CHANNEL");
