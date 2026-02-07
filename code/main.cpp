#include "window.h"
#include "terrainwidget.h"
#include "scalarfield2.h"
#include "spe_shader.h"
#include "texture.h"

static Window* window;
static TerrainRaytracingWidget* widget;
static ScalarField2 hf;
static ScalarField2 uplift;
static ScalarField2 gpu_drainage;
static GPU_SPE gpu_spe;
static Texture2D albedoTexture;
static int shadingMode;
static float brushRadius = 30;
static bool brushRadius_changed = false;
static float brushStrength = 10.0;
static bool brushStrength_changed = false;
static bool ongoing_gpu_spe = true;
static float delta_time = 100;

/*!
\brief Compute the intersection between a plane and a ray.
The intersection depth is returned if intersection occurs.
\param ray The ray.
\param t Intersection depth.
*/
static bool PlaneIntersect(const Ray& ray, double& t)
{
	double epsilon = 1e-4f;
	double x = Vector::Z * ray.Direction();
	if ((x < epsilon) && (x > -epsilon))
		return false;
	double c = Vector::Null * Vector::Z;
	double y = c - (Vector::Z * ray.Origin());
	t = y / x;
	return true;
}

/*!
\brief Reset the camera around a given 3D box.
*/
static void ResetCamera()
{
	Box2 box = hf.GetBox();
	Vector2 v = 0.5 * box.Diagonal();
	Camera cam = Camera(box.Center().ToVector(0.0) - (2.0 * v).ToVector(-Norm(v)), box.Center().ToVector(0), Vector(0.0, 0.0, 1.0), 1.0, 1.0, 5.0, 10000);
	widget->SetCamera(cam);
}

int main()
{
	// Init
	window = new Window("Stream Power Erosion", 1920, 1080);
	widget = new TerrainRaytracingWidget();
	window->SetWidget(widget);
	hf = ScalarField2(Box2(Vector2::Null, 150*1000), "../data/heightfields/hfTest2.png", 0.0, 1000.0);
	uplift = ScalarField2(Box2(Vector2::Null, 150*1000), 256, 256, 1.0);
	widget->SetHeightField(&hf);

	// gpu_spe init
	gpu_spe.Init(hf);
	gpu_spe.SetUplift(uplift);
	gpu_drainage = hf;

	albedoTexture = Texture2D(hf.GetSizeX(), hf.GetSizeY());
	albedoTexture.Fill(Color8(225, 225, 225, 255));
	widget->SetAlbedo(albedoTexture);

	ResetCamera();

	// Main loop
	while (!window->Exit()) {
		// Heightfield editing
		bool leftMouse = window->GetMousePressed(GLFW_MOUSE_BUTTON_LEFT);
		bool rightMouse = window->GetMousePressed(GLFW_MOUSE_BUTTON_RIGHT);
		if ((leftMouse) && window->GetKey(GLFW_KEY_LEFT_CONTROL)) {
			Camera cam = widget->GetCamera();
			double xpos, ypos;
			glfwGetCursorPos(window->getPointer(), &xpos, &ypos);
			Ray ray = cam.PixelToRay(int(xpos), int(ypos), window->width(), window->height());
			double t;
			if (PlaneIntersect(ray, t)) {
				uplift.Gaussian(ray(t), brushRadius * 1000, brushStrength);
				gpu_spe.SetUplift(uplift);
			}
		}
		if (ongoing_gpu_spe) {
			// simulation step
			gpu_spe.Step(200);
			gpu_spe.GetData(hf);
			widget->UpdateInternal();
		}

		window->Update();
	}
	return 0;
}
