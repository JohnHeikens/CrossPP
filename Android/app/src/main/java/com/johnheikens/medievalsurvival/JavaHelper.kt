package com.johnheikens.medievalsurvival

import android.content.Context
import android.content.res.AssetManager
import java.io.File
import java.io.FileOutputStream
import java.io.IOException
import java.io.OutputStream
import android.util.Log
class JavaHelper {
    companion object{


        @JvmStatic fun initializeContext(context: Context): String {
            print("initializing the context")
            val assetDir = "data" // Assuming "data" is the directory you want to copy recursively
            val destinationDir = File(context.filesDir, assetDir)//context.cacheDir

            //when the 'data' folder is not found, then we extract the data folder.
            if(!destinationDir.exists())
            {
                print("didn't find the data folder")
                copyAssetsToCache(context, assetDir, destinationDir)
            }
            return destinationDir.toString();
        }
        @JvmStatic fun copyAssetsToCache(context: Context, assetDir: String, destinationDir: File) {
            val assetManager = context.assets
            try {
                val assets = assetManager.list(assetDir)
                if (assets != null && assets.size > 0) {
                    // If the assetDir is a directory
                    destinationDir.mkdirs()
                    for (asset in assets) {
                        val assetPath = assetDir + File.separator + asset
                        val destinationPath = destinationDir.absolutePath + File.separator + asset
                        val newDestinationDir = File(destinationDir, asset)

                        copyAssetsToCache(context, assetPath, newDestinationDir)
                        //copyAssetFile(assetManager, assetPath, destinationPath)
                    }
                } else {
                    // If the assetDir is a file
                    copyAssetFile(assetManager, assetDir, destinationDir.path)
                    //val subDirs = assetManager.list(assetDir)
                    //if (subDirs != null) {
                    //    for (subDir in subDirs) {
                    //        val newAssetDir = assetDir + File.separator + subDir
                    //        val newDestinationDir = File(destinationDir, subDir)
                    //        copyAssetsToCache(context, newAssetDir, newDestinationDir)
                    //    }
                    //}
                }
            } catch (e: IOException) {
                e.printStackTrace()
            }
        }

        @Throws(IOException::class)
        @JvmStatic private fun copyAssetFile(
            assetManager: AssetManager,
            assetPath: String,
            destinationPath: String
        ) {
            val `in` = assetManager.open(assetPath)
            val out: OutputStream = FileOutputStream(destinationPath)
            val buffer = ByteArray(1024)
            var read: Int
            while (`in`.read(buffer).also { read = it } != -1) {
                out.write(buffer, 0, read)
            }
            `in`.close()
            out.close()
        }
    }
}