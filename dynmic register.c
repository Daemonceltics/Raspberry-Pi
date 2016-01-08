#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<jni.h>
#include<assert.h>



jstring native_ggPrintHello(JNIEnv* env,jclass clazz)
{
	return (*env)->NewStringUTF(env,"Hello frome GG-jni!");
}

/*
 * 方法对应表
 */
static JNINativeMethod gMethods[]={
		{"ggPrintHello","()Ljava/lang/String;",(void*)native_ggPrintHello},//绑定
};

/*
 * 为某个类注册本地方法
 */
static int registerNativeMethods(JNIEnv *env
		,const char* className
		,JNINativeMethod* gMethods,int numMethods)
{
	jclass clazz;
	clazz=(*env)->FindClass(env,className);
	if(clazz==NULL)
	{
		return JNI_FALSE;
	}
	if((*env)->RegisterNatives(env,clazz,gMethods,numMethods)<0)
	{
		return JNI_FALSE;
	}

	return JNI_TRUE;
}

/*
 * 为所有类注册本地方法
 */
static int registerNative(JNIEnv *env)
{
	const char* kClassName="com/ggndktest1/MainActivity";//指定要注册的类
	return registerNativeMethods(env,kClassName,gMethods,
			sizeof(gMethods)/sizeof(gMethods[0]));
}

/*
 * System.loadLibrary("lib")时调用
 * 如果成功返回JNI版本,失败返回-1
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm,void* reserved)
{
	JNIEnv *env=NULL;
	jint result=-1;

	if((*vm)->GetEnv(vm,(void**)&env,JNI_VERSION_1_4)!=JNI_OK)
	{
		return -1;
	}

	assert(env!=NULL);

	if(!registerNative(env))
	{
		return -1;
	}
	//成功
	result=JNI_VERSION_1_4;

	return result;
}

   

/*****************************************
*              C++版动态注册注册代码
*
*********************************************/
  




#include <string.h>  
#include <android/log.h>  
#include <jni.h> 
#define JNI_DEBUG

#ifdef JNI_DEBUG
#ifndef LOG_TAG
#define LOG_TAG "Britney"
#endif
#define LOGD(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#else
#define LOGD(...) do{} while(0)
#define LOGE(...) do{} while(0)
#endif 
  

/* 
 * Class:     com_example_nativeegl_MyRenderer 
 * Method:    nativeGetHelloString 
 * Signature: ()Ljava/lang/String; 
 */  
jstring  nativeGetHelloString(JNIEnv *env, jobject thiz) {  
      return env->NewStringUTF((char*)" This is calling from JNI suckers!");  
  }  
/* 
 * Class:     com_example_nativeegl_MyRenderer 
 * Method:    nativeDrawFrame 
 * Signature: ()V 
 */  
void nativeDrawFrame(JNIEnv *env, jobject obj) {  
  
}  
  
/* 
 * Class:     com_example_nativeegl_MyRenderer 
 * Method:    nativeSurfaceChanged 
 * Signature: (II)V 
 */  
void nativeSurfaceChanged(JNIEnv *env, jobject obj, jint width, jint height){  
  
}  
  
/* 
 * Class:     com_example_nativeegl_MyRenderer 
 * Method:    nativeSurfaceCreated 
 * Signature: ()V 
 */  
 void nativeSurfaceCreated(JNIEnv *env, jobject obj) {  
  
}  
  
//----------------------------JNI part:Native register------------------------------------------------------  
static JNINativeMethod gMethods[] = {  
        //{"native method name from Java","(arguments type)return type", "(void*)local_native_name"  
        {"nativeGetHelloString", "()Ljava/lang/String;", (void *)nativeGetHelloString},  
        {"nativeDrawFrame", "()V", (void *)nativeDrawFrame},  
        {"nativeSurfaceChanged", "(II)V", (void *)nativeSurfaceChanged},  
        {"nativeSurfaceCreated", "()V", (void *)nativeSurfaceCreated},  
};  
  
static const char* className="com/example/nativeegldynamicjni/MyRenderer";  
  
static int registerNativeMethods(JNIEnv *env) {  
    jclass clazz;  
    clazz = env->FindClass(className);  
    if (clazz == NULL) {  
        LOGD("failed to load the class %s", className);  
        return JNI_FALSE;  
    }  
    if (env->RegisterNatives(clazz, gMethods, sizeof(gMethods)/sizeof(gMethods[0])) < 0) {  
        return JNI_FALSE;  
    }  
    return JNI_TRUE;  
} //end of registerNativeMethods  
  
jint JNI_OnLoad(JavaVM* vm, void* reserved) {  
    JNIEnv* env = NULL;  
    jint result = -1;  
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK)  
    {  
        LOGE("ERROR: GetEnv failed\n");  
        goto bail;  
    }  
  
    if (registerNativeMethods(env) < 0) {  
        LOGE("ERROR: jnitest native registration failed\n");  
        goto bail;  
    }  
    result = JNI_VERSION_1_4;  
  
    bail:  
    return result;  
}


LOCAL_PATH := $(call my-dir)  
include $(CLEAR_VARS)  

LOCAL_LDLIBS += -llog  

include $(BUILD_SHARED_LIBRARY)






/*****************************************
*              C版动态注册注册代码
*
*********************************************/


static JNINativeMethod gMethods[] = {
        //{"native method name from Java","(arguments type)return type", "(void*)local_native_name"
        {"trigger", "()V", (void *)native_trigger},

};

static const char* className="com/example/own/MainActivity";

static int registerNativeMethods(JNIEnv *env) {
    jclass clazz;
    clazz = (*env)->FindClass(env,className);
    if (clazz == NULL) {
        LOGD("failed to load the class %s", className);
        return JNI_FALSE;
    }
    if ((*env)->RegisterNatives(env,clazz, gMethods, sizeof(gMethods)/sizeof(gMethods[0])) < 0) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
} //end of registerNativeMethods

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env = NULL;
    jint result = -1;


	if((*vm)->GetEnv(vm,(void**)&env,JNI_VERSION_1_4)!=JNI_OK)
    {
        LOGE("ERROR: GetEnv failed\n");
        goto bail;
    }

    if (registerNativeMethods(env) < 0) {
        LOGE("ERROR: jnitest native registration failed\n");
        goto bail;
    }
    result = JNI_VERSION_1_4;

    bail:
    return result;
}
