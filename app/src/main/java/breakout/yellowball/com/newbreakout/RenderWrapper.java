package breakout.yellowball.com.newbreakout;

import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class RenderWrapper implements GLSurfaceView.Renderer{

    static {
        System.loadLibrary("native-lib");
    }

    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {
        on_suface_createrd();
    }

    public void onSurfaceChanged(GL10 gl, int width, int height)
    {
        on_serface_changed(width, height);
    }

    public void onDrawFrame(GL10 gl)
    {
        on_draw_frame();
    }



    public native void on_suface_createrd();
    public native void on_serface_changed(int width, int height);
    public native void on_draw_frame();
}
