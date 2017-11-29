#include "cfbis.hpp"


Plugin *plugin;

void init(rack::Plugin *p) {
	plugin = p;
	p->slug = "cfbis";
#ifdef VERSION
	p->version = TOSTRING(VERSION);
#endif

p->addModel(createModel<CUBEWidget>("cfbis", "CUBE", "Cube", BLANK_TAG));
}
