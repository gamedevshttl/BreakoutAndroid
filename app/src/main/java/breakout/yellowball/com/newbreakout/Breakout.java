package breakout.yellowball.com.newbreakout;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

import java.io.File;
import android.content.res.AssetManager;

public class Breakout extends AppCompatActivity {

    private GLSurfaceView glSurfaceView;
    private boolean rendererSet;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

//        setContentView(R.layout.activity_breakout);
//        // Example of a call to a native method
//        TextView tv = (TextView) findViewById(R.id.sample_text);
//        tv.setText(stringFromJNI());

        File path = this.getExternalFilesDir(null);
        if(null == path){
            path = this.getFilesDir();
        }
        on_set_data_dir(path.getPath());

        init_asset_manager(this.getAssets());

        ActivityManager activityManager = (ActivityManager)getSystemService(Context.ACTIVITY_SERVICE);
        ConfigurationInfo configurationInfo = activityManager.getDeviceConfigurationInfo();

        final boolean suppoerES2 = configurationInfo.reqGlEsVersion >= 0x2000;

        if(suppoerES2){
            glSurfaceView = new GLSurfaceView(this);
            glSurfaceView.setEGLContextClientVersion(2);
            glSurfaceView.setRenderer(new RenderWrapper());
            rendererSet = true;
            setContentView(glSurfaceView);
        }
    }

    @Override
    public void onPause()
    {
        super.onPause();
        if(rendererSet)
            glSurfaceView.onPause();
    }

    @Override
    public void onResume()
    {
        super.onResume();
        if(rendererSet)
            glSurfaceView.onResume();
    }


    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
    public native void on_set_data_dir(String dir);
    public native void init_asset_manager(AssetManager assetManager);
}
