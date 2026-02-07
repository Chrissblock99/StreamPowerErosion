#include "spe_shader.h"


GPU_SPE::~GPU_SPE() {
	glDeleteTextures(1, &bedrockTexture);
	glDeleteTextures(1, &tempBedrockTexture);

	glDeleteTextures(1, &streamTexture);
	glDeleteTextures(1, &tempStreamTexture);

	glDeleteTextures(1, &upliftTexture);

	glDeleteTextures(1, &steepestTexture);

	release_program(simulationShader);
	release_program(precalcShader);
	release_program(simulationShader2);
	release_program(precalcShader2);
}

void GPU_SPE::Init(const ScalarField2& hf) {
	// Prepare data for first step

	srcPos = Vec2I(1, 1);
	size = Vec2I(hf.GetSizeX()-2, hf.GetSizeY()-2);

	nx = hf.GetSizeX();
	ny = hf.GetSizeY();
	totalBufferSize = hf.VertexSize();

	tmpData.resize(totalBufferSize);
	for (int i = 0; i < totalBufferSize; i++)
		tmpData[i] = hf.at(i);

	std::vector<float> tmpZeros(totalBufferSize, 0.);

	// Prepare shader & Init buffer - Just done once
	std::string fullPath = "./data/shaders/spe_shader.glsl";
	simulationShader = read_program(fullPath.c_str());

	std::string fullPathPrecalc = "./data/shaders/spe_shader_precalc.glsl";
	precalcShader = read_program(fullPathPrecalc.c_str());

	std::string fullPath2 = "./data/shaders/spe_shader2.glsl";
	simulationShader2 = read_program(fullPath2.c_str());

	std::string fullPathPrecalc2 = "./data/shaders/spe_shader_precalc2.glsl";
	precalcShader2 = read_program(fullPathPrecalc2.c_str());

	if (bedrockTexture == 0) glGenTextures(1, &bedrockTexture);
	glBindTexture(GL_TEXTURE_2D, bedrockTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx, ny);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny, GL_RED, GL_FLOAT, &tmpData.front());

	if (tempBedrockTexture == 0) glGenTextures(1, &tempBedrockTexture);
	glBindTexture(GL_TEXTURE_2D, tempBedrockTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx, ny);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny, GL_RED, GL_FLOAT, &tmpZeros.front());

	if (streamTexture == 0) glGenTextures(1, &streamTexture);
	glBindTexture(GL_TEXTURE_2D, streamTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx, ny);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny, GL_RED, GL_FLOAT, &tmpZeros.front());

	if (tempStreamTexture == 0) glGenTextures(1, &tempStreamTexture);
	glBindTexture(GL_TEXTURE_2D, tempStreamTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx, ny);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny, GL_RED, GL_FLOAT, &tmpZeros.front());

	if (upliftTexture == 0) glGenTextures(1, &upliftTexture);
	glBindTexture(GL_TEXTURE_2D, upliftTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx, ny);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny, GL_RED, GL_FLOAT, &tmpZeros.front());
	
	if (steepestTexture == 0) glGenTextures(1, &steepestTexture);
	glBindTexture(GL_TEXTURE_2D, steepestTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8I, nx, ny);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny, GL_RED, GL_BYTE, &tmpZeros.front());

	// Uniforms - just once

	std::cout << "srcPos: " << srcPos[0] << " " << srcPos[1] << std::endl;
	std::cout << "size: " << size[0] << " " << size[1] << std::endl;
	std::cout << "nx ny: " << nx << " " << ny << std::endl;

	glUseProgram(simulationShader);
	glBindImageTexture(0, bedrockTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader, "bedrockMap"), 0);
	glBindImageTexture(1, streamTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader, "streamMap"), 1);
	glBindImageTexture(2, tempBedrockTexture, 0, false, 0, GL_WRITE_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader, "tempBedrockMap"), 2);
	glBindImageTexture(3, tempStreamTexture, 0, false, 0, GL_WRITE_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader, "tempStreamMap"), 3);
	glBindImageTexture(4, upliftTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader, "upliftMap"), 4);
	glBindImageTexture(5, steepestTexture, 0, false, 0, GL_READ_ONLY, GL_R8I); glUniform1i(glGetUniformLocation(simulationShader, "steepestMap"), 5);
	glUniform2i(glGetUniformLocation(simulationShader, "srcPos"), srcPos[0], srcPos[1]);
	glUniform2i(glGetUniformLocation(simulationShader, "size"), size[0], size[1]);

	glUseProgram(precalcShader);
	glBindImageTexture(0, bedrockTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(precalcShader, "bedrockMap"), 0);
	glBindImageTexture(5, steepestTexture, 0, false, 0, GL_WRITE_ONLY, GL_R8I); glUniform1i(glGetUniformLocation(precalcShader, "steepestMap"), 5);
	glUniform2i(glGetUniformLocation(precalcShader, "srcPos"), srcPos[0], srcPos[1]);
	glUniform2i(glGetUniformLocation(precalcShader, "size"), size[0], size[1]);

	// dual buffering
	glUseProgram(simulationShader2);
	glBindImageTexture(0, bedrockTexture, 0, false, 0, GL_WRITE_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader2, "bedrockMap"), 0);
	glBindImageTexture(1, streamTexture, 0, false, 0, GL_WRITE_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader2, "streamMap"), 1);
	glBindImageTexture(2, tempBedrockTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader2, "tempBedrockMap"), 2);
	glBindImageTexture(3, tempStreamTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader2, "tempStreamMap"), 3);
	glBindImageTexture(4, upliftTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader2, "upliftMap"), 4);
	glBindImageTexture(5, steepestTexture, 0, false, 0, GL_READ_ONLY, GL_R8I); glUniform1i(glGetUniformLocation(simulationShader2, "steepestMap"), 5);
	glUniform2i(glGetUniformLocation(simulationShader2, "srcPos"), srcPos[0], srcPos[1]);
	glUniform2i(glGetUniformLocation(simulationShader2, "size"), size[0], size[1]);

	glUseProgram(precalcShader2);
	glBindImageTexture(2, tempBedrockTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(precalcShader2, "tempBedrockMap"), 2);
	glBindImageTexture(5, steepestTexture, 0, false, 0, GL_WRITE_ONLY, GL_R8I); glUniform1i(glGetUniformLocation(precalcShader2, "steepestMap"), 5);
	glUniform2i(glGetUniformLocation(precalcShader2, "srcPos"), srcPos[0], srcPos[1]);
	glUniform2i(glGetUniformLocation(precalcShader2, "size"), size[0], size[1]);

	glUseProgram(0);
}

void GPU_SPE::Step(int n) {

	for (int i = 0; i < n; i++) {
		glUseProgram(precalcShader);
		glDispatchCompute((size[0] / 8) + 1, (size[1] / 8) + 1, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glUseProgram(simulationShader);
		glDispatchCompute((size[0] / 8) + 1, (size[1] / 8) + 1, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glUseProgram(precalcShader2);
		glDispatchCompute((size[0] / 8) + 1, (size[1] / 8) + 1, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glUseProgram(simulationShader2);
		glDispatchCompute((size[0] / 8) + 1, (size[1] / 8) + 1, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

	glUseProgram(0);
}

void GPU_SPE::SetDt(float dt) const {
	glUseProgram(simulationShader);
	glUniform1f(glGetUniformLocation(simulationShader, "dt"), dt);
	glUseProgram(precalcShader);
	glUniform1f(glGetUniformLocation(precalcShader, "dt"), dt);
	glUseProgram(simulationShader2);
	glUniform1f(glGetUniformLocation(simulationShader2, "dt"), dt);
	glUseProgram(precalcShader2);
	glUniform1f(glGetUniformLocation(precalcShader2, "dt"), dt);
	glUseProgram(0);
}

void GPU_SPE::SetUplift(const ScalarField2& uplift) const {
	glUseProgram(simulationShader);

	glBindTexture(GL_TEXTURE_2D, upliftTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny, GL_RED, GL_FLOAT, &uplift.GetFloatData()[0]);

	glUseProgram(simulationShader2);

	glBindTexture(GL_TEXTURE_2D, upliftTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny, GL_RED, GL_FLOAT, &uplift.GetFloatData()[0]);

	glUseProgram(0);
}

GLuint GPU_SPE::GetData() const {
	return bedrockTexture;
}

void GPU_SPE::GetData(ScalarField2& sf) {
	glGetTextureSubImage(bedrockTexture, 0, 0, 0, 0, nx, ny, 1, GL_RED, GL_FLOAT, sizeof(float) * totalBufferSize, tmpData.data());

	for (int i = 0; i < totalBufferSize; i++)
		sf[i] = double(tmpData[i]);
}

void GPU_SPE::GetData(ScalarField2& sf, ScalarField2& sa) {
	glGetTextureSubImage(bedrockTexture, 0, 0, 0, 0, nx, ny, 1, GL_RED, GL_FLOAT, sizeof(float) * totalBufferSize, tmpData.data());

	for (int i = 0; i < totalBufferSize; i++)
		sf[i] = double(tmpData[i]);

	glGetTextureSubImage(streamTexture, 0, 0, 0, 0, nx, ny, 1, GL_RED, GL_FLOAT, sizeof(float) * totalBufferSize, tmpData.data());

	for (int i = 0; i < totalBufferSize; i++)
		sa[i] = double(tmpData[i]);

	/*double low, high;
	sa.GetRange(low, high);
	std::cout << low << " " << high << std::endl;*/
}

void GPU_SPE::resetSim() {
	std::vector<float> tmpZeros(totalBufferSize, 0.);
	ScalarField2 hf = ScalarField2(Box2(Vector2::Null, 150*1000), "../data/heightfields/hfTest2.png", 0.0, 1000.0);

	tmpData.resize(totalBufferSize);
	for (int i = 0; i < totalBufferSize; i++)
		tmpData[i] = hf.at(i);

	glBindTexture(GL_TEXTURE_2D, bedrockTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny, GL_RED, GL_FLOAT, &tmpData.front());
	glBindTexture(GL_TEXTURE_2D, streamTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny, GL_RED, GL_FLOAT, &tmpZeros.front());
}

void GPU_SPE::outputData() {
	ScalarField2 data = ScalarField2(Box2(Vector2::Null, 150 * 1000), 256, 256, 0);

	glGetTextureSubImage(streamTexture, 0, 0, 0, 0, nx, ny, 1, GL_RED, GL_FLOAT, sizeof(float) * totalBufferSize, tmpData.data());

	for (int i = 0; i < totalBufferSize; i++)
		data[i] = sqrt(double(tmpData[i]) + 1.0);

	std::string fileName = "/tmp/stream";
	data.Save(fileName.c_str());



	glGetTextureSubImage(upliftTexture, 0, 0, 0, 0, nx, ny, 1, GL_RED, GL_FLOAT, sizeof(float) * totalBufferSize, tmpData.data());

	for (int i = 0; i < totalBufferSize; i++)
		data[i] = double(tmpData[i]);

	fileName = "/tmp/uplift";
	data.Save(fileName.c_str());



	glGetTextureSubImage(bedrockTexture, 0, 0, 0, 0, nx, ny, 1, GL_RED, GL_FLOAT, sizeof(float) * totalBufferSize, tmpData.data());

	for (int i = 0; i < totalBufferSize; i++)
		data[i] = double(tmpData[i]);

	fileName = "/tmp/height";
	data.Save(fileName.c_str());

	/*double low, high;
	data.GetRange(low, high);
	std::cout << low << " " << high << std::endl;*/
}


