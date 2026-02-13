#include <iostream>

#include "spe_shader.h"

GPU_SPE::~GPU_SPE() {
	glDeleteTextures(1, &bedrockTexture);
	glDeleteTextures(1, &tempBedrockTexture);

	glDeleteTextures(1, &streamTexture);
	glDeleteTextures(1, &tempStreamTexture);

	glDeleteTextures(1, &upliftTexture);

	glDeleteTextures(1, &steepestTexture);

	release_program(initShader);
	release_program(simulationShader);
	release_program(precalcShader);
	release_program(simulationShader2);
	release_program(precalcShader2);
}

void GPU_SPE::Init(int inSizeX, int inSizeY) {
	// Prepare data for first step

	sizeX = inSizeX-2;
	sizeY = inSizeY-2;

	nx = inSizeX;
	ny = inSizeY;

	// Prepare shader & Init buffer - Just done once
	std::string fullPathInit = "./data/shaders/texture_init.comp";
	initShader = read_program(fullPathInit.c_str());

	std::string fullPath = "./data/shaders/spe_shader.comp";
	simulationShader = read_program(fullPath.c_str());

	std::string fullPathPrecalc = "./data/shaders/spe_shader_precalc.comp";
	precalcShader = read_program(fullPathPrecalc.c_str());

	std::string fullPath2 = "./data/shaders/spe_shader2.comp";
	simulationShader2 = read_program(fullPath2.c_str());

	std::string fullPathPrecalc2 = "./data/shaders/spe_shader_precalc2.comp";
	precalcShader2 = read_program(fullPathPrecalc2.c_str());

	if (bedrockTexture == 0) glGenTextures(1, &bedrockTexture);
	glBindTexture(GL_TEXTURE_2D, bedrockTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx, ny);

	if (tempBedrockTexture == 0) glGenTextures(1, &tempBedrockTexture);
	glBindTexture(GL_TEXTURE_2D, tempBedrockTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx, ny);

	if (streamTexture == 0) glGenTextures(1, &streamTexture);
	glBindTexture(GL_TEXTURE_2D, streamTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx, ny);

	if (tempStreamTexture == 0) glGenTextures(1, &tempStreamTexture);
	glBindTexture(GL_TEXTURE_2D, tempStreamTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx, ny);

	if (upliftTexture == 0) glGenTextures(1, &upliftTexture);
	glBindTexture(GL_TEXTURE_2D, upliftTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx, ny);
	
	if (steepestTexture == 0) glGenTextures(1, &steepestTexture);
	glBindTexture(GL_TEXTURE_2D, steepestTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8I, nx, ny);

	// Uniforms - just once

	std::cout << "srcPos: " << srcPosX << " " << srcPosY << std::endl;
	std::cout << "size: " << sizeX << " " << sizeY << std::endl;
	std::cout << "nx ny: " << nx << " " << ny << std::endl;

	glUseProgram(initShader);
	glBindImageTexture(0, bedrockTexture, 0, false, 0, GL_WRITE_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(initShader, "bedrockMap"), 0);
	glBindImageTexture(1, streamTexture, 0, false, 0, GL_WRITE_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(initShader, "streamMap"), 1);
	glBindImageTexture(2, tempBedrockTexture, 0, false, 0, GL_WRITE_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(initShader, "tempBedrockMap"), 2);
	glBindImageTexture(3, tempStreamTexture, 0, false, 0, GL_WRITE_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(initShader, "tempStreamMap"), 3);
	glBindImageTexture(4, upliftTexture, 0, false, 0, GL_WRITE_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(initShader, "upliftMap"), 4);
	glBindImageTexture(5, steepestTexture, 0, false, 0, GL_WRITE_ONLY, GL_R8I); glUniform1i(glGetUniformLocation(initShader, "steepestMap"), 5);
	glUniform2i(glGetUniformLocation(initShader, "srcPos"), srcPosX, srcPosY);
	glUniform2i(glGetUniformLocation(initShader, "size"), sizeX, sizeY);
	//run init
	glUseProgram(initShader);
	glDispatchCompute((sizeX / 8) + 1, (sizeY / 8) + 1, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glUseProgram(simulationShader);
	glBindImageTexture(0, bedrockTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader, "bedrockMap"), 0);
	glBindImageTexture(1, streamTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader, "streamMap"), 1);
	glBindImageTexture(2, tempBedrockTexture, 0, false, 0, GL_WRITE_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader, "tempBedrockMap"), 2);
	glBindImageTexture(3, tempStreamTexture, 0, false, 0, GL_WRITE_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader, "tempStreamMap"), 3);
	glBindImageTexture(4, upliftTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader, "upliftMap"), 4);
	glBindImageTexture(5, steepestTexture, 0, false, 0, GL_READ_ONLY, GL_R8I); glUniform1i(glGetUniformLocation(simulationShader, "steepestMap"), 5);
	glUniform2i(glGetUniformLocation(simulationShader, "srcPos"), srcPosX, srcPosY);
	glUniform2i(glGetUniformLocation(simulationShader, "size"), sizeX, sizeY);

	glUseProgram(precalcShader);
	glBindImageTexture(0, bedrockTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(precalcShader, "bedrockMap"), 0);
	glBindImageTexture(5, steepestTexture, 0, false, 0, GL_WRITE_ONLY, GL_R8I); glUniform1i(glGetUniformLocation(precalcShader, "steepestMap"), 5);
	glUniform2i(glGetUniformLocation(precalcShader, "srcPos"), srcPosX, srcPosY);
	glUniform2i(glGetUniformLocation(precalcShader, "size"), sizeX, sizeY);

	// dual buffering
	glUseProgram(simulationShader2);
	glBindImageTexture(0, bedrockTexture, 0, false, 0, GL_WRITE_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader2, "bedrockMap"), 0);
	glBindImageTexture(1, streamTexture, 0, false, 0, GL_WRITE_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader2, "streamMap"), 1);
	glBindImageTexture(2, tempBedrockTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader2, "tempBedrockMap"), 2);
	glBindImageTexture(3, tempStreamTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader2, "tempStreamMap"), 3);
	glBindImageTexture(4, upliftTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader2, "upliftMap"), 4);
	glBindImageTexture(5, steepestTexture, 0, false, 0, GL_READ_ONLY, GL_R8I); glUniform1i(glGetUniformLocation(simulationShader2, "steepestMap"), 5);
	glUniform2i(glGetUniformLocation(simulationShader2, "srcPos"), srcPosX, srcPosY);
	glUniform2i(glGetUniformLocation(simulationShader2, "size"), sizeX, sizeY);

	glUseProgram(precalcShader2);
	glBindImageTexture(2, tempBedrockTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(precalcShader2, "tempBedrockMap"), 2);
	glBindImageTexture(5, steepestTexture, 0, false, 0, GL_WRITE_ONLY, GL_R8I); glUniform1i(glGetUniformLocation(precalcShader2, "steepestMap"), 5);
	glUniform2i(glGetUniformLocation(precalcShader2, "srcPos"), srcPosX, srcPosY);
	glUniform2i(glGetUniformLocation(precalcShader2, "size"), sizeX, sizeY);

	glUseProgram(0);
}

void GPU_SPE::Step(int n) {

	for (int i = 0; i < n; i++) {
		glUseProgram(precalcShader);
		glDispatchCompute((sizeX / 8) + 1, (sizeY / 8) + 1, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glUseProgram(simulationShader);
		glDispatchCompute((sizeX / 8) + 1, (sizeY / 8) + 1, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glUseProgram(precalcShader2);
		glDispatchCompute((sizeX / 8) + 1, (sizeY / 8) + 1, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glUseProgram(simulationShader2);
		glDispatchCompute((sizeX / 8) + 1, (sizeY / 8) + 1, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

	glUseProgram(0);
}