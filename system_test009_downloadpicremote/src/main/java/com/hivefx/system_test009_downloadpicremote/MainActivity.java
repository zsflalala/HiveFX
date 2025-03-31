package com.hivefx.system_test009_downloadpicremote;

import android.os.Bundle;
import android.view.View;
import com.google.androidgamesdk.GameActivity;

public class MainActivity extends GameActivity
{
    static
    {
        System.loadLibrary("system_test009_downloadpicremote");
    }

//    @Override
//    protected void onCreate(Bundle savedInstanceState)
//    {
//        super.onCreate(savedInstanceState);
//
//        Context context = this;
//        File ExternalFilesDir = context.getExternalFilesDir(null);
//        String Path = ExternalFilesDir != null ? ExternalFilesDir.getAbsolutePath() : null;
//
//        Downloader.setOutputPath(Path);
//    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus)
    {
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