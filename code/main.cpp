#include "window.h"
#include "tesselationWidget.h"
#include "scalarfield2.h"
#include "spe_shader.h"
#include "texture.h"

static Window* window;
static TesselationWidget* widget;
static ScalarField2 hf;
static ScalarField2 uplift;
static GPU_SPE gpu_spe;
static float brushRadius = 30;
static float brushStrength = 10.0;
static bool ongoing_gpu_spe = true;

int main()
{
	// Init
	window = new Window("Stream Power Erosion", 1920, 1080);
	widget = new TesselationWidget();
	window->SetWidget(widget);
	hf = ScalarField2(Box2(Vector2::Null, 150*1000), "../data/heightfields/hfTest2.png", 0.0, 1000.0);
	uplift = ScalarField2(Box2(Vector2::Null, 150*1000), "../data/uplifts/lambda.png", 0.4, 10.0);

	// gpu_spe init
	gpu_spe.Init(hf);
	gpu_spe.SetUplift(uplift);

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
