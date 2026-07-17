#ifndef EDITOR_PANEL_SCENE_VIEW
#define EDITOR_PANEL_SCENE_VIEW

#include "panel.hpp"
#include "real/core/types.hpp"
#include "real/scene/entity.hpp"
#include "real/scene/scene.hpp"

namespace editor {

class PanelSceneView : public Panel {
public:
    PanelSceneView(real::Ref<real::Instance> _instance, real::Ref<real::Scene> _scene);
    ~PanelSceneView();

    void draw() override;

public:
    void render_properties();

public:
    real::EntityHandle handle;
	real::Ref<real::Scene> scene;
};

}

#endif
