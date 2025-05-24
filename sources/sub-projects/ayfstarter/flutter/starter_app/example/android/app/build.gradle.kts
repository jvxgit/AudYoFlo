plugins {
    id("com.android.application")
    id("kotlin-android")
    // The Flutter Gradle Plugin must be applied after the Android and Kotlin Gradle plugins.
    id("dev.flutter.flutter-gradle-plugin")
}

android {
    namespace = "com.example.ayfbinrender_example"
    compileSdk = flutter.compileSdkVersion
    ndkVersion = flutter.ndkVersion
    
    // Override the default NDK version to prevent warning. flutter.ndkVersion is set somewhere in the Flutter package,
    // https://stackoverflow.com/questions/77228813/where-is-defined-flutter-ndkversion-in-build-gradle
    // Currently, it is set to flutter.ndkVersion=26.3.11579264
    ndkVersion = "27.0.12077973"

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }    

    kotlinOptions {
        jvmTarget = JavaVersion.VERSION_11.toString()
    }

    defaultConfig {
        // TODO: Specify your own unique Application ID (https://developer.android.com/studio/build/application-id.html).
        applicationId = "com.example.ayfbinrender_example"
        // You can update the following values to match your application needs.
        // For more information, see: https://flutter.dev/to/review-gradle-config.
        minSdk = flutter.minSdkVersion
        targetSdk = flutter.targetSdkVersion
        versionCode = flutter.versionCode
        versionName = flutter.versionName
    }

    buildTypes {
        release {
            // TODO: Add your own signing config for the release build.
            // Signing with the debug keys for now, so `flutter run --release` works.
            signingConfig = signingConfigs.getByName("debug")
        }
    }
}

flutter {
    source = "../.."
}

// ========================================================================
// This part has been added to copy the jni libraries to the android apk.
// ========================================================================
val copyJniLibs by tasks.registering(Copy::class) {
    
    println(" ### Registering task <copyJniLibs>")

    // Get the path where the current AYF-SDK is located
    val myEnvVar = System.getenv("AYF_SDK_PATH_ANDROID")
    println(" ### Environment Variable <AYF_SDK_PATH_ANDROID>: $myEnvVar")

    // Specify current abi. Typically, we would need to copy JNIs for all kinds of ABIs
    val abi = "x86_64"
    println(" ### ABI Specification: $abi")

    val fromPath = "$myEnvVar/$abi/bin"         
    
    val out: Provider<Directory> = layout.buildDirectory.dir("intermediates/merged_jni_libs/debug/mergeDebugJniLibFolders/out/$abi")
    val bDir = out.get().asFile
    // println("bDir: $bDir")
    
    // OLD VERSION:  val toPath = "$buildDir/intermediates/merged_jni_libs/debug/mergeDebugJniLibFolders/out/x86_64"
    val toPath = "$bDir"

    println(" ### Copy JNI libs from location: $fromPath")
    println(" ### Copy JNI libs to location: $toPath")

    // Parameters for copy
    from(fromPath)
    into(toPath)    
    include("**/libayfstarter-native-config_import.so")
    include("**/libjvx-flutter-native-shared_import.so")

    // Renaming the so files to be more applicable AND generic at the same time!!
    rename("libayfstarter-native-config_import.so", "ayfcorepack-starter_app.so")
    rename("libjvx-flutter-native-shared_import.so", "ayfflutter.so")

    // Define when to skip this in case the target is up-to-date
    outputs.upToDateWhen { false }

    println(" ### Task <copyJniLibs>, check source folder: $fromPath")
    if (!file(fromPath).exists()) {
        println(" ### --> WARNING: Source folder <$fromPath> does not exist!")
    }
    else
    {
        println(" ### --> Source folder <$fromPath> exists.")
    }

    println(" ### Task <copyJniLibs>, check destination folder: $toPath")
    if (!file(toPath).exists()) {
        println(" ### --> WARNING: Destination folder <$toPath> does not exist!")
    }
    else
    {
        println(" ### --> Destination folder <$toPath> exists.")
    }

    doLast {
        println(" ### Task <copyJniLibs> successfully executed, outputting directory content after processing:")
        destinationDir.walkTopDown().forEach { file ->
            if (file.isFile) {
                println(" ### - ${file.relativeTo(destinationDir)}")
            }
        }
    }
}

// Run this task before all other build tasks
tasks.named("preBuild") {
    dependsOn("copyJniLibs")
}


