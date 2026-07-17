
#include "panel.hpp"
#include "real/core/instance.hpp"

namespace editor {

Panel::Panel(real::Ref<real::Instance> _instance) 
	: instance(_instance), panelID() {}

Panel::~Panel() {

}

}

