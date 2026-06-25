#ifndef EDITOR_PANEL_ENTITY_VIEWER
#define EDITOR_PANEL_ENTITY_VIEWER

#include "panel.hpp"
#include "real/scene/entity.hpp"

namespace editor {

class PanelEntityViewer : public Panel {
public:
    PanelEntityViewer(Shared<real::Instance> _instance);
    ~PanelEntityViewer();

    void draw();

public:
    real::EntityHandle handle;
};

}

#endif