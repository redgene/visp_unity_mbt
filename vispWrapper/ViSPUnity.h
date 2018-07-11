#pragma once
#define TESTDLL_API __declspec(dllexport)

//#include <visp3/core/vpConfig.h>
//#include <visp3/core/vpMatrix.h>
//#include <visp3/core/vpMath.h>
//#include <visp3/core/vpTranslationVector.h>
//#include <visp3/core/vpImage.h>
//#include <visp3/core/vpImageConvert.h>
//#include <visp3/core/vpImagePoint.h>
//#include <visp3/core/vpPoseVector.h>
//#include <visp3/core/vpRect.h>
//#include <visp3/core/vpPolygon.h>
//#include <visp3/core/vpXmlParserCamera.h>
//#include <visp3/core/vpArray2D.h>

#include <visp3/visp_core.h>
#include <visp3/io/vpImageIo.h>
#include <visp3/blob/vpDot2.h>
#include <visp3/detection/vpDetectorAprilTag.h>
#include <visp3/mbt/vpMbGenericTracker.h>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <ios>

using namespace std;
extern "C" {
	//TESTDLL_API void createCaoFile(double cubeEdgeSize);
	//TESTDLL_API void AprilTagMBT(unsigned char* const bitmap, int height, int width, double *pointx, double *pointy, double* kltX, double* kltY, int* kltNumber, int t, int e);
	//TESTDLL_API void InitMBT(double cam_px, double cam_py, double cam_u0, double cam_v0, int t);
	
	VISP_EXPORT void createCaoFile(double cubeEdgeSize);
	VISP_EXPORT void AprilTagMBT(unsigned char* const bitmap, int height, int width, double *pointx, double *pointy, double* kltX, double* kltY, int* kltNumber, int t, int e);
	VISP_EXPORT void InitMBT(double cam_px, double cam_py, double cam_u0, double cam_v0, int t);
}
