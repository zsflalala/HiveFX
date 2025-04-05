package com.hive.system_test013_quantizationframeplaying;

import android.util.Log;
import android.view.View;
import com.google.androidgamesdk.GameActivity;
import android.opengl.GLES30;

public class MainActivity extends GameActivity
{
    static
    {
        System.loadLibrary("system_test013_quantizationframeplaying");
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus)
    {
        int[] results = new int[1];
        GLES30.glGetIntegerv(GLES30.GL_MAJOR_VERSION, results, 0);
        int majorVersion = results[0];

        if (majorVersion >= 3) {
            Log.d("OpenGLInfo", "✅ 设备支持 OpenGL ES 3.0 或更高版本");
        } else {
            Log.d("OpenGLInfo", "❌ 设备仅支持 OpenGL ES 2.x 或更低版本");
        }

        super.onWindowFocusChanged(hasFocus);
        if (hasFocus)
        {
            hideSystemUi();
        }
    }
    private void hideSystemUi()
    {
        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                        | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_FULLSCREEN
        );
    }
}