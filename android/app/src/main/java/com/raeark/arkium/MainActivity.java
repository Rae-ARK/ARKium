package com.raeark.arkium;

import android.os.Bundle;

import com.getcapacitor.BridgeActivity;
import com.raeark.arkium.plugins.FileProviderPlugin;

public class MainActivity extends BridgeActivity {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        registerPlugin(FileProviderPlugin.class);
        super.onCreate(savedInstanceState);
    }
}
