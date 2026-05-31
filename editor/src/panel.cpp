
#include "panel.hpp"
#include "real/core/instance.hpp"

namespace editor {

Panel::Panel(Shared<real::Instance> _instance) 
	: instance(_instance), panelID() {}

Panel::~Panel() {

}

}

