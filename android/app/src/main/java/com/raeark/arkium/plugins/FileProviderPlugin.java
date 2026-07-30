package com.raeark.arkium.plugins;

import android.content.Intent;
import android.net.Uri;
import android.content.Context;
import android.content.ContentResolver;

import com.getcapacitor.Plugin;
import com.getcapacitor.PluginCall;
import com.getcapacitor.annotation.ActivityCallback;
import com.getcapacitor.annotation.CapacitorPlugin;
import com.getcapacitor.PluginMethod;

@CapacitorPlugin(name = "FileProvider")
public class FileProviderPlugin extends Plugin {

    private static final int PICK_FOLDER = 1001;

    @PluginMethod()
    public void pickFolder(PluginCall call) {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE);
        intent.addFlags(
            Intent.FLAG_GRANT_READ_URI_PERMISSION |
            Intent.FLAG_GRANT_WRITE_URI_PERMISSION |
            Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION
        );

        startActivityForResult(call, intent, "folderPicked");
    }

    @ActivityCallback
    private void folderPicked(PluginCall call, Intent result) {
        if (result == null || result.getData() == null) {
            call.reject("No folder selected");
            return;
        }

        Uri uri = result.getData();

        getContext()
            .getContentResolver()
            .takePersistableUriPermission(
                uri,
                Intent.FLAG_GRANT_READ_URI_PERMISSION |
                Intent.FLAG_GRANT_WRITE_URI_PERMISSION
            );

        call.resolve(new com.getcapacitor.JSObject()
            .put("folderId", uri.toString())
        );
    }
}
