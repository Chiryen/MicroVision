#include "stdafx.h"
#include "MicroVisionCali.h"
#include "conio.h"

using namespace std;

MicroVisionCali::MicroVisionCali(void)
{
}


MicroVisionCali::~MicroVisionCali(void)
{
}

void MicroVisionCali::initCornerData(int nImage, Size imageSize, Size boardSize, float squareWidth, CornerData& cornerData)
{
	cornerData.nImages = nImage;
	cornerData.imageSize = imageSize;
	cornerData.boardSize = boardSize;
	cornerData.nPoints = nImage * boardSize.width * boardSize.height;
	cornerData.nPointsPerImage = boardSize.width * boardSize.height;
	cornerData.objectPoints.resize(nImage, vector<Point3f>(cornerData.nPointsPerImage, Point3f(0,0,0)));
	cornerData.imagePoints1.resize(nImage, vector<Point2f>(cornerData.nPointsPerImage, Point2f(0,0)));
	cornerData.imagePoints2.resize(nImage, vector<Point2f>(cornerData.nPointsPerImage, Point2f(0,0)));
	cornerData.imagePoints3.resize(nImage, vector<Point2f>(cornerData.nPointsPerImage, Point2f(0,0)));

	//计算棋盘角点的世界坐标值
	int i,j,k,n;
	for( i = 0; i < nImage; i++ )
	{
		// objectPoints的每一行为一副棋盘格的数据
		n = 0;
		for( j = 0; j < boardSize.height; j++ )
			for( k = 0; k < boardSize.width; k++ )
				cornerData.objectPoints[i][n++] = cv::Point3f(j*squareWidth, k*squareWidth, 0);
	}
}

int MicroVisionCali::detectCorners(Mat& img1, Mat& img2, CornerData& cornerData, int imageCount)
{

	// 提取当前棋盘对应的角点坐标子矩阵
	vector<Point2f>& corners1 = cornerData.imagePoints1[imageCount];
	vector<Point2f>& corners2 = cornerData.imagePoints2[imageCount];

	// 寻找棋盘及其角点
	bool found1 = false;
	bool found2 = false;
	//	int flags = CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK;
	int flags = CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_FILTER_QUADS ;

	found1 = findChessboardCorners(img1, cornerData.boardSize, corners1, flags);
	found2 = findChessboardCorners(img2, cornerData.boardSize, corners2, flags);

	// 若左右视图都成功检测到所有角点
	// 则将检测到的角点坐标精确化
	if ( found1 && found2 )
	{
		//转换为灰度图
		Mat gray1, gray2;
		cvtColor(img1, gray1, cv::COLOR_RGB2GRAY);
		cvtColor(img2, gray2, cv::COLOR_RGB2GRAY);

		//计算角点的精确坐标
		cv::Size regionSize(3, 3);
		cornerSubPix(gray1, corners1, regionSize, cv::Size(-1,-1), cv::TermCriteria(cv::TermCriteria::EPS|cv::TermCriteria::MAX_ITER, 30, 0.05));
		cornerSubPix(gray2, corners2, regionSize, cv::Size(-1,-1), cv::TermCriteria(cv::TermCriteria::EPS|cv::TermCriteria::MAX_ITER, 30, 0.05));
	}

	// 显示检测到的角点
	drawChessboardCorners(img1, cornerData.boardSize, corners1, found1);
	drawChessboardCorners(img2, cornerData.boardSize, corners2, found2);

	// 显示当前good_board的数目
	char info[10];
	sprintf_s(info, "%02d/%02d", imageCount+1, cornerData.nImages);
	string text = info;
	showText(img1, text);
	showText(img2, text);

	return (found1 && found2) ? 1 : 0;
}

void MicroVisionCali::showText(Mat& img, string text)
{
	int fontFace = cv::FONT_HERSHEY_PLAIN;
	double fontScale = 5;
	int fontThickness = 5;

	// get text size
	int textBaseline=0;
	cv::Size textSize = cv::getTextSize(text, fontFace,
		fontScale, fontThickness, &textBaseline);
	textBaseline += fontThickness;

	// put the text at lower right corner
	cv::Point textOrg((img.cols - textSize.width - 10),
		(img.rows - textSize.height - 10));

	// then put the text itself
	putText(img, text, textOrg, fontFace, fontScale,
		cv::Scalar(0,0,255), fontThickness, 8);
	
}

int MicroVisionCali::saveCornerData(const char* filename, const CornerData& cornerData)
{
	cv::FileStorage fs(filename, cv::FileStorage::WRITE);
	if (fs.isOpened())
	{
		time_t rawtime; 
		time(&rawtime);
		fs << "calibrationDate" << asctime(localtime(&rawtime));

		fs << "nPoints"			<< cornerData.nPoints;
		fs << "nImages"			<< cornerData.nImages;
		fs << "nPointsPerImage" << cornerData.nPointsPerImage;

		fs << "imageSize" << "[" << cornerData.imageSize.width << cornerData.imageSize.height << "]";

		fs << "boardSize" << "[" << cornerData.boardSize.width << cornerData.boardSize.height << "]";

		for (int i=0; i<cornerData.nImages;i++) 
		{
			std::stringstream imagename; 
			imagename << "image" << i;

			fs << imagename.str() << "{";

			for (int j=0; j<cornerData.nPointsPerImage; j++) 
			{
				std::stringstream nodename; 
				nodename << "node" << j;

				fs << nodename.str() << "{";

				cv::Point3f op = cornerData.objectPoints[i][j];
				cv::Point2f ip1 = cornerData.imagePoints1[i][j];
				cv::Point2f ip2 = cornerData.imagePoints2[i][j];

				fs << "objectPoints" << "[:"; 
				fs << op.x << op.y << op.z << "]";

				fs << "imagePoints1" << "[:"; 
				fs << ip1.x << ip1.y << "]";

				fs << "imagePoints2" << "[:"; 
				fs << ip2.x << ip2.y << "]";

				fs << "}";
			}

			fs << "}";
		}

		fs.release();
		return 1;
	} 
	else
	{
		return 0;
	}
}

int MicroVisionCali::loadCornerData(const char* filename, CornerData& cornerDatas)
{
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	if (fs.isOpened())
	{		
		fs["nPoints"]			>> cornerDatas.nPoints;
		fs["nImages"]			>> cornerDatas.nImages;
		fs["nPointsPerImage"]	>> cornerDatas.nPointsPerImage;

		cv::FileNodeIterator it = fs["imageSize"].begin(); 
		it >> cornerDatas.imageSize.width >> cornerDatas.imageSize.height;

		cv::FileNodeIterator bt = fs["boardSize"].begin(); 
		bt >> cornerDatas.boardSize.width >> cornerDatas.boardSize.height;

		for (int i=0; i<cornerDatas.nImages;i++) 
		{
			std::stringstream imagename; 
			imagename << "image" << i;

			cv::FileNode img = fs[imagename.str()];
			vector<cv::Point3f> ov;
			vector<cv::Point2f> iv1, iv2;
			for (int j=0; j<cornerDatas.nPointsPerImage; j++) 
			{
				std::stringstream nodename; 
				nodename << "node" << j;

				cv::FileNode pnt = img[nodename.str()];
				cv::Point3f op;
				cv::Point2f ip1, ip2;
				cv::FileNodeIterator ot = pnt["objectPoints"].begin(); 
				ot >> op.x >> op.y >> op.z;
				cv::FileNodeIterator it1 = pnt["imagePoints1"].begin(); 
				it1 >> ip1.x >> ip1.y;
				cv::FileNodeIterator it2 = pnt["imagePoints2"].begin(); 
				it2 >> ip2.x >> ip2.y;

				iv1.push_back(ip1);
				iv2.push_back(ip2);
				ov.push_back(op);
			}
			cornerDatas.imagePoints1.push_back(iv1);
			cornerDatas.imagePoints2.push_back(iv2);
			cornerDatas.objectPoints.push_back(ov);
		}

		fs.release();
		return 1;
	} 
	else
	{
		return 0;
	}
}

int MicroVisionCali::calibrateStereoCamera(CornerData& cornerData, StereoParams& stereoParams)
{

	/***
		*	执行单目定标
		*/
	stereoParams.cameraParams1.cameraMatrix = Mat::eye(3, 3, CV_32FC1);
	
	calibrateCamera(
		cornerData.objectPoints, 
		cornerData.imagePoints1, 
		cornerData.imageSize, 
		stereoParams.cameraParams1.cameraMatrix, 
		stereoParams.cameraParams1.distortionCoefficients,
		stereoParams.cameraParams1.rotations, 
		stereoParams.cameraParams1.translations,
		stereoParams.cameraParams1.flags
		);

	calibrateCamera(
		cornerData.objectPoints, 
		cornerData.imagePoints2, 
		cornerData.imageSize, 
		stereoParams.cameraParams2.cameraMatrix, 
		stereoParams.cameraParams2.distortionCoefficients,
		stereoParams.cameraParams2.rotations, 
		stereoParams.cameraParams2.translations,
		stereoParams.cameraParams2.flags
		);

	/***
		*	保存单目定标结果至本地
		*/
	saveCameraParams(stereoParams.cameraParams1, (m_workDir + "cameraParams_one.yml").c_str());
	saveCameraParams(stereoParams.cameraParams2, (m_workDir + "cameraParams_two.yml").c_str());

	stereoParams.imageSize = cornerData.imageSize;

	stereoCalibrate(
		cornerData.objectPoints,
		cornerData.imagePoints1,
		cornerData.imagePoints2,
		stereoParams.cameraParams1.cameraMatrix,
		stereoParams.cameraParams1.distortionCoefficients,
		stereoParams.cameraParams2.cameraMatrix,
		stereoParams.cameraParams2.distortionCoefficients,
		stereoParams.imageSize,
		stereoParams.rotation,
		stereoParams.translation,
		stereoParams.essential,
		stereoParams.foundational,
		TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 30, 1e-6),
		stereoParams.flags + CALIB_FIX_K3 + CALIB_FIX_K4 + CALIB_FIX_K5
	);

	return 1;
}

//保存相机内参数数据
int MicroVisionCali::saveCameraParams(const CameraParams& cameraParams, const char* filename)
{
	std::string filename_ = filename;

	//按当前时间生成文件名 
	if (filename_ == "")
	{
		int strLen = 20;
		char *pCurrTime = (char*)malloc(sizeof(char)*strLen);
		memset(pCurrTime, 0, sizeof(char)*strLen);
		time_t now;
		time(&now);
		strftime(pCurrTime, strLen , "%Y_%m_%d_%H_%M_%S_", localtime(&now));

		filename_ =  pCurrTime;
		filename_ += "cameraParams.yml";
	}

	//写入数据
	FileStorage fs(filename_.c_str(), FileStorage::WRITE);
	if (fs.isOpened())
	{
		time_t rawtime; 
		time(&rawtime);
		fs << "calibrationDate" << asctime(localtime(&rawtime));

		char flagText[1024];
		sprintf_s( flagText, "flags: %s%s%s%s%s",
			cameraParams.flags & CALIB_FIX_K3 ? "fix_k3" : "",
			cameraParams.flags & CALIB_USE_INTRINSIC_GUESS ? " + use_intrinsic_guess" : "",
			cameraParams.flags & CALIB_FIX_ASPECT_RATIO ? " + fix_aspect_ratio" : "",
			cameraParams.flags & CALIB_FIX_PRINCIPAL_POINT ? " + fix_principal_point" : "",
			cameraParams.flags & CALIB_ZERO_TANGENT_DIST ? " + zero_tangent_dist" : "" );
		cvWriteComment(*fs, flagText, 0);
		
		fs << "flags"					<< cameraParams.flags;

		fs << "imageSize" << "[" << cameraParams.imageSize.width << cameraParams.imageSize.height << "]";
		
		fs << "cameraMatrix"			<< cameraParams.cameraMatrix;
		fs << "distortionCoefficients"	<< cameraParams.distortionCoefficients;

		int nImages = cameraParams.rotations.size();
		fs << "nImages"	<< nImages;
		for (UINT i = 0; i < nImages; i++)
		{
			char matName[50];
			sprintf_s(matName, "rotaionMatrix_%d", i);

			fs << matName << cameraParams.rotations[i];
		}
		for (UINT i = 0; i < nImages; i++)
		{
			char matName[50];
			sprintf_s(matName, "translationMatrix_%d", i);

			fs << matName << cameraParams.translations[i];
		}

		fs.release();
		return 1;
	} 
	else
	{
		return 0;
	}
}


void MicroVisionCali::setWorkDir(const char* workDir)
{
	m_workDir = workDir;
}


int MicroVisionCali::getStereoCalibrateError(CornerData& cornerData, StereoParams& stereoParams, double& err)
{
	// 利用对极线约束检查立体校正效果
	// because the output fundamental matrix implicitly
	// includes all the output information,
	// we can check the quality of calibration using the
	// epipolar geometry constraint: m2^t*F*m1=0
	
	vector<cv::Vec3f> epilines[2];
	vector<vector<cv::Point2f> > imagePoints[2];
	cv::Mat cameraMatrix[2], distCoeffs[2];
	int npoints = 0;
	int i,j,k;

	imagePoints[0] = cornerData.imagePoints1;
	imagePoints[1] = cornerData.imagePoints2;
	cameraMatrix[0] = stereoParams.cameraParams1.cameraMatrix;
	cameraMatrix[1] = stereoParams.cameraParams2.cameraMatrix;
	distCoeffs[0] = stereoParams.cameraParams1.distortionCoefficients;
	distCoeffs[1] = stereoParams.cameraParams2.distortionCoefficients;

	for( i = 0; i < cornerData.nImages; i++ )
	{
		int npt = (int)imagePoints[0][i].size();
		cv::Mat imgpt[2];

		for( k = 0; k < 2; k++ )
		{
			imgpt[k] = cv::Mat(imagePoints[k][i]);
			// 计算校正后的棋盘角点坐标
			undistortPoints(imgpt[k], imgpt[k], cameraMatrix[k], distCoeffs[k], cv::Mat(), cameraMatrix[k]);
			// 计算对极线
			computeCorrespondEpilines(imgpt[k], k+1, stereoParams.foundational, epilines[k]);
		}

		// 计算对极线误差
		for( j = 0; j < npt; j++ )
		{
			double errij = 
				fabs(imagePoints[0][i][j].x * epilines[1][j][0] +
					 imagePoints[0][i][j].y * epilines[1][j][1] + epilines[1][j][2]) +
				fabs(imagePoints[1][i][j].x * epilines[0][j][0] +
					 imagePoints[1][i][j].y * epilines[0][j][1] + epilines[0][j][2]);
			err += errij;
		}
		npoints += npt;
	}
	err /= npoints;

	return 1;
}


int MicroVisionCali::rectifyStereoCamera(CornerData& cornerData, StereoParams& stereoParams, RemapMatrixs& remapMatrixs, RECTIFYMETHOD method)
{
	//初始化
	remapMatrixs.mX1 = Mat(stereoParams.imageSize, CV_32FC1);
	remapMatrixs.mY1 = Mat(stereoParams.imageSize, CV_32FC1);
	remapMatrixs.mX2 = Mat(stereoParams.imageSize, CV_32FC1);
	remapMatrixs.mY2 = Mat(stereoParams.imageSize, CV_32FC1);

	Mat R1, R2, P1, P2, Q;
	Rect roi1, roi2;

	stereoParams.alpha = -1;
    /*if (stereoParams.alpha < 0 || stereoParams.alpha > 1)
        stereoParams.alpha = -1;*/

	//执行双目校正
	stereoRectify(
		stereoParams.cameraParams1.cameraMatrix,
		stereoParams.cameraParams1.distortionCoefficients,
		stereoParams.cameraParams2.cameraMatrix,
		stereoParams.cameraParams2.distortionCoefficients,
		stereoParams.imageSize,
		stereoParams.rotation,
		stereoParams.translation,
		R1,R2, P1, P2, Q, 
		cv::CALIB_ZERO_DISPARITY,
		stereoParams.alpha, 
		stereoParams.imageSize,
		&roi1, &roi2);

	//使用HARTLEY方法的额外处理
	if (method == RECTIFY_HARTLEY)
	{
		cv::Mat F, H1, H2;
		F = findFundamentalMat(
			cornerData.imagePoints1,
			cornerData.imagePoints2,
			cv::FM_8POINT, 0, 0);
		stereoRectifyUncalibrated(
			cornerData.imagePoints1,
			cornerData.imagePoints2,
			F, stereoParams.imageSize, H1, H2, 3);

		R1 = stereoParams.cameraParams1.cameraMatrix.inv() * H1 * stereoParams.cameraParams1.cameraMatrix;
		R2 = stereoParams.cameraParams2.cameraMatrix.inv() * H2 * stereoParams.cameraParams2.cameraMatrix;
		P1 = stereoParams.cameraParams1.cameraMatrix;
		P2 = stereoParams.cameraParams2.cameraMatrix;
	}

	//生成图像校正所需的像素映射矩阵
	initUndistortRectifyMap(
		stereoParams.cameraParams1.cameraMatrix,
		stereoParams.cameraParams1.distortionCoefficients,
		R1, P1, 
		stereoParams.imageSize,
		CV_16SC2,
		remapMatrixs.mX1, remapMatrixs.mY1);

	initUndistortRectifyMap(
		stereoParams.cameraParams2.cameraMatrix,
		stereoParams.cameraParams2.distortionCoefficients,
		R2, P2, 
		stereoParams.imageSize,
		CV_16SC2,
		remapMatrixs.mX2, remapMatrixs.mY2);

	//输出数据
	Q.copyTo(remapMatrixs.Q);
	remapMatrixs.roi1 = roi1;
	remapMatrixs.roi2 = roi2;

	return 1;
}


int MicroVisionCali::saveCalibrationDatas(const char* filename, RECTIFYMETHOD method, CornerData& cornerData, StereoParams& stereoParams, RemapMatrixs& remapMatrixs)
{
	
	FileStorage fs(filename, FileStorage::WRITE);

	if (fs.isOpened())
	{
		time_t rawtime;
		time(&rawtime);
		fs << "calibrationDate" << asctime(localtime(&rawtime));

		fs << "num_boards"	<< cornerData.nImages;
		fs << "imageSize" << "[" << cornerData.imageSize.width << cornerData.imageSize.height << "]";

		char flagText[1024];
		sprintf_s( flagText, "flags: %s%s%s%s%s",
			stereoParams.flags & CALIB_USE_INTRINSIC_GUESS ? "+ use_intrinsic_guess" : "",
			stereoParams.flags & CALIB_FIX_ASPECT_RATIO ? " + fix_aspect_ratio" : "",
			stereoParams.flags & CALIB_FIX_PRINCIPAL_POINT ? " + fix_principal_point" : "",
			stereoParams.flags & CALIB_FIX_INTRINSIC ? " + fix_intrinsic" : "",
			stereoParams.flags & CALIB_SAME_FOCAL_LENGTH ? " + same_focal_length" : "" );

		cvWriteComment(*fs, flagText, 0);

		fs << "stereoCalibrateFlags"		<< stereoParams.flags;
		fs << "leftCameraMatrix"			<< stereoParams.cameraParams1.cameraMatrix;
		fs << "leftDistortCoefficients"		<< stereoParams.cameraParams1.distortionCoefficients;
		fs << "rightCameraMatrix"			<< stereoParams.cameraParams2.cameraMatrix;
		fs << "rightDistortCoefficients"	<< stereoParams.cameraParams2.distortionCoefficients;
		fs << "rotationMatrix"				<< stereoParams.rotation;
		fs << "translationVector"			<< stereoParams.translation;
		fs << "foundationalMatrix"			<< stereoParams.foundational;

		if (method == RECTIFY_BOUGUET)
		{
			fs << "rectifyMethod" << "BOUGUET";
			fs << "leftValidArea" << "[:"
				<< remapMatrixs.roi1.x << remapMatrixs.roi1.y
				<< remapMatrixs.roi1.width << remapMatrixs.roi1.height << "]";
			fs << "rightValidArea" << "[:"
				<< remapMatrixs.roi2.x << remapMatrixs.roi2.y
				<< remapMatrixs.roi2.width << remapMatrixs.roi2.height << "]";
			fs << "QMatrix" << remapMatrixs.Q;
		}
		else
			fs << "rectifyMethod" << "HARTLEY";

		fs << "remapX1" << remapMatrixs.mX1;
		fs << "remapY1" << remapMatrixs.mY1;
		fs << "remapX2" << remapMatrixs.mX2;
		fs << "remapY2" << remapMatrixs.mY2;

		fs.release();
		return 1;
	} 
	else
	{
		return 0;
	}
}


int MicroVisionCali::remapImage(Mat& img1, Mat& img2, Mat& img1r, Mat& img2r, RemapMatrixs& remapMatrixs)
{
	if ( !remapMatrixs.mX1.empty() && !remapMatrixs.mY1.empty() )
	{
		remap( img1, img1r, remapMatrixs.mX1, remapMatrixs.mY1, INTER_LINEAR );
	}
	else
		return -1;
	if ( !remapMatrixs.mX2.empty() && !remapMatrixs.mY2.empty() )
	{
		remap( img2, img2r, remapMatrixs.mX2, remapMatrixs.mY2, INTER_LINEAR );
	}
	else
		return -2;

	return 1;
}
