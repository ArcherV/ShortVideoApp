package com.example.shortvideoapp;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.view.SurfaceView;
import android.view.View;
import android.widget.ImageButton;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private static final int RUNNING = 0xFFFF0000;
    private static final int PERMISSION_REQUEST_CAMERA = 1;

    SurfaceView view = null;
    ImageButton recorder_button = null;
    ImageButton player_button = null;

    private boolean recording;
    private boolean playing;
    int color = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[] { Manifest.permission.CAMERA }, PERMISSION_REQUEST_CAMERA);
            return;
        }

        recording = playing = false;

        view = findViewById(R.id.camera_preview);
        recorder_button = findViewById(R.id.record_button);
        player_button = findViewById(R.id.play_button);
        recorder_button.setOnClickListener(this);
        player_button.setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.record_button:
                if (!recording) {
                    recorder_button.setColorFilter(RUNNING);
                } else {
                    recorder_button.setColorFilter(0);
                }
                recording = !recording;
                Toast.makeText(getApplicationContext(), "开始录制了", Toast.LENGTH_SHORT).show();
                break;
            case R.id.play_button:
                if (!playing) {
                    player_button.setColorFilter(RUNNING);
                    color = (color + 1) % 3;
                    setColor(color);
                } else {
                    player_button.setColorFilter(0);
                }
                playing = !playing;
                Toast.makeText(getApplicationContext(), "开始播放了", Toast.LENGTH_SHORT).show();
                break;
        }
    }
    // 为了测试EGL是否创建正确
    private native void setColor(int c);
}
