#ifndef EDITOR_PANEL_SCENE_VIEW
#define EDITOR_PANEL_SCENE_VIEW

#include "panel.hpp"
#include "real/core/types.hpp"
#include "real/scene/entity.hpp"
#include "real/scene/scene.hpp"

namespace editor {

class PanelSceneView : public Panel {
public:
    PanelSceneView(Shared<real::Instance> _instance, Shared<real::Scene> _scene);
    ~PanelSceneView();

    void draw() override;

public:
    void render_properties();

public:
    real::EntityHandle handle;
    Shared<real::Scene> scene;
};

}

#endif