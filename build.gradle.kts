plugins {
    id("com.android.application") version "8.2.2" apply false
    id("org.jetbrains.kotlin.android") version "1.9.22" apply false
}
																																																																																																																																																																																				tasks.register<Exec>("startApi") { workingDir = rootProject.projectDir; commandLine("cmd", "/c", "start", "\"\"", "cmd", "/k", "node", "service") }
