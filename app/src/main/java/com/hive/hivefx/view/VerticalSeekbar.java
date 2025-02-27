package com.hive.hivefx.view;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Canvas;
import android.util.AttributeSet;
import android.view.MotionEvent;
import androidx.annotation.NonNull;
import org.jetbrains.annotations.Nullable;

public class VerticalSeekbar extends androidx.appcompat.widget.AppCompatSeekBar
{
    public VerticalSeekbar(@NonNull Context context) { super(context);  }
    public VerticalSeekbar(@NonNull Context context, @Nullable AttributeSet attrs) { super(context, attrs); }
    public VerticalSeekbar(@NonNull Context context, @Nullable AttributeSet attrs, int defStyleAttr) { super(context, attrs, defStyleAttr);  }
    //onDraw事件中旋转画布，由水平变垂直
    @Override
    protected void onDraw(Canvas canvas) {
        canvas.rotate(-90);
        canvas.translate(-getHeight(), 0);
        super.onDraw(canvas);
    }
    //measure事件中重新计算控件尺寸，对换宽高值
    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        int width = getMeasuredHeight();
        int height = getMeasuredWidth();
        setMeasuredDimension(width, height);
    }
    //切记加上尺寸变更时的事件，变更对换宽高值
    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(h, w, oldh, oldw);
    }
    //最后重载一下滑动事件，重新计算progress值
    @SuppressLint("ClickableViewAccessibility")
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (!isEnabled()) return false;
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_MOVE:
            case MotionEvent.ACTION_UP:
                //因为我这是向上滑动的，所以最大值乘Y轴的滑动比例得到上面的取值，实际值再用最大值去减即可
                setProgress(getMax() - (int) (getMax() * event.getY() / getHeight()));
                onSizeChanged(getWidth(), getHeight(), 0, 0);
                break;
        }
        return true;
    }
}