#ifndef __SPE_SHADER__
#define __SPE_SHADER__

#include <GL/glew.h>
#include <vector>
#include <string>

#include "shader-api.h"

class GPU_SPE {
private:
	GLuint initShader = 0;			//!< Compute shader

	GLuint simulationShader = 0;
	GLuint precalcShader = 0;
	GLuint simulationShader2 = 0;
	GLuint precalcShader2 = 0;

	GLuint bedrockTexture = 0;				//!< Bedrock elevation buffer
	GLuint tempBedrockTexture = 0;			//!< Output bedrock elevation buffer

	GLuint streamTexture = 0;				//!< Water elevation buffer
	GLuint tempStreamTexture = 0;				//!< Output water elevation buffer

	GLuint upliftTexture = 0;		//!< Uplift buffer

	GLuint steepestTexture = 0;

	int srcPosX = 1;
	int srcPosY = 1;
	int sizeX = 0;
	int sizeY = 0;
	int nx = 0;
	int ny = 0;
public:
	GPU_SPE() {};
	~GPU_SPE();

	void Init(int inSizeX, int inSizeY);
	void Step(int n);
};

#endif
