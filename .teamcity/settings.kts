import jetbrains.buildServer.configs.kotlin.*
import jetbrains.buildServer.configs.kotlin.buildFeatures.perfmon
import jetbrains.buildServer.configs.kotlin.buildSteps.script
import jetbrains.buildServer.configs.kotlin.triggers.vcs

/*
The settings script is an entry point for defining a TeamCity
project hierarchy. The script should contain a single call to the
project() function with a Project instance or an init function as
an argument.

VcsRoots, BuildTypes, Templates, and subprojects can be
registered inside the project using the vcsRoot(), buildType(),
template(), and subProject() methods respectively.

To debug settings scripts in command-line, run the

    mvnDebug org.jetbrains.teamcity:teamcity-configs-maven-plugin:generate

command and attach your debugger to the port 8000.

To debug in IntelliJ Idea, open the 'Maven Projects' tool window (View
-> Tool Windows -> Maven Projects), find the generate task node
(Plugins -> teamcity-configs -> teamcity-configs:generate), the
'Debug' option is available in the context menu for the task.
*/

version = "2024.03"

project {

    buildType(UnitAndIntegrationTesting)
    buildType(Build)
}

object Build : BuildType({
    name = "Build"

    vcs {
        root(DslContext.settingsRoot)
    }

    steps {
        script {
            name = "Build Ros2 vehicle package"
            id = "Build_Ros2_vehicle_package"
            scriptContent = """
                source /opt/ros/humble/setup.bash
                cd ros2_ws
                colcon build --packages-select robotvehicle_package
            """.trimIndent()
        }
    }

    triggers {
        vcs {
        }
    }

    features {
        perfmon {
        }
    }
})

object UnitAndIntegrationTesting : BuildType({
    name = "Unit and Integration Testing"

    vcs {
        root(DslContext.settingsRoot)
    }

    steps {
        script {
            name = "RunTests"
            id = "simpleRunner"
            scriptContent = """
                source /opt/ros/humble/setup.bash
                cd ros2_ws
                colcon test --packages-select robotvehicle_package
                colcon test-result --all --verbose
            """.trimIndent()
        }
    }

    dependencies {
        snapshot(Build) {
            reuseBuilds = ReuseBuilds.NO
        }
    }
})
