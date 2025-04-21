package com.hivefx.system_test009_downloadpicremote;
import java.net.HttpURLConnection;
import java.net.URL;
import java.io.File;

public class Downloader
{
    public static boolean downloadFile(String vUrl, String vOutputFile)
    {
        try
        {
            URL UrlObj = new URL(vUrl);
            HttpURLConnection Connection = (HttpURLConnection) UrlObj.openConnection();

            Connection.setRequestMethod("GET");
            Connection.setConnectTimeout(15000);
            Connection.setReadTimeout(15000);
            Connection.connect();

            File OutputFile = new File(vOutputFile);
            File ParentDir = OutputFile.getParentFile();
            if (!ParentDir.exists())
            {
                ParentDir.mkdirs();
            }

            try (java.io.InputStream FileInpuStream = Connection.getInputStream();
                 java.io.FileOutputStream FileOutpuStream = new java.io.FileOutputStream(vOutputFile))
            {
                byte[] buffer = new byte[4096];
                int FileLength;
                while ((FileLength = FileInpuStream.read(buffer)) != -1) {
                    FileOutpuStream.write(buffer, 0, FileLength);
                }
            }
            Connection.disconnect();
        }
        catch (Exception e)
        {
            e.printStackTrace();
            return false;
        }
        return true;
    }
}
