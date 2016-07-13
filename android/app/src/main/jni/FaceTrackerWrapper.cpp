/*
* FaceTrackerWrapper.cpp
*
*  Created on: 2016-7-13
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include <android/log.h>

#include "FaceTrackerWrapper.h"
#include "FaceTracker/Tracker.h"

#ifndef LOG_TAG
#define LOG_TAG "wysaid"
#endif

#define  LOG_INFO(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOG_ERROR(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define LOG_CODE(...) __VA_ARGS__

class MyTracker
{
public:

	MyTracker() : _isTrackerOK(false), _hasFace(false)
	{
		int windowSizes[] = {
            13, 11, 9, 7
        };

        for(int i : windowSizes)
        {
        	_windowSize.push_back(i);
        }

        _resultPoints.resize(66);
	}

	void setup(const char *modelfname, const char *trifname, const char *confname)
	{
		if(!_isTrackerOK)
		{
			_tracker.Load(modelfname);
            _triModel = FACETRACKER::IO::LoadTri(trifname);
            _conModel = FACETRACKER::IO::LoadCon(confname);
            _isTrackerOK = true;
		}
	}

	void setup()
	{
		if(!_isTrackerOK)
		{
			const char* ftData =
#include "face2.tracker"
            ;
            const char* triData =
#include "face.tri"
            ;
            const char* conData =
#include "face.con"
            ;
            _tracker.LoadFromData(ftData);
            _triModel = FACETRACKER::IO::LoadTriByData(triData);
            _conModel = FACETRACKER::IO::LoadConByData(conData);
            _isTrackerOK = true;
		}
		LOG_INFO("Internal setup!");
	}

	inline FACETRACKER::Tracker& tracker() { return _tracker; }
	inline cv::Mat& tri() { return _triModel; }
	inline cv::Mat& con() { return _conModel; }
	inline bool isOK() { return _isTrackerOK; }
	inline std::vector<cv::Point2f>& keyPoints() { return _resultPoints; }
	inline bool hasFace() { return _hasFace; }
	inline void resetFrame() { _tracker.FrameReset(); }

	bool trackFace(cv::Mat& faceImage, int framePerDetection = -1, int iteration = 5, double clamp = 3.0, double tolerance = 0.01, bool checkStatus = false)
	{
        _hasFace = _tracker.Track(faceImage, _windowSize, framePerDetection, iteration, clamp, tolerance, checkStatus) == 0;

        if(_hasFace)
        {
        	LOG_INFO("find face!");
        	for(int i = 0; i < 66; ++i)
	        {
	            _resultPoints[i] = cv::Point2f(_tracker._shape.at<double>(i, 0), _tracker._shape.at<double>(i + 66, 0));
	        }
        }
        else
        {
        	LOG_INFO("no face!");
        	_tracker.FrameReset();
        }
        
        return _hasFace;
	}

protected:
	FACETRACKER::Tracker _tracker;
    cv::Mat _triModel;
    cv::Mat _conModel;

    std::vector<int> _windowSize;

    std::vector<cv::Point2f> _resultPoints;

    bool _isTrackerOK, _hasFace;
};

extern "C"
{
	JNIEXPORT jlong JNICALL Java_org_wysaid_FaceTracker_MainActivity_nativeCreateTracker(JNIEnv *env, jclass, jstring model, jstring tri, jstring con)
	{
		MyTracker* tracker = new MyTracker();

		if(model == nullptr || tri == nullptr || con == nullptr)
		{
			tracker->setup();
		}
		else
		{
			const char* modelStr = env->GetStringUTFChars(model, 0);
			const char* triStr = env->GetStringUTFChars(tri, 0);
			const char* conStr = env->GetStringUTFChars(con, 0);

			tracker->setup(modelStr, triStr, conStr);

			env->ReleaseStringUTFChars(model, modelStr);
			env->ReleaseStringUTFChars(tri, triStr);
			env->ReleaseStringUTFChars(con, conStr);
		}

		LOG_INFO("tracker created...");

		return (jlong)tracker;
	}

	JNIEXPORT void JNICALL Java_org_wysaid_FaceTracker_MainActivity_nativeReleaseTracker(JNIEnv *env, jobject, jlong addr)
	{
		MyTracker* tracker = (MyTracker*)addr;
		delete tracker;
		LOG_INFO("tracker release...");
	}

	JNIEXPORT void JNICALL Java_org_wysaid_FaceTracker_MainActivity_nativeResetTrackerFrame(JNIEnv *, jobject, jlong addr)
	{
		MyTracker* tracker = (MyTracker*)addr;
		tracker->resetFrame();
	}

	JNIEXPORT jfloatArray JNICALL Java_org_wysaid_FaceTracker_MainActivity_nativeTrackFace(JNIEnv *env, jobject, jlong addr, jobject dataBuffer, jint width, jint height, jint stride, jint channel)
	{
		MyTracker* tracker = (MyTracker*)addr;
		unsigned char* buffer = (unsigned char*)env->GetDirectBufferAddress(dataBuffer);

		if(buffer == nullptr)
		{
			LOG_ERROR("A direct buffer should be passed!");
		}

		LOG_INFO("Track face image - width: %d, height: %d, stride: %d, channel: %d\n", width, height, stride, channel);

		long cvType = channel == 1 ? CV_8UC1 : CV_8UC4;

		cv::Mat img(height, width, cvType, buffer, stride);
		cv::Mat grayImg;

		if(cvType != CV_8UC1)
		{
			cv::cvtColor(img, grayImg, cv::COLOR_BGRA2GRAY);
			LOG_INFO("Gray Image Generated!\n");
		}
		else
		{
			grayImg = img;
		}

		if(tracker->trackFace(grayImg))
		{
			jfloatArray arr = env->NewFloatArray(132);

			if(arr != nullptr)
			{
				auto& pnts = tracker->keyPoints();
				env->SetFloatArrayRegion(arr, 0, 132, (jfloat*)pnts.data());				
			}

			return arr;
		}

		return nullptr;
	}

}
