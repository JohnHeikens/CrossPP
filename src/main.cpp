#include "main.h"
// #include <crtdbg.h>
#include <iostream>
#include "constants.h"
#include "gameForm.h"
#include "application/application.h"
#include "globalFunctions.h"
#include "array/wstringFunctions.h"
#include <thread>
#include <filesystem>
#include <string>
#include "folderList.h"
#include <filesystem/filemanager.h>

#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Config.hpp>
#include "application/thread/setThreadName.h"

#if onWindows
#include "windowsIncluder.h"
#include <corecrt.h>
#include <shlobj.h>
#elif onAndroid
stdPath androidCacheDir;

// Since we want to get the native activity from SFML, we'll have to use an
// extra header here:
#include <SFML/System/NativeActivity.hpp>
#include <android/native_activity.h>
#include <jni.h>
// NDK/JNI sub example - call Java code from native code
//int vibrate(sf::Time duration)
//{
//    // First we'll need the native activity handle
//    ANativeActivity* activity = sf::getNativeActivity();
//
//    // Retrieve the JVM and JNI environment
//    JavaVM* vm  = activity->vm;
//    JNIEnv* env = activity->env;
//
//    // First, attach this thread to the main thread
//    JavaVMAttachArgs attachargs;
//    attachargs.version = JNI_VERSION_1_6;
//    attachargs.name    = "NativeThread";
//    attachargs.group   = nullptr;
//    jint res           = vm->AttachCurrentThread(&env, &attachargs);
//
//    if (res == JNI_ERR)
//        return EXIT_FAILURE;
//
//    // Retrieve class information
//    jclass natact  = env->FindClass("android/app/NativeActivity");
//    jclass context = env->FindClass("android/content/Context");
//
//    // Get the value of a constant
//    jfieldID fid    = env->GetStaticFieldID(context, "VIBRATOR_SERVICE", "Ljava/lang/String;");
//    jobject  svcstr = env->GetStaticObjectField(context, fid);
//
//    // Get the method 'getSystemService' and call it
//    jmethodID getss   = env->GetMethodID(natact, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
//    jobject   vib_obj = env->CallObjectMethod(activity->clazz, getss, svcstr);
//
//    // Get the object's class and retrieve the member name
//    jclass    vib_cls = env->GetObjectClass(vib_obj);
//    jmethodID vibrate = env->GetMethodID(vib_cls, "vibrate", "(J)V");
//
//    // Determine the timeframe
//    jlong length = duration.asMilliseconds();
//
//    // Bzzz!
//    env->CallVoidMethod(vib_obj, vibrate, length);
//
//    // Free references
//    env->DeleteLocalRef(vib_obj);
//    env->DeleteLocalRef(vib_cls);
//    env->DeleteLocalRef(svcstr);
//    env->DeleteLocalRef(context);
//    env->DeleteLocalRef(natact);
//
//    // Detach thread again
//    vm->DetachCurrentThread();
//    return 0;
//}
#endif

bool loadingFinished = false;
sf::RenderWindow* window = nullptr;

void showLoadingScreen() {
    // Create the main rendering window
    window = application::createWindow(gameName);

    sf::Font font;
    //this will also work on android when the data folder has not been copied yet,
    //as it will look for data/font.otf in the assets folder.
    font.loadFromFile("data/font.otf");

    // Start game loop
    while (window->isOpen()) {
        // Process events
        sf::Event Event;
        while (window->pollEvent(Event)) {
            // Close window : exit
            if (Event.type == sf::Event::Closed)
                window->close();
        }
        if (loadingFinished) {
            //don't close the window, we will use it for later
            //window->close();
            return;
        }

        // Clear the screen (fill it with black color)
        window->clear(sf::Color::Magenta);

        sf::Text loadingText(
                "Loading medieval survival\nwhen you updated this app,\nthis might take several minutes.\nDO NOT CLOSE",
                font);
        loadingText.setFillColor(sf::Color::White);
        loadingText.setCharacterSize(math::minimum(window->getView().getSize().y, window->getView().getSize().x) / 20);


        //center text
        sf::FloatRect textRect = loadingText.getLocalBounds();
        loadingText.setOrigin(textRect.left + textRect.width / 2.0f,
                       textRect.top + textRect.height / 2.0f);
        loadingText.setPosition(window->getView().getCenter());

        window->draw(loadingText);

        // Display window contents on screen
        window->display();
    }
}

std::filesystem::path getCommonAppDataFolder() {
#if onWindows
    TCHAR szPath[MAX_PATH];
    // https://stackoverflow.com/questions/2899013/how-do-i-get-the-application-data-path-in-windows-using-c
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szPath)))
    {
        // to make this work, #define UNICODE is defined in "windowsIncluder.h"
        return std::filesystem::path(&szPath[0]);
    }
#elif onAndroid

    // First we'll need the native activity handle
    ANativeActivity *activity = sf::getNativeActivity();


    JNIEnv *env = activity->env;
    JavaVM *vm = activity->vm;

    // First, attach this thread to the main thread
    JavaVMAttachArgs attachargs;
    attachargs.version = JNI_VERSION_1_6;
    attachargs.name = "NativeThread";
    attachargs.group = nullptr;
    jint res = vm->AttachCurrentThread(&env, &attachargs);
    if (res == JNI_ERR) {
        handleError(L"couldn't attach to current thread");
        return L"";
    }



    //https://stackoverflow.com/questions/7701801/obtaining-the-name-of-an-android-apk-using-c-and-the-nativeactivity-class
    jobject obj = activity->clazz;
    jclass nativeActivityClass = env->GetObjectClass(obj);


    jmethodID getApplicationContextMethod = env->GetMethodID(nativeActivityClass,
                                                             "getApplicationContext",
                                                             "()Landroid/content/Context;");
    jobject applicationContext = env->CallObjectMethod(activity->clazz,
                                                       getApplicationContextMethod);

    jmethodID getClassLoader = env->GetMethodID(nativeActivityClass, "getClassLoader",
                                                "()Ljava/lang/ClassLoader;");
    jobject classLoader = env->CallObjectMethod(activity->clazz, getClassLoader);
    jclass classLoaderClass = env->FindClass("java/lang/ClassLoader");
    jmethodID findClass = env->GetMethodID(classLoaderClass, "loadClass",
                                           "(Ljava/lang/String;)Ljava/lang/Class;");
    jstring strClassName = env->NewStringUTF("com.johnheikens.medievalsurvival.JavaHelper");
    jclass javaHelperClass = (jclass) (env->CallObjectMethod(classLoader, findClass, strClassName));

    //jclass javaHelperClass = env->FindClass("com/johnheikens/medievalsurvival/JavaHelper");
    jmethodID initializeContextMethod = env->GetStaticMethodID(javaHelperClass, "initializeContext",
                                                               "(Landroid/app/Activity;)Ljava/lang/String;");
    jobject jExtractedAssetsDir = env->CallStaticObjectMethod(javaHelperClass,
                                                              initializeContextMethod,
                                                              activity->clazz);
    const char *assetsDir = env->GetStringUTFChars((jstring) jExtractedAssetsDir, nullptr);
    androidCacheDir = assetsDir;
    androidCacheDir = androidCacheDir.parent_path();//remove the 'data/'


    env->ReleaseStringUTFChars((jstring) jExtractedAssetsDir, assetsDir);


    env->DeleteLocalRef(javaHelperClass);
    env->DeleteLocalRef(strClassName);
    env->DeleteLocalRef(classLoaderClass);
    env->DeleteLocalRef(classLoader);
    env->DeleteLocalRef(applicationContext);
    env->DeleteLocalRef(nativeActivityClass);

    vm->DetachCurrentThread();


    return androidCacheDir;
#elif onLinux
    return L"/user/share";

#elif onMac
    return L"/Library/Application Support/";
#endif

    handleError(L"common appdata folder not found");
    return std::filesystem::path();
}

// #include <imgui-SFML.h>
// #include <imgui.h>

bool mainRunning = false;

int main(int argc, char *argv[]) {
    if (mainRunning) {
        std::cout
                << "main function is being called again. this is probably because of a changed config in android";
        //the main function is called
        return 0;
        handleError(L"main is somehow called twice");
    }
    mainRunning = true;
    std::thread loadingThread(showLoadingScreen);
    // check if the application is installed or if we're debugging
    //
    if (onAndroid || !std::filesystem::exists(L"data")) { // this application is not deployed
        // change data folder to %appdata%/JohnHeikens/Medieval Survival/data
        //
        // TODO: get common appdata folder on every system
        std::filesystem::path commonAppDataFolder = getCommonAppDataFolder();
        std::filesystem::path newWorkingDirectory = onAndroid ? commonAppDataFolder :
                                                    commonAppDataFolder / L"JohnHeikens" /
                                                    L"Medieval Survival";
        if (std::filesystem::exists(newWorkingDirectory)) {
            std::filesystem::current_path(newWorkingDirectory); // setting path
#if onAndroid
//copy files from APK to working directory

#endif
        } else {
            handleError(newWorkingDirectory.wstring() + L" not found");
        }
        //
        // check if it worked
        if (std::filesystem::exists(L"data")) {
            std::cout << "true";
        } else {
            handleError(L"data folder not found in " + commonAppDataFolder.wstring());
        }
    }
    workingDirectory = std::filesystem::current_path();
    createFoldersIfNotExists(savesFolder);
    //
    // fontFamily* family = new fontFamily(new texture(std::wstring(L"data\\ascii.png"), true));
    // defaultTheme = new theme(new font(family));
    currentApplication = new application(new gameForm(), gameName);
    setCurrentThreadName(L"client thread");
    loadingFinished = true;
    loadingThread.join();
    currentApplication->window = window;
    cint &result = currentApplication->run();
    //
    //
    delete currentApplication;
    //_CrtDumpMemoryLeaks();
    return result;
}

std::filesystem::path workingDirectory;

//#if onAndroid
//#include <jni.h>
//
//extern "C" JNIEXPORT void JNICALL
//Java_com_johnheikens_medievalsurvival_MainActivity_nativeMain(JNIEnv *env, jobject thiz,
//                                                              jobject destination_dir) {
//    // Call the main() function of your C++ code
//
//    // Get the class of the destination_dir object
//    jclass fileClass = env->GetObjectClass(destination_dir);
//
//    // Find the getAbsolutePath() method of the File class
//    jmethodID getAbsolutePathMethod = env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
//
//    // Call getAbsolutePath() to get the absolute path of the directory
//    jstring absolutePathString = (jstring) env->CallObjectMethod(destination_dir, getAbsolutePathMethod);
//
//    // Convert the absolute path string to a C-style string
//    const char *absolutePath = env->GetStringUTFChars(absolutePathString, nullptr);
//
//    androidCacheDir = absolutePath;//convert the C-style string to a path object
//    // Release the UTF characters
//    env->ReleaseStringUTFChars(absolutePathString, absolutePath);
//
//    main(0, nullptr);
//}
//#endif
//

//
// std::wstring getUserAppDataFolder()
//{
//	char* pValue;
//	size_t len;
//	errno_t err = _dupenv_s(&pValue, &len, "APPDATA");
//	if (pValue && !err)
//	{
//		std::wstring appDataFolder = stringToWString(std::string(pValue, len));
//		delete pValue;
//		return appDataFolder;
//	}
//	handleError(L"user appdata folder not found");
//	return L"";
//}
//
// int WINAPI WinMain(
//	HINSTANCE hInstance,
//	HINSTANCE hPrevInstance,
//	LPSTR lpCmdLine,
//	int nShowCmd)
//{
//
//	std::wstring arguments = stringToWString(std::string(lpCmdLine));
//
//	//check if the application is installed or if we're debugging
//
//	if (!std::filesystem::exists(L"data")) {//this application is not deployed
//		//change data folder to %appdata%/JohnHeikens/Medieval Survival/data
//
//		std::wstring commonAppDataFolder = getCommonAppDataFolder();
//		std::wstring newWorkingDirectory = commonAppDataFolder / L"\\JohnHeikens\\Medieval Survival";
//		if (std::filesystem::exists(newWorkingDirectory)) {
//			std::filesystem::current_path(newWorkingDirectory); //setting path
//		}
//		else {
//			handleError(newWorkingDirectory + L" not found");
//		}
//
//		//check if it worked
//		if (std::filesystem::exists(L"data")) {
//			std::cout << "true";
//		}
//		else {
//			handleError(L"data folder not found in " + commonAppDataFolder);
//		}
//	}
//	workingDirectory = std::filesystem::current_path();
//	createFoldersIfNotExists(savesFolder);
//
//	//fontFamily* family = new fontFamily(new texture(std::wstring(L"data\\ascii.png"), true));
//	//defaultTheme = new theme(new font(family));
//	currentApplication = new application(new gameForm(), hInstance);
//	currentApplication->windowCaption = gameName;
//	cint& result = currentApplication->run();
//
//
//	delete currentApplication;
//	_CrtDumpMemoryLeaks();
//	return result;
//}