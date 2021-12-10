
#include "plugin.hpp"



struct BUFFER : Module {
	enum ParamIds {
		
		LENGTH_PARAM,
		
		NUM_PARAMS
	};
	enum InputIds {
		IN_INPUT,

		NUM_INPUTS
		
	};
	enum OutputIds {
		X_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};


	float buf[10000] ={};
	float x = 0;
	int pos = 0;
	int length = 0;
	float l_gain ;
	int l_affi ;

	bool MODE_STATE = false ;
	dsp::SchmittTrigger modeTrigger;


BUFFER() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);


		configParam(LENGTH_PARAM, 0.0f, 1.0f, 0.5f, "LENGTH");

}


json_t *dataToJson() override {
		json_t *rootJ = json_object();
		

		json_object_set_new(rootJ, "modestate", json_integer(MODE_STATE));
		return rootJ;
		}

void dataFromJson(json_t *rootJ) override {
		

		json_t *modestateJ = json_object_get(rootJ, "modestate");
		if (modestateJ)
			MODE_STATE = json_integer_value(modestateJ);
	
	}





void process(const ProcessArgs &args) override { 





		length = int(params[LENGTH_PARAM].getValue()*9998.0f)+1;
		l_affi =0;



	buf[pos]=(inputs[IN_INPUT].getVoltage()) ; 

	x = float(pos) ;
	if (pos<9999) pos=pos+1; else pos=0;


	if ((pos-length)>0)
		outputs[X_OUTPUT].setVoltage(clamp(buf[pos-length],-10.0f,10.0f));
	else
		outputs[X_OUTPUT].setVoltage(clamp(buf[9999+pos-length],-10.0f,10.0f));
 


}

};

struct BUFFERDisplay : TransparentWidget {
	BUFFER *module;

	BUFFERDisplay() {
	
		
	}
	
	void draw(const DrawArgs &args) override {
if (module) {
		nvgStrokeWidth(args.vg,1.2);
		nvgStrokeColor(args.vg, nvgRGBA(0x28, 0xb0, 0xf3, 0xff ));
		{
			nvgBeginPath(args.vg);
			nvgMoveTo(args.vg, clamp(module->buf[int(module->x)]*4.0f,-45.0f,45.0f),0.0f);
			for (int i=1;i<module->length; i++) {if ((module->x-i)>0) nvgLineTo(args.vg, clamp(module->buf[int(module->x)-i]*4.0f,-45.0f,45.0f), -200.0*(i+1)/(module->length)); 
							       else nvgLineTo(args.vg, clamp(module->buf[9999+int(module->x)-i]*4.0f,-45.0f,45.0f), -200.0*(i+1)/(module->length));
								}
		}
		nvgLineCap(args.vg, NVG_ROUND);
		nvgMiterLimit(args.vg, 20.0f);
		nvgGlobalCompositeOperation(args.vg, NVG_LIGHTER);
		nvgStroke(args.vg);

	}
};
};

struct MBDisplay : TransparentWidget {
	BUFFER *module;

	MBDisplay() {
		
	}
	
	void draw(const DrawArgs &args) override {

float gainX = module ? module->l_gain : 1.0f;
int affich = module ? module->l_affi : 0;
float d=9.3f;

		if (affich==1) {
		float xx = d*sin(-(gainX*0.17+0.15)*M_PI) ;
		float yy = d*cos((gainX*0.17+0.15)*M_PI) ;

		
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, 0,0, d);
			nvgFillColor(args.vg, nvgRGBA(0x00, 0x00, 0x00, 0xff));
			nvgFill(args.vg);	
		
			nvgStrokeWidth(args.vg,1.2);
			nvgStrokeColor(args.vg, nvgRGBA(0xff, 0xff, 0xff, 0xff));
			{
				nvgBeginPath(args.vg);
				nvgMoveTo(args.vg, 0,0);
				nvgLineTo(args.vg, xx,yy);
				nvgClosePath(args.vg);
			}
			nvgStroke(args.vg);
		}

	}
};


///////////////////

struct BUFFERWidget : ModuleWidget {
	BUFFERWidget(BUFFER *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/BUFFER.svg")));


	addChild(createWidget<ScrewSilver>(Vec(0, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-15, 0)));
	addChild(createWidget<ScrewSilver>(Vec(0, 365)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x-15, 365)));

	{
		BUFFERDisplay *bdisplay = new BUFFERDisplay();
		bdisplay->box.pos = Vec(67.5, 270);
		bdisplay->module = module ;
		addChild(bdisplay);
	}





	addInput(createInput<PJ301MPort>(Vec(20, 6.5), module, BUFFER::IN_INPUT));


	addParam(createParam<RoundBlackKnob>(Vec(53, 284), module, BUFFER::LENGTH_PARAM)); 
	{
		MBDisplay *pdisplay = new MBDisplay();
		pdisplay->box.pos = Vec(92.8, 293.2);
		pdisplay->module = module;
		addChild(pdisplay);
	}
	addOutput(createOutput<PJ301MPort>(Vec(box.size.x-15-31, 6.5), module, BUFFER::X_OUTPUT)); 
	
}
};

Model *modelBUFFER = createModel<BUFFER, BUFFERWidget>("BUFFER");