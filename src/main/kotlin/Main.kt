package me.chriss99

lateinit var window: Window
lateinit var widget: TesselationWidget
lateinit var gpuSpe: SpeShader
var ongoingGpuSpe = true;

fun main() {
    window = Window("Stream Power Erosion", 1920, 1080);
    widget = TesselationWidget();
    window.widget = widget

    gpuSpe = SpeShader(256, 256);

    // Main loop
    while (!window.exit()) {
        if (ongoingGpuSpe) {
            // simulation step
            gpuSpe.step(200);
        }

        window.update();
    }

    gpuSpe.delete()
    window.delete()
}
