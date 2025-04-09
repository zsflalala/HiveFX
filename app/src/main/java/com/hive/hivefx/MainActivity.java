package com.hive.hivefx;

import android.annotation.SuppressLint;
import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;

import com.google.androidgamesdk.GameActivity;

public class MainActivity extends GameActivity
{
    static
    {
        System.loadLibrary("hivefx");
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

        // 查找按钮并设置点击监听器
        Button btn_Snow = CustomLayout.findViewById(R.id.btn_Snow);
        btn_Snow.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // 当按钮点击时，调用 native 方法来转调 C++ 代码
                nativeSetRenderType(4, -1);
            }
        });

        Button btn_SmallSnowFore = CustomLayout.findViewById(R.id.btn_SmallSnowFore);
        btn_SmallSnowFore.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // 当按钮点击时，调用 native 方法来转调 C++ 代码
                nativeSetRenderType(1, 5);
            }
        });

        Button btn_SmallSnowBack = CustomLayout.findViewById(R.id.btn_SmallSnowBack);
        btn_SmallSnowBack.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // 当按钮点击时，调用 native 方法来转调 C++ 代码
                nativeSetRenderType(1, 6);
            }
        });

        Button btn_SmallRainFore = CustomLayout.findViewById(R.id.btn_SmallRainFore);
        btn_SmallRainFore.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // 当按钮点击时，调用 native 方法来转调 C++ 代码
                nativeSetRenderType(2, 9);
            }
        });

        Button btn_SmallRainBack = CustomLayout.findViewById(R.id.btn_SmallRainBack);
        btn_SmallRainBack.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // 当按钮点击时，调用 native 方法来转调 C++ 代码
                nativeSetRenderType(2, 10);
            }
        });

        Button btn_BigSnowFore = CustomLayout.findViewById(R.id.btn_BigSnowFore);
        btn_BigSnowFore.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // 当按钮点击时，调用 native 方法来转调 C++ 代码
                nativeSetRenderType(1, 7);
            }
        });

        Button btn_BigSnowBack = CustomLayout.findViewById(R.id.btn_BigSnowBack);
        btn_BigSnowBack.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // 当按钮点击时，调用 native 方法来转调 C++ 代码
                nativeSetRenderType(1, 8);
            }
        });

        Button btn_BigRainFore = CustomLayout.findViewById(R.id.btn_BigRainFore);
        btn_BigRainFore.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // 当按钮点击时，调用 native 方法来转调 C++ 代码
                nativeSetRenderType(2, 11);
            }
        });

        Button btn_BigRainBack = CustomLayout.findViewById(R.id.btn_BigRainBack);
        btn_BigRainBack.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // 当按钮点击时，调用 native 方法来转调 C++ 代码
                nativeSetRenderType(2, 12);
            }
        });

        ParentFrameLayout.addView(CustomLayout);
    }

    public native void nativeSetRenderType(int renderType, int enableRenderType);

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