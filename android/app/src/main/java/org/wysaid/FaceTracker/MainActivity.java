package org.wysaid.FaceTracker;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;
import android.widget.Toast;

import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class MainActivity extends AppCompatActivity {

    static  {
        System.loadLibrary("opencv_java3");
        System.loadLibrary("FaceTracker");
    }

    protected long mNativeHandler = 0;
    protected ByteBuffer mBuffer;
    protected Bitmap mBitmap;

    protected static native long nativeCreateTracker(String model, String tri, String con);
    protected native void nativeReleaseTracker(long handler);
    protected native float[] nativeTrackFace(long handler, Buffer buffer, int width, int height, int stride, int channel);
    protected native void nativeResetTrackerFrame(long handler);


    public void resetTrackerFrame() {
        nativeResetTrackerFrame(mNativeHandler);
    }

    //if drawMesh is true, the mesh would be drawn on mBuffer.
    public float[] trackFaceByBitmap(Bitmap bmp, boolean drawMesh) {
        if(bmp == null || bmp.isRecycled() || bmp.getConfig() != Bitmap.Config.ARGB_8888) {
            return null;
        }

        int bufferLen = bmp.getRowBytes() * bmp.getHeight();

        if(mBuffer == null || mBuffer.limit() < bufferLen) {
            if(mBuffer == null) {
                mBuffer = ByteBuffer.allocateDirect(bufferLen).order(ByteOrder.nativeOrder());
            } else {
                mBuffer.limit(bufferLen);
            }
        }

        mBuffer.position(0);

        bmp.copyPixelsToBuffer(mBuffer);

        return nativeTrackFace(mNativeHandler, mBuffer.position(0), bmp.getWidth(), bmp.getHeight(), bmp.getRowBytes(), 4);
    }

    public void releaseTracker() {
        if(mNativeHandler != 0) {
            nativeReleaseTracker(mNativeHandler);
            mNativeHandler = 0;
        }
    }

    private ImageView theView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        theView = (ImageView) findViewById(R.id.resultView);

        if(mNativeHandler == 0) {
            mNativeHandler = nativeCreateTracker(null, null, null);
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        releaseTracker();
    }

    public void drawFeature(Bitmap bmp, float[] keyPoints) {
        Canvas canvas = new Canvas(bmp);
        Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
        paint.setColor(0xffff0000);
        paint.setStrokeWidth(6.0f);
        canvas.drawPoints(keyPoints, paint);
    }

    public void trackFace(View view) {
        if(mBitmap == null) {
            mBitmap = BitmapFactory.decodeResource(getResources(), R.drawable.demo);
            if(mBitmap.getConfig() != Bitmap.Config.ARGB_8888 || !mBitmap.isMutable()) {
                Bitmap newBMP = mBitmap.copy(Bitmap.Config.ARGB_8888, true);
                mBitmap = newBMP;
            }
        }

        resetTrackerFrame();
        float[] keyPoints = trackFaceByBitmap(mBitmap, true);
        if(keyPoints != null) {
            Toast.makeText(this, "find face!", Toast.LENGTH_SHORT).show();
            drawFeature(mBitmap, keyPoints);
            theView.setImageBitmap(mBitmap);
        } else {
            Toast.makeText(this, "no face found!", Toast.LENGTH_SHORT).show();
        }
    }
}
