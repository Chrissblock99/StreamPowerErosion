plugins {
    kotlin("jvm") version "2.2.21"
}

group = "me.chriss99"
version = "1.0-SNAPSHOT"

val lwjglVersion = "3.3.6"
val lwjglNatives = "natives-linux"

repositories {
    mavenCentral()
}

dependencies {
    testImplementation(kotlin("test"))

    implementation(platform("org.lwjgl:lwjgl-bom:$lwjglVersion"))

    implementation("org.lwjgl", "lwjgl")
    implementation("org.lwjgl", "lwjgl-glfw")
    implementation("org.lwjgl", "lwjgl-opengl")
    runtimeOnly("org.lwjgl", "lwjgl", classifier = lwjglNatives)
    runtimeOnly("org.lwjgl", "lwjgl-glfw", classifier = lwjglNatives)
    runtimeOnly("org.lwjgl", "lwjgl-opengl", classifier = lwjglNatives)
}

kotlin {
    jvmToolchain(21)
}

tasks {
    jar {
        manifest {
            attributes["Main-Class"] = "me.chriss99.Main"
            attributes["Implementation-Version"] = version
        }
    }


    test {
        useJUnitPlatform()
    }
}
