import java.io.IOException

plugins {
    id("java")
    id("com.gradleup.shadow") version "9.3.2"
}

group = "yukifuri.support.asmalltoy"
version = "1.0"

repositories {
    mavenCentral()
}

dependencies {
    implementation("com.github.lalyos:jfiglet:0.0.9")
    testImplementation(platform("org.junit:junit-bom:5.10.0"))
    testImplementation("org.junit.jupiter:junit-jupiter")
    testRuntimeOnly("org.junit.platform:junit-platform-launcher")
}

tasks.test {
    useJUnitPlatform()
}

tasks.jar {
    manifest {
        attributes["Main-Class"] = "yukifuri.support.asmalltoy.Main"
        attributes["Class-Path"] = configurations.runtimeClasspath.get().joinToString(" ") {
            it.name
        }
    }
}

val depCopyDest = "$rootDir/dependencies"

tasks.register<Copy>("copyDependencies") {
    mustRunAfter("build")
    description = "Copy dependencies to $depCopyDest"
    group = "application"

    val dest = File(depCopyDest)

    if (dest.exists()) {
        try {
            delete(dest)
        } catch (ioe: IOException) {
            System.err.println("FAILED to delete $dest")
            throw ioe
        }

        dest.mkdirs()
    }

    from(configurations.runtimeClasspath)
    into(depCopyDest)
}

tasks.shadowJar {
    archiveBaseName = "Terminal-ASCII-Clock"
    archiveClassifier = "full"
}
