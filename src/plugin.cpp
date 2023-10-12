

#include "plugin.hpp"
#include <vector>
#include <math.h> 



Plugin* pluginInstance;

void init(Plugin* p) {
	pluginInstance = p;

	p->addModel(modelWilt);
	p->addModel(modelDicotyledon);
	p->addModel(modelSunflower);
	p->addModel(modelSinensis);
	p->addModel(modelHellebore);
	// p->addModel(modelMyModule);
	p->addModel(modelPruners);
}
