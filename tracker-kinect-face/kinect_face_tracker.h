


#include <cmath>

#include "api/plugin-api.hpp"
#include "compat/timer.hpp"
#include "compat/macros.hpp"
#include "cv/video-widget.hpp"

// Kinect Header files
#include <Kinect.h>
#include <Kinect.Face.h>

#pragma once

// @deprecated Use UniqueInterface instead. Remove it at some point.
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != nullptr)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = nullptr;
	}
}

template<class Interface>
inline void ReleaseInterface(Interface* pInterfaceToRelease)
{
	if (pInterfaceToRelease != nullptr)
	{
		pInterfaceToRelease->Release();
	}
}

// Safely use Microsoft interfaces.
template<typename T>
class UniqueInterface : public std::unique_ptr<T, decltype(&ReleaseInterface<T>)> ///**/
{
public:
	UniqueInterface() : std::unique_ptr<T, decltype(&ReleaseInterface<T>)>(nullptr, ReleaseInterface<T>){}
	// Access pointer, typically for creation
	T** PtrPtr() { return &iPtr; };
	// Called this once the pointer was created
	void Reset() { std::unique_ptr<T, decltype(&ReleaseInterface<T>)>::reset(iPtr); }
	// If ever you want to release that interface before the object is deleted
	void Free() { iPtr = nullptr; Reset(); }	
private:
	T* iPtr = nullptr;
};


//
//
//
class KinectFaceTracker : public ITracker
{
public:
	KinectFaceTracker();
	~KinectFaceTracker() override;
	module_status start_tracker(QFrame* aFrame) override;
	void data(double *data) override;
	bool center() override;

private:
	Timer t;

	// Kinect stuff
	static const int       cColorWidth = 1920;
	static const int       cColorHeight = 1080;


	void Update();
	HRESULT InitializeDefaultSensor();
	void ProcessFaces();
	HRESULT UpdateBodyData(IBody** ppBodies);
	void ExtractFaceRotationInDegrees(const Vector4* pQuaternion, float* pPitch, float* pYaw, float* pRoll);
	static IBody* FindClosestBody(IBody** aBodies);
	static IBody* FindTrackedBodyById(IBody** aBodies,UINT64 aTrackingId);

	// Current Kinect
	IKinectSensor*         m_pKinectSensor;

	// Coordinate mapper
	ICoordinateMapper*     m_pCoordinateMapper;

	// Color reader
	IColorFrameReader*     m_pColorFrameReader;

	// Body reader
	IBodyFrameReader*      m_pBodyFrameReader;

	// Face sources
	IHighDefinitionFaceFrameSource*	   m_pFaceFrameSource;

	// Face readers
	IHighDefinitionFaceFrameReader*	   m_pFaceFrameReader;

	//
	RGBQUAD*               m_pColorRGBX;

	RectI iFaceBox = { 0 };

	CameraSpacePoint iLastFacePosition;
	CameraSpacePoint iFacePosition;
	CameraSpacePoint iFacePositionCenter;

	Vector4 iFaceRotationQuaternion;
	// As Yaw, Pitch, Roll
	CameraSpacePoint iLastFaceRotation;
	CameraSpacePoint iFaceRotation;
	CameraSpacePoint iFaceRotationCenter;
	//
	std::unique_ptr<cv_video_widget> iVideoWidget;
	std::unique_ptr<QLayout> iLayout;

	// Id of the body currently being tracked
	UINT64 iTrackingId = 0;
};
