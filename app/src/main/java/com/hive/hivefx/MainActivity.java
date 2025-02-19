package com.hive.hivefx;

import android.annotation.SuppressLint;
import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.Toast;

import com.google.androidgamesdk.GameActivity;

public class MainActivity extends GameActivity
{
    static
    {
        System.loadLibrary("hivefx");
        System.loadLibrary("mylibrary");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        addExtraViews();
    }

    private void addExtraViews() {
        FrameLayout ParentFrameLayout = findViewById(contentViewId);
        LayoutInflater inflater = (LayoutInflater) getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        @SuppressLint("InflateParams") View CustomLayout = inflater.inflate(R.layout.activity_game, null);
        Button SpeedUpButton    = CustomLayout.findViewById(R.id.btn_speed_up);
        Button SpeedResetButton = CustomLayout.findViewById(R.id.btn_speed_reset);
        SpeedUpButton.setOnClickListener(v -> {
            Toast.makeText(this, "Speed Up!", Toast.LENGTH_SHORT).show();
        });
        SpeedResetButton.setOnClickListener(v -> {
            Toast.makeText(this, "Speed Reset.", Toast.LENGTH_SHORT).show();
        });
        ParentFrameLayout.addView(CustomLayout);
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