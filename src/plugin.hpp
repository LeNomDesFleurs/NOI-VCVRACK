#pragma once

#ifndef PLUGIN
#define PLUGIN

#include <rack.hpp>
#include "outils.hpp"


using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
extern Model* modelMyModule;
extern Model* modelWilt;
extern Model* modelDicotyledon;
extern Model* modelSunflower;
extern Model* modelSinensis;
extern Model* modelHellebore;
extern Model* modelPruners;
#endif /*PLUGIN*/
