#include <jni.h>
#include <string.h>
#include <assert.h>
// for native asset manager
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include <android/log.h>

#define TAG "MICHAEL"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG , TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO , TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN , TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR , TAG, __VA_ARGS__)


void decoding(unsigned char* content, long len) {
	// 7 6 5 4  3 2 1 0
	// modify reverse the seq
	// 0 1 2 3 4 5 6 7
	int i = 0;
	for(i = 0; i < len; i += 2 ) { 
		content[i] = (content[i] << 4 ) | ( content[i] >> 4);
		content[i] = ((content[i] << 2 ) & 0xcc) | ((content[i] >> 2) & 0x33);
		content[i] = ((content[i] << 1 ) & 0xaa) | ((content[i] >> 1) & 0x55);
	}

	// modify every bit ^
	for(i = 0; i < len; i += 7 ) {
		content[i] = ~content[i];
	}

	// modify %5's 8 <----> %5+1's 8
	for(i = 0; i < len; i += 5 ) {
		if(( i + 1 )== len) {
			break;
		}
		unsigned temp = 0x80 & content[i+1];
		content[i+1] &= 0x7F;
		content[i+1] |= 0x80 & content[i];
		content[i] &= 0x7F;
		content[i] |= temp;
	}

	for(i = 0; i < len; i += 3 ) {
		unsigned char temp = 0x00 ;
		temp |= ( content[i] & 0x10 ) >> 1;
		temp |= ( content[i] & 0x08 ) << 1;
		content[i] = ((content[i] & 0xE7 ) | temp );
	}
}


jstring Java_com_unionsdk_pluginlayer_PluginLayer_getAsset(JNIEnv* env, jobject clazz,
		jobject assetManager) {
	AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
	assert(NULL != mgr);
	AAsset* asset = AAssetManager_open( mgr, "union-config.cj", AASSET_MODE_UNKNOWN);
	if( asset == NULL) {
		LOGI(" %s", "union-config.cj couldn't open");
		return (*env)->NewStringUTF(env, "");
		//return (*env)->GetStringUTFChars(env, "", NULL);
	}
	off_t bufferSize = AAsset_getLength(asset);
	unsigned char *buffer=(unsigned char *)malloc(bufferSize+1); 
	buffer[bufferSize]=0;
	int numBytesRead = AAsset_read(asset, buffer, bufferSize); 
	//decode the data
	decoding(buffer, bufferSize);
	//LOGI(": %s",buffer); 
	AAsset_close(asset); 
	//return (*env)->GetStringUTFChars(env, "", NULL);
	return (*env)->NewStringUTF(env, (char *)buffer);
}

