#include "plugin.hpp"


struct DUALFX : Module {
	enum ParamIds {
		
		NUM_PARAMS
	};
	enum InputIds {

		NUM_INPUTS
	};
	enum OutputIds {

		NUM_OUTPUTS
	};


	DUALFX() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
	}

};



struct DUALFXWidget : ModuleWidget {
	DUALFXWidget(DUALFX *module) {
setModule(module);
setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/DUALFX.svg")));
}
};


Model *modelDUALFX = createModel<DUALFX, DUALFXWidget>("DUALFX");
