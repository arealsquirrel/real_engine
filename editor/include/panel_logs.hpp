#ifndef EDITOR_PANEL_LOGS_HPP
#define EDITOR_PANEL_LOGS_HPP

#include "panel.hpp"
#include "real/core/game.hpp"
#include "real/core/instance.hpp"
#include "real/core/logging.hpp"
#include "real/core/types.hpp"
namespace editor {

class PanelLogs : public Panel {
public:
	PanelLogs(real::Ref<real::Instance> _instance, real::LogSink_Buffer *lb);
	~PanelLogs() = default;

	void draw();

private:
	real::LogSink_Buffer *log_buffer;
};

}

#endif
