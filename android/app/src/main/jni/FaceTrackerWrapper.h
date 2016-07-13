/*
* FaceTrackerWrapper.h
*
*  Created on: 2016-7-13
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#ifndef _FaceTrackerWrapper_h_
#define _FaceTrackerWrapper_h_

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL Java_org_wysaid_FaceTracker_MainActivity_nativeCreateTracker
  (JNIEnv *env, jclass, jstring model, jstring tri, jstring con);

JNIEXPORT void JNICALL Java_org_wysaid_FaceTracker_MainActivity_nativeReleaseTracker
  (JNIEnv *, jobject, jlong);

JNIEXPORT jfloatArray JNICALL Java_org_wysaid_FaceTracker_MainActivity_nativeTrackFace
  (JNIEnv *, jobject, jlong, jobject, jint, jint, jint, jint);

JNIEXPORT void JNICALL Java_org_wysaid_FaceTracker_MainActivity_nativeResetTrackerFrame
  (JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif


#endif