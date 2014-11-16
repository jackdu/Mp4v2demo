package com.djn.mp4v2demo;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;

public class MainActivity extends Activity {
    private Button mStartBtn = null;

    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mStartBtn = (Button) findViewById(R.id.btn_start);
        mStartBtn.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(final View v) {
                demuxH264FromMp4();
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(final Menu menu) {
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(final MenuItem item) {
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onStart() {
        super.onStart();
        InitNative();
    }

    @Override
    protected void onStop() {
        super.onStop();
        DeinitNative();
    }

    private native int InitNative();

    private native int DeinitNative();

    private native int demuxH264FromMp4();

    static {
        System.loadLibrary("mp4v2");
        System.loadLibrary("Mp4v2demo");
    }
}
