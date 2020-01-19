#include "plugin.hpp"


Plugin *pluginInstance;

void init(rack::Plugin *p) {
	pluginInstance = p;
	
	p->addModel(modelTRACK);
	p->addModel(modelMASTER);

}
