package com.hivefx.system_test009_downloadpicremote;
import java.net.HttpURLConnection;
import java.net.URL;

public class Downloader
{
    public static void downloadFile(String vUrl, String vOutputFile) {
        new Thread(() -> { // 避免阻塞主线程
            try {
                URL UrlObj = new URL(vUrl);
                HttpURLConnection Connection = (HttpURLConnection) UrlObj.openConnection();
                Connection.setRequestMethod("GET");
                Connection.connect();

                // 获取输入流并保存文件
                try (java.io.InputStream FileInpuStream = Connection.getInputStream();
                     java.io.FileOutputStream FileOutpuStream = new java.io.FileOutputStream(vOutputFile)) {
                    byte[] buffer = new byte[4096];
                    int FileLength;
                    while ((FileLength = FileInpuStream.read(buffer)) != -1) {
                        FileOutpuStream.write(buffer, 0, FileLength);
                    }
                }
                Connection.disconnect();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }).start();
    }
}
