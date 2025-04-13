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
        btn_Snow.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ERenderType.SNOW_SCENE.changeState();
                nativeSetRenderType0(ERenderType.SNOW_SCENE.getValue(), ERenderType.SNOW_SCENE.getState());
            }
        });

        Button btn_SmallSnowFore = CustomLayout.findViewById(R.id.btn_SmallSnowFore);
        btn_SmallSnowFore.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ERenderType.SMALL_SNOW_FORE.changeState();
                nativeSetRenderType(ERenderType.SNOW.getValue(), ERenderType.SMALL_SNOW_FORE.getValue(), ERenderType.SMALL_SNOW_FORE.getState());
            }
        });

        Button btn_SmallSnowBack = CustomLayout.findViewById(R.id.btn_SmallSnowBack);
        btn_SmallSnowBack.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ERenderType.SMALL_SNOW_BACK.changeState();
                nativeSetRenderType(ERenderType.SNOW.getValue(), ERenderType.SMALL_SNOW_BACK.getValue(), ERenderType.SMALL_SNOW_BACK.getState());
            }
        });

        Button btn_SmallRainFore = CustomLayout.findViewById(R.id.btn_SmallRainFore);
        btn_SmallRainFore.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ERenderType.SMALL_RAIN_FORE.changeState();
                nativeSetRenderType(ERenderType.RAIN.getValue(), ERenderType.SMALL_RAIN_FORE.getValue(), ERenderType.SMALL_RAIN_FORE.getState());
            }
        });

        Button btn_SmallRainBack = CustomLayout.findViewById(R.id.btn_SmallRainBack);
        btn_SmallRainBack.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ERenderType.SMALL_RAIN_BACK.changeState();
                nativeSetRenderType(ERenderType.RAIN.getValue(), ERenderType.SMALL_RAIN_BACK.getValue(), ERenderType.SMALL_RAIN_BACK.getState());
            }
        });

        Button btn_BigSnowFore = CustomLayout.findViewById(R.id.btn_BigSnowFore);
        btn_BigSnowFore.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ERenderType.BIG_SNOW_FORE.changeState();
                nativeSetRenderType(ERenderType.SNOW.getValue(), ERenderType.BIG_SNOW_FORE.getValue(), ERenderType.BIG_SNOW_FORE.getState());
            }
        });

        Button btn_BigSnowBack = CustomLayout.findViewById(R.id.btn_BigSnowBack);
        btn_BigSnowBack.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ERenderType.BIG_SNOW_BACK.changeState();
                nativeSetRenderType(ERenderType.SNOW.getValue(), ERenderType.BIG_SNOW_BACK.getValue(), ERenderType.BIG_SNOW_BACK.getState());
            }
        });

        Button btn_BigRainFore = CustomLayout.findViewById(R.id.btn_BigRainFore);
        btn_BigRainFore.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ERenderType.BIG_RAIN_FORE.changeState();
                nativeSetRenderType(ERenderType.RAIN.getValue(), ERenderType.BIG_RAIN_FORE.getValue(), ERenderType.BIG_RAIN_FORE.getState());
            }
        });

        Button btn_BigRainBack = CustomLayout.findViewById(R.id.btn_BigRainBack);
        btn_BigRainBack.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                ERenderType.BIG_RAIN_BACK.changeState();
                nativeSetRenderType(ERenderType.RAIN.getValue(), ERenderType.BIG_RAIN_BACK.getValue(), ERenderType.BIG_RAIN_BACK.getState());
            }
        });

        ParentFrameLayout.addView(CustomLayout);
    }

    public native void nativeSetRenderType(int renderType, int enableRenderType, boolean state);
    public native void nativeSetRenderType0(int renderType, boolean state);

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

    public enum ERenderType {
        NONE(0),
        SNOW(1),
        RAIN(2),
        CLOUD(3),
        SNOW_SCENE(4),
        SMALL_SNOW_FORE(5),
        SMALL_SNOW_BACK(6),
        BIG_SNOW_FORE(7),
        BIG_SNOW_BACK(8),
        SMALL_RAIN_FORE(9),
        SMALL_RAIN_BACK(10),
        BIG_RAIN_FORE(11),
        BIG_RAIN_BACK(12);

        private final int value;
        private boolean state;

        ERenderType(int value) {
            this.value = value;
            this.state = false;
        }

        public int getValue() {
            return value;
        }

        public void changeState()
        {
            this.state = !this.state;
        }

        public boolean getState()
        {
            return this.state;
        }
    }
}