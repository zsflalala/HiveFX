package com.hive.system_test004_snowstylization;

import android.content.Intent;
import android.net.Uri;
import android.util.Log;
import android.view.View;

import com.google.androidgamesdk.GameActivity;

import java.io.File;

public class MainActivity extends GameActivity
{
    static
    {
        System.loadLibrary("system_test004_snowstylization");
    }

    @Override
    protected void onResume() {
        super.onResume();
        String file = "/storage/emulated/0/Pictures/Screenshots/";
        sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, Uri.parse(file)));
//        sendBroadcast(new Intent(Intent.ACTION_MEDIA_MOUNTED, Uri.parse(file)));
    }

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