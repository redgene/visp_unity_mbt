#pragma once

#define TESTDLL_API __declspec(dllexport)

#include <visp3/core/vpConfig.h>
#include <visp3/core/vpMatrix.h>
#include <visp3/core/vpMath.h>
#include <visp3/core/vpTranslationVector.h>
#include <visp3/core/vpImage.h>
#include <visp3/io/vpImageIo.h>
#include <visp3/core/vpImageConvert.h>
#include <visp3/blob/vpDot2.h>
#include <visp3/core/vpImagePoint.h>

#include <iostream>
#include <string>
#include <sstream>

#include <visp3/detection/vpDetectorAprilTag.h>
#include <visp3/io/vpImageIo.h>
#include <visp3/core/vpPoseVector.h>
#include <visp3/core/vpRect.h>
#include <visp3/core/vpPolygon.h>

#include <fstream>
#include <ios>
#include <iostream>

#include <visp3/gui/vpDisplayGDI.h>
#include <visp3/gui/vpDisplayOpenCV.h>
#include <visp3/gui/vpDisplayX.h>
#include <visp3/core/vpXmlParserCamera.h>
#include <visp3/sensor/vpV4l2Grabber.h>
#include <visp3/detection/vpDetectorAprilTag.h>
#include <visp3/mbt/vpMbGenericTracker.h>


using namespace std;
extern "C" {
	TESTDLL_API void createCaoFile(double cubeEdgeSize);
	TESTDLL_API void AprilTagMBT(unsigned char* const bitmap, int height, int width, double *pointx, double *pointy);
	TESTDLL_API void InitMBT();
}
