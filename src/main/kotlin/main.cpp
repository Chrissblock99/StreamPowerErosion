#include "window.h"
#include "tesselationWidget.h"
#include "spe_shader.h"

static Window* window;
static TesselationWidget* widget;
static GPU_SPE gpu_spe;
static bool ongoing_gpu_spe = true;

int main()
{
	// Init
	window = new Window("Stream Power Erosion", 1920, 1080);
	widget = new TesselationWidget();
	window->SetWidget(widget);

	// gpu_spe init
	gpu_spe.Init(256, 256);

	// Main loop
	while (!window->Exit()) {
		if (ongoing_gpu_spe) {
			// simulation step
			gpu_spe.Step(200);
		}

		window->Update();
	}
	return 0;
}
