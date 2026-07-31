package com.raeark.arkium.plugins;

import android.content.ContentResolver;
import android.content.Intent;
import android.net.Uri;
import androidx.documentfile.provider.DocumentFile;
import com.getcapacitor.JSArray;
import com.getcapacitor.JSObject;
import com.getcapacitor.Plugin;
import com.getcapacitor.PluginCall;
import com.getcapacitor.PluginMethod;
import com.getcapacitor.annotation.ActivityCallback;
import com.getcapacitor.annotation.CapacitorPlugin;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;

@CapacitorPlugin(name = "FileProvider")
public class FileProviderPlugin extends Plugin {

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
                Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION
            );

        call.resolve(new JSObject().put("folderId", uri.toString()));
    }

    @PluginMethod()
    public void listFiles(PluginCall call) {
        String folderId = call.getString("folderId");
        if (folderId == null) {
            call.reject("folderId is required");
            return;
        }

        Uri treeUri = Uri.parse(folderId);
        DocumentFile dir = DocumentFile.fromTreeUri(getContext(), treeUri);

        if (dir == null || !dir.isDirectory()) {
            call.reject("Folder not found or not accessible");
            return;
        }

        JSArray files = new JSArray();
        for (DocumentFile file : dir.listFiles()) {
            JSObject entry = new JSObject();
            entry.put("name", file.getName());
            entry.put("path", file.getUri().toString());
            entry.put("type", file.isDirectory() ? "directory" : "file");
            entry.put("size", file.length());
            files.put(entry);
        }

        JSObject result = new JSObject();
        result.put("files", files);
        call.resolve(result);
    }

    @PluginMethod()
    public void readFile(PluginCall call) {
        String path = call.getString("path");
        if (path == null) {
            call.reject("path is required");
            return;
        }

        Uri uri = Uri.parse(path);
        ContentResolver resolver = getContext().getContentResolver();

        try (InputStream inputStream = resolver.openInputStream(uri)) {
            if (inputStream == null) {
                call.reject("Unable to open file");
                return;
            }

            ByteArrayOutputStream buffer = new ByteArrayOutputStream();
            byte[] chunk = new byte[4096];
            int bytesRead;
            while ((bytesRead = inputStream.read(chunk)) != -1) {
                buffer.write(chunk, 0, bytesRead);
            }

            String content = buffer.toString("UTF-8");
            call.resolve(new JSObject().put("content", content));
        } catch (IOException e) {
            call.reject("Failed to read file: " + e.getMessage());
        }
    }
}
