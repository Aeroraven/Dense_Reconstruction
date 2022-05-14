#include "../../include/Test/tsTest.h"
#include "../../include/Misc/msAuxiliaryUtils.h"
#include "../../include/StereoRectification/srStereoRectification.h"
#include "../../include/Common/cmAlgo.h"
#include "../../include/SemiGlobalMatching/CostCalculator/smCostCalculator.h"
#include "../../include/SemiGlobalMatching/CostCalculator/smCensusTransformCostCalculator.h"
#include "../../include/SemiGlobalMatching/CostCalculator/smFourPathCostAggregator.h"
#include "../../include/SemiGlobalMatching/CostCalculator/smEightPathCostAggregator.h"
#include "../../include/SemiGlobalMatching/CostOptimizer/smCostOptimizer.h"
#include "../../include/SemiGlobalMatching/Helper/smCostHelper.h"
#include "../../include/SemiGlobalMatching/DepthEstimator/smDepthConverter.h"
#include "../../include/SemiGlobalMatching/Helper/smDisparityHelper.h"
#include "../../include/DenseReconstruction/Voxel/drPlainVoxelStore.h"
#include "../../include/DenseReconstruction/TSDF/drTSDF.h"
#include "../../include/Common/Utility/cmVisExt.h"
#include "../../include/StereoRectification/srStereoRectification.h"

namespace Test{
    void Test::stereoRectify(){
        StereoRectification::StereoRectification* stereoRectify = new StereoRectification::StereoRectification();
        cv::Mat camItr = cv::imread("E:\\60fps_images_archieve\\scene_00_0001.png", 0);
        cv::Mat camItl = cv::imread("E:\\60fps_images_archieve\\scene_00_0002.png", 0);
        Common::Camera::MonocularCameraIntrinsic camIntb,camIntc;
        Common::Camera::MonocularCameraExtrinsic camExtb,camExtc;
        Misc::AuxiliaryUtils::msParseIntrinsic("E:\\60fps_GT_archieve\\scene_00_0001.txt",&camIntc);
        Misc::AuxiliaryUtils::msParseExtrinsic("E:\\60fps_GT_archieve\\scene_00_0001.txt",&camExtc);
        Misc::AuxiliaryUtils::msParseIntrinsic("E:\\60fps_GT_archieve\\scene_00_0002.txt",&camIntb);
        Misc::AuxiliaryUtils::msParseExtrinsic("E:\\60fps_GT_archieve\\scene_00_0002.txt",&camExtb);
        
        dbg_trace(dbg_output<<"Here In";);
        //Starts
        cv::Mat rectImL,rectImR,rectImLM,rectImRM;
        cv::Mat HL,HR,KL,KR,RL,RR,DD;
        stereoRectify->stereoRectify(&camItl,&camItr,&camIntb,&camIntc,&camExtb,&camExtc,&rectImL,&rectImR,&rectImLM,&rectImRM,&HL,&HR,&RL,&RR,&HL,&HR,&DD);

        //View
        cv::imshow("Left", rectImL);
        cv::imshow("Right", rectImR);
        cv::waitKey(0);
    }

    void Test::epipolarLineProjection() {
		using namespace std;
        cv::Mat camItr = cv::imread("E:\\60fps_images_archieve\\scene_00_0001.png", 0);
        cv::Mat camItl = cv::imread("E:\\60fps_images_archieve\\scene_00_0002.png", 0);
        Common::Camera::MonocularCameraIntrinsic camIntb, camIntc;
        Common::Camera::MonocularCameraExtrinsic camExtb, camExtc;
        Misc::AuxiliaryUtils::msParseIntrinsic("E:\\60fps_GT_archieve\\scene_00_0001.txt", &camIntc);
        Misc::AuxiliaryUtils::msParseExtrinsic("E:\\60fps_GT_archieve\\scene_00_0001.txt", &camExtc);
        Misc::AuxiliaryUtils::msParseIntrinsic("E:\\60fps_GT_archieve\\scene_00_0002.txt", &camIntb);
        Misc::AuxiliaryUtils::msParseExtrinsic("E:\\60fps_GT_archieve\\scene_00_0002.txt", &camExtb);

		cv::Mat epiB = (cv::Mat_<f64>(3, 1) << 0, 0, 0);
		cv::Mat epiK = (cv::Mat_<f64>(3, 1) << 0, 0, 0);
		Common::Math::Vec3 vec = { {368,265,1} };
		//Common::AlgorithmCV::cmIdealEpipolarEquationByFundamentalMatrix(&camIntb,&camExtb,&camExtc,&vec,&epiB,&epiK);
		Common::AlgorithmCV::cmIdealEpipolarEquation(&camIntb, &camIntc, &camExtb, &camExtc, &vec, &epiB, &epiK);
		cout << endl << "B:";
		for (i32 i = 0; i < 3; i++) {
			epiB.at<f64>(i, 0) /= epiB.at<f64>(2, 0);
			cout << epiB.at<f64>(i, 0) << ",";
		}
		cout << endl << "K:";
		for (i32 i = 0; i < 3; i++) {
			epiK.at<f64>(i, 0) /= epiK.at<f64>(2, 0);
			cout << epiK.at<f64>(i, 0) << ",";
		}
		f64 dist = sqrt(epiK.at<f64>(0, 0) * epiK.at<f64>(0, 0) + epiK.at<f64>(1, 0) * epiK.at<f64>(1, 0));
		epiK.at<f64>(0, 0) /= dist;
		epiK.at<f64>(1, 0) /= dist;
		cout << endl;
		for (i32 i = 0; i < 50; i++) {
			cv::Mat pta = epiB + epiK * i * 20;
			cv::Mat ptb = epiB - epiK * i * 20;
			cout << "draw" << pta.at<f64>(0, 0) << "," << pta.at<f64>(1, 0) << "|" << ptb.at<f64>(0, 0) << "," << ptb.at<f64>(1, 0) << endl;
			if (pta.at<f64>(0, 0) < camItr.cols && pta.at<f64>(0, 0) > 0 && pta.at<f64>(1, 0) < camItr.rows && pta.at<f64>(1, 0) > 0) {
				cv::circle(camItr, cv::Point(pta.at<f64>(0, 0), pta.at<f64>(1, 0)), 3, cv::Scalar(255, 0, 0));
			}
			if (ptb.at<f64>(0, 0) < camItr.cols && ptb.at<f64>(0, 0) > 0 && ptb.at<f64>(1, 0) < camItr.rows && ptb.at<f64>(1, 0) > 0) {
				cv::circle(camItr, cv::Point(ptb.at<f64>(0, 0), ptb.at<f64>(1, 0)), 3, cv::Scalar(255, 0, 0));
			}
		}
		cv::circle(camItl, cv::Point(vec.a[0], vec.a[1]), 3, cv::Scalar(255, 0, 0), -1);
		cv::imshow("ITL", camItl);
		cv::imshow("ITR", camItr);
		cv::waitKey(0);
    }

	void Test::binocularSGM() {
		using namespace std;
		cout << "Starting" << endl;
		StereoRectification::StereoRectification* srect = new StereoRectification::StereoRectification();
		//Create Objects
		SemiGlobalMatching::DisparityHelper* disparityHelper = new SemiGlobalMatching::DisparityHelper();
		SemiGlobalMatching::CostOptimizer* discretizator = new SemiGlobalMatching::CostOptimizer();
		SemiGlobalMatching::DepthConverter depthConverter = SemiGlobalMatching::DepthConverter();
		DenseReconstruction::TruncatedSDF tsdfCalc = DenseReconstruction::TruncatedSDF();
		Common::Util::VisualizationExt visExt = Common::Util::VisualizationExt();

		//Load Images
		cout << "Loading Image" << endl;
		cv::Mat imageLeft = cv::imread("C:/WR/Dense-Reconstruction/samples/vl.png", 0);
		cv::Mat imageRight = cv::imread("C:/WR/Dense-Reconstruction/samples/vr.png", 0);


		u32 imageWidth = imageLeft.size[1];
		u32 imageHeight = imageLeft.size[0];
		u32 disparityRange = 64;
		cout << "IW=" << imageWidth << ", IH=" << imageHeight << endl;

		//Disparity Estimate
		cout << "Estimating Disparity" << endl;
		f64* leftDisparityMap = allocate_mem(f64, (usize)imageWidth * imageHeight);
		disparityHelper->smIdealBinocularDisparity(imageLeft.data, imageRight.data, imageWidth, imageHeight, disparityRange, leftDisparityMap);

		cout << "Discretization" << endl;
		u32* leftDisparityMapMfds = allocate_mem(u32, (usize)imageWidth * imageHeight);
		discretizator->smDisparityMapDiscretization(leftDisparityMap, leftDisparityMapMfds, imageWidth, imageHeight, disparityRange, 0);

		cout << "Saving PPM" << endl;
		Common::Algorithm::cmSaveAsPPM32("C:/WR/Dense-Reconstruction/samples/vs1-cb-3da.ppm", leftDisparityMapMfds, imageWidth, imageHeight, disparityRange);
	}
	void Test::sgmMarchingCubeSurface() {
		using namespace std;
		cout << "Starting" << endl;
		StereoRectification::StereoRectification* srect = new StereoRectification::StereoRectification();
		//Create Objects
		SemiGlobalMatching::DisparityHelper* disparityHelper = new SemiGlobalMatching::DisparityHelper();
		SemiGlobalMatching::CostOptimizer* discretizator = new SemiGlobalMatching::CostOptimizer();
		SemiGlobalMatching::DepthConverter depthConverter = SemiGlobalMatching::DepthConverter();
		DenseReconstruction::TruncatedSDF tsdfCalc = DenseReconstruction::TruncatedSDF();
		Common::Util::VisualizationExt visExt = Common::Util::VisualizationExt();

		//Load Images
		cout << "Loading Image" << endl;
		cv::Mat imageLeft = cv::imread("C:/WR/Dense-Reconstruction/samples/vl.png", 0);
		cv::Mat imageRight = cv::imread("C:/WR/Dense-Reconstruction/samples/vr.png", 0);


		u32 imageWidth = imageLeft.size[1];
		u32 imageHeight = imageLeft.size[0];
		u32 disparityRange = 64;
		cout << "IW=" << imageWidth << ", IH=" << imageHeight << endl;

		//Disparity Estimate
		cout << "Estimating Disparity" << endl;
		f64* leftDisparityMap = allocate_mem(f64, (usize)imageWidth * imageHeight);
		disparityHelper->smIdealBinocularDisparity(imageLeft.data, imageRight.data, imageWidth, imageHeight, disparityRange, leftDisparityMap);

		cout << "Discretization" << endl;
		u32* leftDisparityMapMfds = allocate_mem(u32, (usize)imageWidth * imageHeight);
		discretizator->smDisparityMapDiscretization(leftDisparityMap, leftDisparityMapMfds, imageWidth, imageHeight, disparityRange, 0);

		cout << "Saving PPM" << endl;
		Common::Algorithm::cmSaveAsPPM32("C:/WR/Dense-Reconstruction/samples/vs1-cb-3da.ppm", leftDisparityMapMfds, imageWidth, imageHeight, disparityRange);
		cout << "Depth Estimation" << endl;
		f64* depthMap = allocate_mem(f64, (usize)imageWidth * imageHeight);
		depthConverter.smIdealBinocularDisparityToDepth(leftDisparityMap, depthMap, imageWidth, imageHeight, 20.0, 60.0);


		//Discretization
		cout << "Discretization" << endl;
		u32* depthMapTrunc = allocate_mem(u32, (usize)imageWidth * imageHeight);
		u32 depthMapTruncMax = 0;
		depthConverter.smDepthDiscretization(depthMap, depthMapTrunc, &depthMapTruncMax, imageWidth, imageHeight);

		//=========== End of Depth Calculation ==================

		cout << "Initializing Camera Intrinsic" << endl;
		Common::Camera::MonocularCameraIntrinsic* cameraIntrinsic = new Common::Camera::MonocularCameraIntrinsic();
		cameraIntrinsic->fx = 450.0;
		cameraIntrinsic->fy = 375.0;
		cameraIntrinsic->cx = 450.0 / 2.0;
		cameraIntrinsic->cy = 375.0 / 2.0;
		cameraIntrinsic->dx = 1.0;
		cameraIntrinsic->dy = 1.0;

		cout << "Creating Voxels" << endl;
		DenseReconstruction::VoxelStore* voxelStore = new DenseReconstruction::PlainVoxelStore();
		voxelStore->drInitialize(128, 128, 128, -64, -64, -64, 1.0);
		DenseReconstruction::VoxelStore* voxelStoreTemp = new DenseReconstruction::PlainVoxelStore();
		voxelStoreTemp->drInitialize(128, 128, 128, -64, -64, -64, 1.0);

		cout << "Calculating Truncated Signed Distance Field" << endl;
		tsdfCalc.drIdealFirstTsdfEstimate(depthMap, imageWidth, imageHeight, cameraIntrinsic, voxelStore, 10.0);

		cout << "Converting TSDF" << endl;
		visExt.cmuTsdfBinarization(voxelStore, voxelStoreTemp);

		cout << "Converting to Mesh" << endl;
		Common::Mesh::SimpleMesh* mesh = new Common::Mesh::SimpleMesh();
		visExt.cmuVoxelMarchingCubes(voxelStore, mesh);


		cout << "F=" << mesh->f.size() << ", V=" << mesh->v.size() << endl;
		cout << "Saving As OBJ" << endl;
		visExt.cmuExportMeshToObj("C:/WR/Sayu/samples/3d-3c.obj", mesh);

		//Save PPM
		cout << "Saving PPM" << endl;
		Common::Algorithm::cmSaveAsPPM32("C:/WR/Dense-Reconstruction/samples/vs1-cb-3c.ppm", depthMapTrunc, imageWidth, imageHeight, depthMapTruncMax);
	}
}