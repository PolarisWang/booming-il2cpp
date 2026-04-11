plugins {
    id("com.android.application")
    kotlin("android")
}

android {
    namespace = "com.chaos.mobilehost"
    compileSdk = 34

    defaultConfig {
        applicationId = "com.chaos.mobilehost"
        minSdk = 24
        targetSdk = 34
    }

    externalNativeBuild {
        cmake {
            path = file("../CMakeLists.txt")
        }
    }
}

dependencies {
    implementation("androidx.appcompat:appcompat:1.7.0")
}
