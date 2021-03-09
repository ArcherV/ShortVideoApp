package com.example.shortvideoapp;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.graphics.SurfaceTexture;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.ImageButton;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity implements View.OnClickListener, SurfaceTexture.OnFrameAvailableListener, SurfaceHolder.Callback  {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private static final int RUNNING = 0xFFFF0000;
    private static final String cameraId = "0";
    private static final int PERMISSION_REQUEST = 1;

    SurfaceView view = null;
    ImageButton recorder_button = null;
    ImageButton player_button = null;
    private Camera camera;
    private SurfaceTexture surfaceTexture;

    private boolean recording;
    private boolean playing;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        recording = playing = false;

        view = findViewById(R.id.camera_preview);
        view.getHolder().addCallback(this);
        recorder_button = findViewById(R.id.record_button);
        player_button = findViewById(R.id.play_button);
        recorder_button.setOnClickListener(this);
        player_button.setOnClickListener(this);

        registerAssetManager(getAssets());

        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED ||
            ContextCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) != PackageManager.PERMISSION_GRANTED ||
            ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED ||
            ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[] {
                    Manifest.permission.CAMERA,
                    Manifest.permission.RECORD_AUDIO,
                    Manifest.permission.WRITE_EXTERNAL_STORAGE,
                    Manifest.permission.READ_EXTERNAL_STORAGE },
                    PERMISSION_REQUEST);
        }
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.record_button:
                if (!recording) {
                    recorder_button.setColorFilter(RUNNING);
                    startRecord();
                    Toast.makeText(getApplicationContext(), "开始录制了", Toast.LENGTH_SHORT).show();
                } else {
                    recorder_button.setColorFilter(0);
                    stopRecord();
                    Toast.makeText(getApplicationContext(), "停止录制了", Toast.LENGTH_SHORT).show();
                }
                recording = !recording;
                break;
            case R.id.play_button:
                if (!playing) {
                    player_button.setColorFilter(RUNNING);
                } else {
                    player_button.setColorFilter(0);
                }
                playing = !playing;
                Toast.makeText(getApplicationContext(), "开始播放了", Toast.LENGTH_SHORT).show();
                break;
        }
    }

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        surfaceTexture.updateTexImage();
        render();
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        InitView(holder.getSurface());
        surfaceTexture = new SurfaceTexture(CreateTextureOES());
        surfaceTexture.setOnFrameAvailableListener(this);
        surfaceTexture.setDefaultBufferSize(720, 1280);
        camera = new Camera(this, surfaceTexture);
        camera.openCamera(cameraId);
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

    }

    private native void InitView(Object surface);
    private native void registerAssetManager(AssetManager assetManager);
    private native int CreateTextureOES();
    private native void render();
    private native void startRecord();
    private native void stopRecord();
}
