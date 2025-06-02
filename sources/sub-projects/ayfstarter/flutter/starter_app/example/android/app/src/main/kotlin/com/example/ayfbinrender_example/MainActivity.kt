package com.example.ayfbinrender_example

import io.flutter.embedding.android.FlutterActivity

class MainActivity : FlutterActivity(){

    // Here, we try to load the project specific shared object
    init {
        System.loadLibrary("ayfstarter-native-config_import") // Ohne "lib" und ".so"
    }
}

