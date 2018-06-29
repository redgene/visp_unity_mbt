#include "ViSPUnity.h"
#include <visp3/core/vpArray2D.h>
#include <visp3/core/vpRect.h>
#include <visp3/mbt/vpMbGenericTracker.h>

extern "C" {

	vpImage<unsigned char> I;
	vpDetectorAprilTag detector(vpDetectorAprilTag::TAG_36h11);
	vpHomogeneousMatrix cMo;
	vpCameraParameters cam;

	vpMbGenericTracker tracker;

	typedef enum {
		state_detection,
		state_tracking,
		state_quit
	} state_t;

	state_t state;

	int opt_device = 0;
	vpDetectorAprilTag::vpAprilTagFamily opt_tag_family = vpDetectorAprilTag::TAG_36h11;


	double opt_tag_size = 0.08;
	float opt_quad_decimate = 1.0;
	int opt_nthreads = 1;
	std::string opt_intrinsic_file = "";
	std::string opt_camera_name = "cam1";
	double opt_cube_size = 0.125; // 12.5cm by default: also sent from unity for creating cao file
	double projection_error_threshold = 40.;

	//Function called for detection only
	state_t detectAprilTag(const vpImage<unsigned char> &I, vpDetectorAprilTag &detector,
		double tagSize, const vpCameraParameters &cam, vpHomogeneousMatrix &cMo)
	{
		std::vector<vpHomogeneousMatrix> cMo_vec;

		// Detection
		bool ret = detector.detect(I, tagSize, cam, cMo_vec);
		if (ret && detector.getNbObjects() > 0) { // if tag detected, we pick the first one
			cMo = cMo_vec[0];
			return state_tracking;
		}
		return state_detection;
	}


	void InitMBT(double cam_px, double cam_py, double cam_u0, double cam_v0) {
		cam.initPersProjWithoutDistortion(cam_px, cam_py, cam_u0, cam_v0);

		// Initialize AprilTag detector
		//detector.set (opt_tag_family);
		detector.setAprilTagQuadDecimate(opt_quad_decimate);
		detector.setAprilTagNbThreads(opt_nthreads);

		// Prepare MBT
		tracker.setTrackerType(vpMbGenericTracker::EDGE_TRACKER);
		tracker.getCameraParameters(cam);
		/*bool displayFullModel = false;*/

		// edges
		vpMe me;
		me.setMaskSize(5);
		me.setMaskNumber(180);
		me.setRange(12);
		me.setThreshold(10000);
		me.setMu1(0.5);
		me.setMu2(0.5);
		me.setSampleStep(4);
		tracker.setMovingEdge(me);

		// camera calibration params
		tracker.setCameraParameters(cam);

		// model definition
		tracker.loadModel("cube.cao");
		tracker.setDisplayFeatures(true);
		tracker.setAngleAppear(vpMath::rad(70));
		tracker.setAngleDisappear(vpMath::rad(80));

		state = state_detection;
	}

	void AprilTagMBT(unsigned char* const bitmap, int height, int width, double* pointx, double* pointy) {

		//The following loop flips the bitmap
		for (int r = 0; r < height; r++) {
			for (int c = 0; c < width / 2; c++) {
				unsigned char temp = bitmap[r*width + c];
				bitmap[r*width + c] = bitmap[r*width + width - c - 1];
				bitmap[r*width + width - c - 1] = temp;
			}
		}
		I.resize(height, width);
		I.bitmap = bitmap;

		if (state == state_detection) {
			state = detectAprilTag(I, detector, opt_tag_size, cam, cMo);

			// Initialize the tracker with the result of the detection
			if (state == state_tracking) {
				tracker.initFromPose(I, cMo);
			}
		}

		if (state == state_tracking) {
			//state = track(I, tracker, opt_projection_error_threshold, opt_camera_name, cMo);
			try {
				tracker.track(I);

				// get the lines currently tracked of the model
				std::list<vpMbtDistanceLine *> edges;
				tracker.getLline("Camera", edges, 0);
				int i = 0;

				for (std::list<vpMbtDistanceLine *>::const_iterator it = edges.begin(); it != edges.end(); ++it) {
					vpPoint *P1 = (*it)->p1; 
					vpPoint *P2 = (*it)->p2;
					P1->changeFrame(cMo);
					P2->changeFrame(cMo);
					P1->project();
					P2->project();
					vpImagePoint IP1, IP2;
					vpMeterPixelConversion::convertPoint(cam, P1->get_x(), P1->get_y(), IP1);
					vpMeterPixelConversion::convertPoint(cam, P2->get_x(), P2->get_y(), IP2);
					pointx[i] = IP1.get_u();
					pointy[i] = IP1.get_v();
					i++;
					pointx[i] = IP2.get_u();
					pointy[i] = IP2.get_v();
					i++;
				}

				tracker.getPose(cMo);

				// Detect tracking error
				double projection_error = tracker.computeCurrentProjectionError(I, cMo, cam);
				if (projection_error > projection_error_threshold) {
					state = state_detection;
				}
				else {
					state = state_tracking;
				}
			}
			catch (...) {
				state = state_detection;
			}
		}
	}

	// Creates a cube.cao file in your current directory (in unityProject folder)
	// cubeEdgeSize : size of cube edges in meters 
	void createCaoFile(double cubeEdgeSize)
	{
		std::ofstream fileStream;
		fileStream.open("cube.cao", std::ofstream::out | std::ofstream::trunc);
		fileStream << "V1\n";
		fileStream << "# 3D Points\n";
		fileStream << "8                  # Number of points\n";
		fileStream << cubeEdgeSize / 2 << " " << cubeEdgeSize / 2 << " " << 0 << "    # Point 0: (X, Y, Z)\n";
		fileStream << cubeEdgeSize / 2 << " " << -cubeEdgeSize / 2 << " " << 0 << "    # Point 1\n";
		fileStream << -cubeEdgeSize / 2 << " " << -cubeEdgeSize / 2 << " " << 0 << "    # Point 2\n";
		fileStream << -cubeEdgeSize / 2 << " " << cubeEdgeSize / 2 << " " << 0 << "    # Point 3\n";
		fileStream << -cubeEdgeSize / 2 << " " << cubeEdgeSize / 2 << " " << -cubeEdgeSize << "    # Point 4\n";
		fileStream << -cubeEdgeSize / 2 << " " << -cubeEdgeSize / 2 << " " << -cubeEdgeSize << "    # Point 5\n";
		fileStream << cubeEdgeSize / 2 << " " << -cubeEdgeSize / 2 << " " << -cubeEdgeSize << "    # Point 6\n";
		fileStream << cubeEdgeSize / 2 << " " << cubeEdgeSize / 2 << " " << -cubeEdgeSize << "    # Point 7\n";
		fileStream << "# 3D Lines\n";
		fileStream << "0                  # Number of lines\n";
		fileStream << "# Faces from 3D lines\n";
		fileStream << "0                  # Number of faces\n";
		fileStream << "# Faces from 3D points\n";
		fileStream << "6                  # Number of faces\n";
		fileStream << "4 0 3 2 1          # Face 0: [number of points] [index of the 3D points]...\n";
		fileStream << "4 1 2 5 6\n";
		fileStream << "4 4 7 6 5\n";
		fileStream << "4 0 7 4 3\n";
		fileStream << "4 5 2 3 4\n";
		fileStream << "4 0 1 6 7          # Face 5\n";
		fileStream << "# 3D cylinders\n";
		fileStream << "0                  # Number of cylinders\n";
		fileStream << "# 3D circles\n";
		fileStream << "0                  # Number of circles\n";
		fileStream.close();
	}
}

