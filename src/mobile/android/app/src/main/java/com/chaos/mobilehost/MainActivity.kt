package com.chaos.mobilehost

import android.app.Activity
import android.os.Bundle
import android.util.Log

class MainActivity : Activity() {
    companion object {
        init {
            System.loadLibrary("chaos_mobile_android_host")
        }
    }

    external fun runHost(): Int

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val exitCode = runHost()
        Log.i("ChaosMobileHost", "runHost exitCode=$exitCode")
    }
}
