#pragma once

#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/core/core_c.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define CAM_MAX 3

class MicroVisionCali
{
public:
	MicroVisionCali(void);
	~MicroVisionCali(void);

	/***
	 *	棋盘角点数据 结构体
	 */
	struct CornerData
	{
		int			nPoints;			// 棋盘角点总数
		int			nImages;			// 棋盘图像数
		int			nPointsPerImage;	// 每幅棋盘的角点数
		Size		imageSize;			// 图像分辨率
		Size		boardSize;			// 棋盘尺寸
		vector<vector<Point3f> >	objectPoints;	// 棋盘角点世界坐标序列
		vector<vector<Point2f> >	imagePoints1;	// 视图1的棋盘角点像素坐标序列
		vector<vector<Point2f> >	imagePoints2;	// 视图2的棋盘角点像素坐标序列
		vector<vector<Point2f> >	imagePoints3;	// 视图3的棋盘角点像素坐标序列
	};

	/***
	 *	单目标定的输出参数
	 */
	struct CameraParams
	{
		Size		imageSize;				// 图像分辨率
		Mat			cameraMatrix;			// 摄像机矩阵
		Mat			distortionCoefficients;	// 摄像机畸变参数
		vector<Mat> rotations;				// 棋盘图片的旋转矩阵
		vector<Mat> translations;			// 棋盘图片的平移向量
		int			flags;					// 单目标定所用的标志位
	};

	/***
	 *	双目标定的输出参数
	 */
	struct StereoParams
	{
		Size			imageSize;		// 图像分辨率
		CameraParams	cameraParams1;	// 左摄像机标定参数
		CameraParams	cameraParams2;	// 右摄像机标定参数
		Mat				rotation;		// 旋转矩阵
		Mat				translation;	// 平移向量
		Mat				essential;		// 本质矩阵
		Mat				foundational;	// 基础矩阵
		int				flags;			// 双目标定所用的标志位
        double          alpha;          // 双目校正效果的缩放系数，取值 0~1 或 -1
	};

	/***
	 *	双目校正的输出参数
	 */
	struct  RemapMatrixs
	{
		cv::Mat		mX1;	// 左视图 X 方向像素映射矩阵
		cv::Mat		mY1;	// 左视图 Y 方向像素映射矩阵
		cv::Mat		mX2;	// 右视图 X 方向像素映射矩阵
		cv::Mat		mY2;	// 右视图 Y 方向像素映射矩阵
		cv::Mat		Q;		// 用于计算三维点云的 Q 矩阵
		cv::Rect	roi1;	// 左视图有效区域的矩形
		cv::Rect	roi2;	// 右视图有效区域的矩形
	};

	/***
	 *	双目校正方法
	 */
	enum RECTIFYMETHOD { RECTIFY_BOUGUET, RECTIFY_HARTLEY };


	void initCornerData(int nImage, Size imageSize, Size boardSize, float squareWidth, CornerData& cornerData);
	int detectCorners(Mat& img1, Mat& img2, CornerData& cornerData, int imageCount);
	void showText(Mat& img, string text);
	int saveCornerData(const char* filename, const CornerData& cornerData);
	int calibrateStereoCamera(CornerData& cornerData, StereoParams& stereoParams);
	int saveCameraParams(const CameraParams& cameraParams, const char* filename);
	int loadCornerData(const char* filename, CornerData& cornerDatas);
	void setWorkDir(const char* workDir);
	int getStereoCalibrateError(CornerData& cornerData, StereoParams& stereoParams, double& err);
	int rectifyStereoCamera(CornerData& cornerData, StereoParams& stereoParams, RemapMatrixs& remapMatrixs, RECTIFYMETHOD method);
	int saveCalibrationDatas(const char* filename, RECTIFYMETHOD method, CornerData& cornerData, StereoParams& stereoParams, RemapMatrixs& remapMatrixs);
	int remapImage(Mat& img1, Mat& img2, Mat& img1r, Mat& img2r, RemapMatrixs& remapMatrixs);

private:
	string m_workDir;
};

