pluginManagement {
    repositories {
        google {
            content {
                includeGroupByRegex("com\\.android.*")
                includeGroupByRegex("com\\.google.*")
                includeGroupByRegex("androidx.*")
            }
        }
        mavenCentral()
        gradlePluginPortal()
    }
}
dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {
        google()
        mavenCentral()
    }
}

rootProject.name = "HiveFX"
include(":app")
include(":mylibrary")
include(":system_test001_frameplaying")
include(":system_test002_slidewindow")
include(":system_test003_blending")
include(":system_test004_snowstylization")
include(":system_test006_scrollrain")
include(":system_test005_2channelstexture")
include(":system_test007_combinedframeplaying")
include(":system_test008_buildsimplestapk")
include(":system_test009_downloadpicremote")
include(":system_test010_2CScrollRain")
include(":system_test017_jpgrendering")
include(":system_test018_splashplaying")
include(":system_test013_quantizationframeplaying")
include(":system_test014_compressedfreamplaying")
include(":system_test020_interpolationplaying")
