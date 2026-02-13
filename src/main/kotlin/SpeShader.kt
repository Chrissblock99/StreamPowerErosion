package me.chriss99

import org.lwjgl.opengl.GL45.*

class SpeShader(inSizeX: Int, inSizeY: Int) {
	var srcPosX = 1
	var srcPosY = 1

	val sizeX = inSizeX - 2
	val sizeY = inSizeY - 2

	val nx = inSizeX
	val ny = inSizeY


	val initShader = readProgram("./src/main/glsl/texture_init.comp")

	val simulationShader = readProgram("./src/main/glsl/spe_shader.comp")
	val precalcShader = readProgram("./src/main/glsl/spe_shader_precalc.comp")

	val simulationShader2 = readProgram("./src/main/glsl/spe_shader2.comp")
	val precalcShader2 = readProgram("./src/main/glsl/spe_shader_precalc2.comp")
	
	
	val bedrockTexture = glGenTextures()
	val tempBedrockTexture = glGenTextures()

	val streamTexture = glGenTextures()
	val tempStreamTexture = glGenTextures()

	val upliftTexture = glGenTextures()

	val steepestTexture = glGenTextures()

	init {
		glBindTexture(GL_TEXTURE_2D, bedrockTexture)
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx, ny)

		glBindTexture(GL_TEXTURE_2D, tempBedrockTexture)
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx, ny)

		glBindTexture(GL_TEXTURE_2D, streamTexture)
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx, ny)

		glBindTexture(GL_TEXTURE_2D, tempStreamTexture)
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx, ny)

		glBindTexture(GL_TEXTURE_2D, upliftTexture)
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, nx, ny)

		glBindTexture(GL_TEXTURE_2D, steepestTexture)
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8I, nx, ny)

		// Uniforms - just once

		println("srcPos: $srcPosX $srcPosY")
		println("size: $sizeX $sizeY")
		println("nx ny: $nx $ny")

		glUseProgram(initShader)
		glBindImageTexture(0, bedrockTexture, 0, false, 0, GL_WRITE_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(initShader, "bedrockMap"), 0)
		glBindImageTexture(1, streamTexture, 0, false, 0, GL_WRITE_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(initShader, "streamMap"), 1)
		glBindImageTexture(2, tempBedrockTexture, 0, false, 0, GL_WRITE_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(initShader, "tempBedrockMap"), 2)
		glBindImageTexture(3, tempStreamTexture, 0, false, 0, GL_WRITE_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(initShader, "tempStreamMap"), 3)
		glBindImageTexture(4, upliftTexture, 0, false, 0, GL_WRITE_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(initShader, "upliftMap"), 4)
		glBindImageTexture(5, steepestTexture, 0, false, 0, GL_WRITE_ONLY, GL_R8I); glUniform1i(glGetUniformLocation(initShader, "steepestMap"), 5)
		glUniform2i(glGetUniformLocation(initShader, "srcPos"), srcPosX, srcPosY)
		glUniform2i(glGetUniformLocation(initShader, "size"), sizeX, sizeY)
		//run init
		glUseProgram(initShader)
		glDispatchCompute((sizeX / 8) + 1, (sizeY / 8) + 1, 1)
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT)

		glUseProgram(simulationShader)
		glBindImageTexture(0, bedrockTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader, "bedrockMap"), 0)
		glBindImageTexture(1, streamTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader, "streamMap"), 1)
		glBindImageTexture(2, tempBedrockTexture, 0, false, 0, GL_WRITE_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader, "tempBedrockMap"), 2)
		glBindImageTexture(3, tempStreamTexture, 0, false, 0, GL_WRITE_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader, "tempStreamMap"), 3)
		glBindImageTexture(4, upliftTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader, "upliftMap"), 4)
		glBindImageTexture(5, steepestTexture, 0, false, 0, GL_READ_ONLY, GL_R8I); glUniform1i(glGetUniformLocation(simulationShader, "steepestMap"), 5)
		glUniform2i(glGetUniformLocation(simulationShader, "srcPos"), srcPosX, srcPosY)
		glUniform2i(glGetUniformLocation(simulationShader, "size"), sizeX, sizeY)

		glUseProgram(precalcShader)
		glBindImageTexture(0, bedrockTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(precalcShader, "bedrockMap"), 0)
		glBindImageTexture(5, steepestTexture, 0, false, 0, GL_WRITE_ONLY, GL_R8I); glUniform1i(glGetUniformLocation(precalcShader, "steepestMap"), 5)
		glUniform2i(glGetUniformLocation(precalcShader, "srcPos"), srcPosX, srcPosY)
		glUniform2i(glGetUniformLocation(precalcShader, "size"), sizeX, sizeY)

		// dual buffering
		glUseProgram(simulationShader2)
		glBindImageTexture(0, bedrockTexture, 0, false, 0, GL_WRITE_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader2, "bedrockMap"), 0)
		glBindImageTexture(1, streamTexture, 0, false, 0, GL_WRITE_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader2, "streamMap"), 1)
		glBindImageTexture(2, tempBedrockTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader2, "tempBedrockMap"), 2)
		glBindImageTexture(3, tempStreamTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader2, "tempStreamMap"), 3)
		glBindImageTexture(4, upliftTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(simulationShader2, "upliftMap"), 4)
		glBindImageTexture(5, steepestTexture, 0, false, 0, GL_READ_ONLY, GL_R8I); glUniform1i(glGetUniformLocation(simulationShader2, "steepestMap"), 5)
		glUniform2i(glGetUniformLocation(simulationShader2, "srcPos"), srcPosX, srcPosY)
		glUniform2i(glGetUniformLocation(simulationShader2, "size"), sizeX, sizeY)

		glUseProgram(precalcShader2)
		glBindImageTexture(2, tempBedrockTexture, 0, false, 0, GL_READ_ONLY, GL_R32F); glUniform1i(glGetUniformLocation(precalcShader2, "tempBedrockMap"), 2)
		glBindImageTexture(5, steepestTexture, 0, false, 0, GL_WRITE_ONLY, GL_R8I); glUniform1i(glGetUniformLocation(precalcShader2, "steepestMap"), 5)
		glUniform2i(glGetUniformLocation(precalcShader2, "srcPos"), srcPosX, srcPosY)
		glUniform2i(glGetUniformLocation(precalcShader2, "size"), sizeX, sizeY)

		glUseProgram(0)
	}

	fun step(n: Int) {
		for (i in 0..n) {
			glUseProgram(precalcShader)
			glDispatchCompute((sizeX / 8) + 1, (sizeY / 8) + 1, 1)
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT)
	
			glUseProgram(simulationShader)
			glDispatchCompute((sizeX / 8) + 1, (sizeY / 8) + 1, 1)
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT)
	
			glUseProgram(precalcShader2)
			glDispatchCompute((sizeX / 8) + 1, (sizeY / 8) + 1, 1)
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT)
	
			glUseProgram(simulationShader2)
			glDispatchCompute((sizeX / 8) + 1, (sizeY / 8) + 1, 1)
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT)
		}

		glUseProgram(0)
	}

	fun delete() {
		glDeleteTextures(bedrockTexture)
		glDeleteTextures(tempBedrockTexture)

		glDeleteTextures(streamTexture)
		glDeleteTextures(tempStreamTexture)

		glDeleteTextures(upliftTexture)

		glDeleteTextures(steepestTexture)

		releaseProgram(initShader)
		releaseProgram(simulationShader)
		releaseProgram(precalcShader)
		releaseProgram(simulationShader2)
		releaseProgram(precalcShader2)
	}
}