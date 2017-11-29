


#include "cfbis.hpp"
 


struct CUBE : Module {
	enum ParamIds {
		
		NUM_PARAMS
	};
	enum InputIds {

		NUM_INPUTS
	};
	enum OutputIds {

		NUM_OUTPUTS
	};
int t = 0;

	CUBE() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {}
	void step() override;

};


void CUBE::step() {

}

struct CUBEDisplay : TransparentWidget {
	CUBE *module;
	int frame = 0;
	float xx[12] = {-1, 1, 1,-1,-1, 1, 1,-1};
	float yy[12] = {-1,-1, 1, 1,-1,-1, 1, 1};
	float zz[12] = {-1,-1,-1,-1, 1, 1, 1, 1};
	float x[12] = {};
	float y[12] = {};
	float z[12] = {};
float d =0;
float theta=0 ;
int repx= 55;
int repy=80;

	CUBEDisplay() {
		
	}
	
	void draw(NVGcontext *vg) override {

         for(int i=0; i<12; i++)
        {
	d = sqrt(yy[i]*yy[i] + zz[i]*zz[i]);
	theta = atan2(yy[i],zz[i])+frame/10.0;
	x[i] = xx[i]; //*cos(frame/10.0)
	y[i] = d * sin(theta); 
	z[i] = d * cos(theta);

	d = sqrt(x[i]*x[i] + z[i]*z[i]);
	theta = atan2(x[i],z[i])+frame/20.0;
	x[i] = d * sin(theta); 
	y[i] = y[i]; 
	z[i] = d * cos(theta);

        }

		nvgStrokeColor(vg, nvgRGBA(0x28, 0xb0, 0xf3, 0xff));
		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, x[0]*20+ repx,y[0]*20+ repy);
			nvgLineTo(vg, x[1]*20+ repx,y[1]*20+ repy);
			nvgLineTo(vg, x[2]*20+ repx,y[2]*20+ repy);
			nvgLineTo(vg, x[3]*20+ repx,y[3]*20+ repy);
			nvgClosePath(vg);
		}
		nvgStroke(vg);

		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, x[4]*20+ repx,y[4]*20+ repy);
			nvgLineTo(vg, x[5]*20+ repx,y[5]*20+ repy);
			nvgLineTo(vg, x[6]*20+ repx,y[6]*20+ repy);
			nvgLineTo(vg, x[7]*20+ repx,y[7]*20+ repy);
			nvgClosePath(vg);
		}
		nvgStroke(vg);

		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, x[0]*20+ repx,y[0]*20+ repy);
			nvgLineTo(vg, x[4]*20+ repx,y[4]*20+ repy);
			nvgClosePath(vg);
		}
		nvgStroke(vg);

		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, x[1]*20+ repx,y[1]*20+ repy);
			nvgLineTo(vg, x[5]*20+ repx,y[5]*20+ repy);
			nvgClosePath(vg);
		}
		nvgStroke(vg);

		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, x[2]*20+ repx,y[2]*20+ repy);
			nvgLineTo(vg, x[6]*20+ repx,y[6]*20+ repy);
			nvgClosePath(vg);
		}
		nvgStroke(vg);

		{
			nvgBeginPath(vg);
			nvgMoveTo(vg, x[3]*20+ repx,y[3]*20+ repy);
			nvgLineTo(vg, x[7]*20+ repx,y[7]*20+ repy);
			nvgClosePath(vg);
		}
		nvgStroke(vg);

		
		frame++;
	}
};


CUBEWidget::CUBEWidget() {
	CUBE *module = new CUBE();
	setModule(module);
	box.size = Vec(15*8, 380);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/CUBE.svg")));
		addChild(panel);

	}
	addChild(createScrew<ScrewSilver>(Vec(15, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x-30, 0)));
	addChild(createScrew<ScrewSilver>(Vec(15, 365)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x-30, 365)));

	{
		CUBEDisplay *display = new CUBEDisplay();
		display->module = module;
		display->box.pos = Vec(5, 40);
		display->box.size = Vec(130, 250);
		addChild(display);
	}

}
