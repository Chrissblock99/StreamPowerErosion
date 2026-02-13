package me.chriss99

import org.lwjgl.opengl.GL45.*
import java.io.File
import java.util.regex.Matcher
import java.util.regex.Pattern
import kotlin.math.min

fun read(fileName: String): List<String> {
	println("loading program $fileName")
	val file = File(fileName)
	if (!file.exists()) {
		println("Failed to load program $fileName")
		return emptyList()
	}
	return file.readLines()
}

fun prepareSource(file: List<String>, definitions: List<String>): List<String> {
	if (file.isEmpty())
		return emptyList()

	val file: MutableList<String> = file.toMutableList()

	val source: MutableList<String> = mutableListOf()

	val versionLine = file.indexOfFirst { it.startsWith("#version") }
	if (versionLine == -1) {
		println("[error] no #version directive found")
		println("Failed to prepare source program. No #version directive found")
		return emptyList()
	}

	val version = file.removeAt(versionLine)

	if (file.any { it.contains("#version") }) {
		println("[error] found several #version directives")
		println("Failed to prepare source program. Found several #version directive")
		return emptyList()
	}

	source.add(version)
	source.addAll(definitions)
	source.addAll(file)

	return source
}


fun shaderString(type: Int) = when (type) {
	GL_VERTEX_SHADER -> "vertex shader"
	GL_FRAGMENT_SHADER -> "fragment shader"
	GL_GEOMETRY_SHADER -> "geometry shader"
	GL_TESS_CONTROL_SHADER -> "control shader"
	GL_TESS_EVALUATION_SHADER -> "evaluation shader"
	GL_COMPUTE_SHADER -> "compute shader"
	else -> "shader"
}

val shaderKeys = listOf(
	"VERTEX_SHADER",
	"FRAGMENT_SHADER",
	"GEOMETRY_SHADER",
	"TESSELATION_CONTROL",
	"EVALUATION_CONTROL",
	"COMPUTE_SHADER"
)
const val shaderKeysMax = 6

val shaderTypes = listOf(
	GL_VERTEX_SHADER,
	GL_FRAGMENT_SHADER,
	GL_GEOMETRY_SHADER,
	GL_TESS_CONTROL_SHADER,
	GL_TESS_EVALUATION_SHADER,
	GL_COMPUTE_SHADER
)

fun compileShader(program: Int, shaderType: Int, source: List<String>): Int {
	if (source.isEmpty() || shaderType == 0)
		return 0

	val shader = glCreateShader(shaderType)
	glAttachShader(program, shader)

	glShaderSource(shader, *source.map { "$it\n" }.toTypedArray())
	glCompileShader(shader)

	val status = intArrayOf(0)
	glGetShaderiv(shader, GL_COMPILE_STATUS, status)
	return if (status[0] == GL_TRUE) shader else 0
}


fun reloadProgram(program: Int, filename: String, definitions: List<String>): Int {
	if (program == 0)
		return -1

	val shadersMax = intArrayOf(0)
	glGetProgramiv(program, GL_ATTACHED_SHADERS, shadersMax)
	if (shadersMax[0] > 0) {
		val shaders = IntArray(shadersMax[0])
		glGetAttachedShaders(program, shadersMax, shaders)
		for (shader in shaders) {
			glDetachShader(program, shader)
			glDeleteShader(shader)
		}
	}

	glObjectLabel(GL_PROGRAM, program, filename)

	val commonSource = read(filename)
	for (i in 0..<shaderKeysMax) {
		if (commonSource.indexOfFirst { it.contains(shaderKeys[i]) } != -1) {
			val source = prepareSource(commonSource, definitions.toMutableList().apply { add("#define ${shaderKeys[i]}") })
			val shader = compileShader(program, shaderTypes[i], source)
			if (shader == 0) {
				println("[error] compiling ${shaderString(shaderTypes[i])}...\n$definitions\n")
				println("Failed to compile source program")
			}
		}
	}

	glLinkProgram(program)

	val status = intArrayOf(0)
	glGetProgramiv(program, GL_LINK_STATUS, status)
	if (status[0] == GL_FALSE) {
		println("[error] linking program $program '$filename'...\n")
		println("Failed to link source program")
		return -1
	}

	glUseProgram(program)
	return 0
}

fun readProgram(filename: String, definitions: List<String> = emptyList()): Int {
	val program = glCreateProgram()
	reloadProgram(program, filename, definitions)
	programPrintErrors(program)
	return program
}

fun releaseProgram(program: Int): Int {
	if (program == 0)
		return -1

	val shadersMax = intArrayOf(0)
	glGetProgramiv(program, GL_ATTACHED_SHADERS, shadersMax)

	if (shadersMax[0] > 0) {
		val shaders = IntArray(shadersMax[0])
		glGetAttachedShaders(program, shadersMax, shaders)
		for (shader in shaders) {
			glDetachShader(program, shader)
			glDeleteShader(shader)
		}
	}

	glDeleteProgram(program)
	return 0
}


fun printLine(errors: MutableList<String>, source: List<String>, beginId: Int, lineId: Int) {
	for (lineIndex in beginId..<min(lineId, source.size)) {
		val line = "  ${lineIndex.toString().padStart(4, '0')}  " + source[lineIndex]
		errors.add(line.replace("\t", "    "))
	}
}

val patterns = listOf(
	Pattern.compile("(\\d+)\\((\\d+)\\):([^\\n])"),       // nvidia syntax
	Pattern.compile("(\\d+):(\\d+)\\(\\d+\\):([^\\n])"),  // mesa syntax
	Pattern.compile("ERROR:(\\d+):(\\d+):([^\\n])"),      // ati syntax
	Pattern.compile("WARNING:(\\d+):(\\d+):([^\\n])")     // ati syntax
)

fun printErrors(errors: MutableList<String>, log: List<String>, source: List<String>) {
	println("[error log]\n${log.fold("") { sum, line -> "$sum$line\n" }}")

	val logData = log.mapNotNull { line -> patterns.map { it.matcher(line) }.firstOrNull(Matcher::matches) }
		.map { Triple(it.group(1)!!.toInt(), it.group(2)!!.toInt(), it.group(3)!!) }.sortedBy { it.second }

	var lastLine = -1
	for ((stringId, line, message) in logData) {
		errors.add("\n")
		printLine(errors, source, lastLine + 1, line)
		errors.add("\n")

		errors.add(message)

		lastLine = line
	}
	errors.add("\n")
	printLine(errors, source, lastLine + 1, 1000)
	errors.add("\n")
}

fun programFormatErrors(program: Int, errors: MutableList<String>) {
	errors.clear()

	if (program == 0) {
		errors.add("[error] no program...\n")
		return
	}

	val status = intArrayOf(0)
	glGetProgramiv(program, GL_LINK_STATUS, status)
	if (status[0] == GL_TRUE)
		return

	val shadersMax = intArrayOf(0)
	glGetProgramiv(program, GL_ATTACHED_SHADERS, shadersMax)
	if (shadersMax[0] == 0) {
		errors.add("[error] no shaders...\n")
		return
	}

	val shaders = IntArray(shadersMax[0])
	glGetAttachedShaders(program, shadersMax, shaders)
	for (shader in shaders) {
		val value = intArrayOf(0)
		glGetShaderiv(shader, GL_COMPILE_STATUS, value)
		if (value[0] != GL_FALSE)
			continue

		val log = glGetShaderInfoLog(shader).split("\n")

		val source = glGetShaderSource(shader).split("\n")

		glGetShaderiv(shader, GL_SHADER_TYPE, value)
		errors.add("[error] compiling ${shaderString(value[0])}...\n")

		printErrors(errors, log, source)
	}

	val log = glGetProgramInfoLog(program).split("\n")

	errors.add("[error] linking program...\n")
	errors.addAll(log)
}

fun programPrintErrors(program: Int) {
	val errors = mutableListOf<String>()
	programFormatErrors(program, errors)
	if (!errors.isEmpty())
		println(errors.fold("") { sum, line -> "$sum$line\n"})
}