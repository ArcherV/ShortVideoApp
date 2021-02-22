package com.example.shortvideoapp;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CameraMetadata;
import android.hardware.camera2.CaptureRequest;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.util.Range;
import android.view.Surface;

import java.util.Arrays;
import java.util.List;

import androidx.annotation.NonNull;
import androidx.core.content.ContextCompat;

public class Camera extends CameraDevice.StateCallback {
    private static final String TAG = Camera.class.getSimpleName();

    private static int FPS_MIN = 15;
    private static int FPS_MAX = 30;

    private Context context;
    private CameraManager cameraManager;
    private CaptureRequest.Builder previewBuilder;
    private Handler mHandler;
    private HandlerThread mThreadHandler;
    private SurfaceTexture surfaceTexture;


    public Camera(Context context, SurfaceTexture surfaceTexture) {
        this.context = context;
        this.surfaceTexture = surfaceTexture;
        cameraManager = (CameraManager)context.getSystemService(Context.CAMERA_SERVICE);

        mThreadHandler = new HandlerThread("CAMERA2");
        mThreadHandler.start();
        mHandler = new Handler(mThreadHandler.getLooper());
    }

    public void openCamera(String cameraId) {
        try{
            int permission = ContextCompat.checkSelfPermission(context, Manifest.permission.CAMERA);
            if(PackageManager.PERMISSION_GRANTED == permission) {
                cameraManager.openCamera(cameraId, this, mHandler);
            }else{
                Log.e(TAG,"Permission not available to open camera");
            }
        }catch (CameraAccessException | IllegalStateException | SecurityException e){
            Log.e(TAG,"Opening Camera has an Exception " + e);
            e.printStackTrace();
        }
    }


    @Override
    public void onOpened(@NonNull CameraDevice camera) {
        try {
            previewBuilder = camera.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
            previewBuilder.set(CaptureRequest.JPEG_ORIENTATION, 0);
            Range<Integer> fpsRange = new Range<>(FPS_MIN, FPS_MAX);
            previewBuilder.set(CaptureRequest.CONTROL_AE_TARGET_FPS_RANGE, fpsRange);

            Surface surface = new Surface(surfaceTexture);

            previewBuilder.addTarget(surface);

            List<Surface> targetSurfaces = Arrays.asList(surface);
            camera.createCaptureSession(targetSurfaces,
                    new CameraCaptureSession.StateCallback() {
                        @Override
                        public void onConfigured(@NonNull CameraCaptureSession session) {
                            onCaptureSessionConfigured(session);
                        }
                        @Override
                        public void onConfigureFailed(@NonNull CameraCaptureSession session) {
                            Log.e(TAG,"!!! Creating Capture Session failed due to internal error ");
                        }
                    }, mHandler);

        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    private void onCaptureSessionConfigured(@NonNull CameraCaptureSession session) {
        Log.i(TAG,"Capture Session created");
        previewBuilder.set(CaptureRequest.CONTROL_MODE, CameraMetadata.CONTROL_MODE_AUTO);
        try {
            session.setRepeatingRequest(previewBuilder.build(), null, null);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }


    @Override
    public void onDisconnected(@NonNull CameraDevice camera) {

    }

    @Override
    public void onError(@NonNull CameraDevice camera, int error) {

    }
}
