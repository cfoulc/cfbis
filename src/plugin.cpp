#include "plugin.hpp"


Plugin *pluginInstance;

void init(rack::Plugin *p) {
	pluginInstance = p;
	
	p->addModel(modelCHANNEL);
	p->addModel(modelOUTS);
	p->addModel(modelDUALFX);
	p->addModel(modelBUFFER);

}
