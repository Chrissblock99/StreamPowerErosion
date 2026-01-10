#include "spe_shader.h"


GPU_SPE::~GPU_SPE() {
	glDeleteBuffers(1, &bedrockBuffer);
	glDeleteBuffers(1, &tempBedrockBuffer);

	glDeleteBuffers(1, &streamBuffer);
	glDeleteBuffers(1, &tempStreamBuffer);

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
	size = Vec2I(hf.GetSizeX()-1, hf.GetSizeY()-1);

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

	if (bedrockBuffer == 0) glGenBuffers(1, &bedrockBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, bedrockBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * totalBufferSize, &tmpData.front(), GL_STREAM_READ);

	if (tempBedrockBuffer == 0) glGenBuffers(1, &tempBedrockBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, tempBedrockBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * totalBufferSize, &tmpZeros.front(), GL_STREAM_READ);

	if (streamBuffer == 0) glGenBuffers(1, &streamBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, streamBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * totalBufferSize, &tmpZeros.front(), GL_STREAM_READ);

	if (tempStreamBuffer == 0) glGenBuffers(1, &tempStreamBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, tempStreamBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * totalBufferSize, &tmpZeros.front(), GL_STREAM_READ);

	if (upliftTexture == 0) glGenTextures(1, &upliftTexture);
	glBindTexture(GL_TEXTURE_2D, upliftTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx, ny);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny, GL_RED, GL_FLOAT, &tmpZeros.front());
	
	if (steepestTexture == 0) glGenTextures(1, &steepestTexture);
	glBindTexture(GL_TEXTURE_2D, steepestTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8I, nx, ny);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, nx, ny, GL_RED, GL_BYTE, &tmpZeros.front());

	// Uniforms - just once

	Box2 box = hf.Array2::GetBox();
	Vector2 cellDiag = hf.CellDiagonal();
	std::cout << "srcPos: " << srcPos[0] << " " << srcPos[1] << std::endl;
	std::cout << "size: " << size[0] << " " << size[1] << std::endl;
	std::cout << "nx ny: " << nx << " " << ny << std::endl;
	std::cout << "cellDiag: " << float(cellDiag[0]) << " " << float(cellDiag[1]) << std::endl;
	std::cout << "a: " << float(box[0][0]) << " " << float(box[0][1]) << std::endl;
	std::cout << "b: " << float(box[1][0]) << " " << float(box[1][1]) << std::endl;

	glUseProgram(simulationShader);

	glUniform2i(glGetUniformLocation(simulationShader, "srcPos"), srcPos[0], srcPos[1]);
	glUniform2i(glGetUniformLocation(simulationShader, "size"), size[0], size[1]);

	glUniform1i(glGetUniformLocation(simulationShader, "nx"), nx);
	glUniform1i(glGetUniformLocation(simulationShader, "ny"), ny);
	glUniform2f(glGetUniformLocation(simulationShader, "cellDiag"), float(cellDiag[0]), float(cellDiag[1]));
	glUniform2f(glGetUniformLocation(simulationShader, "a"), float(box[0][0]), float(box[0][1]));
	glUniform2f(glGetUniformLocation(simulationShader, "b"), float(box[1][0]), float(box[1][1]));

	glUseProgram(0);

	glUseProgram(precalcShader);

	glUniform2i(glGetUniformLocation(precalcShader, "srcPos"), srcPos[0], srcPos[1]);
	glUniform2i(glGetUniformLocation(precalcShader, "size"), size[0], size[1]);

	glUniform1i(glGetUniformLocation(precalcShader, "nx"), nx);
	glUniform1i(glGetUniformLocation(precalcShader, "ny"), ny);
	glUniform2f(glGetUniformLocation(precalcShader, "cellDiag"), float(cellDiag[0]), float(cellDiag[1]));
	glUniform2f(glGetUniformLocation(precalcShader, "a"), float(box[0][0]), float(box[0][1]));
	glUniform2f(glGetUniformLocation(precalcShader, "b"), float(box[1][0]), float(box[1][1]));

	glUseProgram(0);

	glUseProgram(simulationShader2);

	glUniform2i(glGetUniformLocation(simulationShader2, "srcPos"), srcPos[0], srcPos[1]);
	glUniform2i(glGetUniformLocation(simulationShader2, "size"), size[0], size[1]);

	glUniform1i(glGetUniformLocation(simulationShader2, "nx"), nx);
	glUniform1i(glGetUniformLocation(simulationShader2, "ny"), ny);
	glUniform2f(glGetUniformLocation(simulationShader2, "cellDiag"), float(cellDiag[0]), float(cellDiag[1]));
	glUniform2f(glGetUniformLocation(simulationShader2, "a"), float(box[0][0]), float(box[0][1]));
	glUniform2f(glGetUniformLocation(simulationShader2, "b"), float(box[1][0]), float(box[1][1]));

	glUseProgram(0);

	glUseProgram(precalcShader2);

	glUniform2i(glGetUniformLocation(precalcShader2, "srcPos"), srcPos[0], srcPos[1]);
	glUniform2i(glGetUniformLocation(precalcShader2, "size"), size[0], size[1]);

	glUniform1i(glGetUniformLocation(precalcShader2, "nx"), nx);
	glUniform1i(glGetUniformLocation(precalcShader2, "ny"), ny);
	glUniform2f(glGetUniformLocation(precalcShader2, "cellDiag"), float(cellDiag[0]), float(cellDiag[1]));
	glUniform2f(glGetUniformLocation(precalcShader2, "a"), float(box[0][0]), float(box[0][1]));
	glUniform2f(glGetUniformLocation(precalcShader2, "b"), float(box[1][0]), float(box[1][1]));

	glUseProgram(0);
}

void GPU_SPE::Step(int n) {

	for (int i = 0; i < n; i++) {
		glUseProgram(precalcShader);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, bedrockBuffer);
        glBindImageTexture(5, steepestTexture, 0, false, 0, GL_WRITE_ONLY, GL_R8I); glUniform1i(glGetUniformLocation(precalcShader, "steepestMap"), 5);

		glDispatchCompute((size[0] / 8) + 1, (size[1] / 8) + 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		glUseProgram(simulationShader);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, bedrockBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, streamBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, tempBedrockBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, tempStreamBuffer);
        glBindImageTexture(4, upliftTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader, "upliftMap"), 4);
        glBindImageTexture(5, steepestTexture, 0, false, 0, GL_READ_ONLY, GL_R8I); glUniform1i(glGetUniformLocation(simulationShader, "steepestMap"), 5);

		glDispatchCompute((size[0] / 8) + 1, (size[1] / 8) + 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// dual buffering
		glUseProgram(precalcShader2);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, tempBedrockBuffer);
        glBindImageTexture(5, steepestTexture, 0, false, 0, GL_WRITE_ONLY, GL_R8I); glUniform1i(glGetUniformLocation(precalcShader2, "steepestMap"), 5);

		glDispatchCompute((size[0] / 8) + 1, (size[1] / 8) + 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		glUseProgram(simulationShader2);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, bedrockBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, streamBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, tempBedrockBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, tempStreamBuffer);
        glBindImageTexture(4, upliftTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader2, "upliftMap"), 4);
        glBindImageTexture(5, steepestTexture, 0, false, 0, GL_READ_ONLY, GL_R8I); glUniform1i(glGetUniformLocation(simulationShader2, "steepestMap"), 5);

		glDispatchCompute((size[0] / 8) + 1, (size[1] / 8) + 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
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
	return bedrockBuffer;
}

void GPU_SPE::GetData(ScalarField2& sf) {
	glGetNamedBufferSubData(bedrockBuffer, 0, sizeof(float) * totalBufferSize, tmpData.data());

	for (int i = 0; i < totalBufferSize; i++)
		sf[i] = double(tmpData[i]);

	/*double low, high;
	sf.GetRange(low, high);
	std::cout << low << " " << high << std::endl;*/
}

void GPU_SPE::GetData(ScalarField2& sf, ScalarField2& sa) {
	glGetNamedBufferSubData(bedrockBuffer, 0, sizeof(float) * totalBufferSize, tmpData.data());

	for (int i = 0; i < totalBufferSize; i++)
		sf[i] = double(tmpData[i]);

	glGetNamedBufferSubData(streamBuffer, 0, sizeof(float) * totalBufferSize, tmpData.data());

	for (int i = 0; i < totalBufferSize; i++)
		sa[i] = double(tmpData[i]);

	/*double low, high;
	sa.GetRange(low, high);
	std::cout << low << " " << high << std::endl;*/
}


