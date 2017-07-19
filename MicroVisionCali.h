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
	 *	���̽ǵ����� �ṹ��
	 */
	struct CornerData
	{
		int			nPoints;			// ���̽ǵ�����
		int			nImages;			// ����ͼ����
		int			nPointsPerImage;	// ÿ�����̵Ľǵ���
		Size		imageSize;			// ͼ��ֱ���
		Size		boardSize;			// ���̳ߴ�
		vector<vector<Point3f> >	objectPoints;	// ���̽ǵ�������������
		vector<vector<Point2f> >	imagePoints1;	// ��ͼ1�����̽ǵ�������������
		vector<vector<Point2f> >	imagePoints2;	// ��ͼ2�����̽ǵ�������������
		vector<vector<Point2f> >	imagePoints3;	// ��ͼ3�����̽ǵ�������������
	};

	/***
	 *	��Ŀ�궨���������
	 */
	struct CameraParams
	{
		Size		imageSize;				// ͼ��ֱ���
		Mat			cameraMatrix;			// ���������
		Mat			distortionCoefficients;	// ������������
		vector<Mat> rotations;				// ����ͼƬ����ת����
		vector<Mat> translations;			// ����ͼƬ��ƽ������
		int			flags;					// ��Ŀ�궨���õı�־λ
	};

	/***
	 *	˫Ŀ�궨���������
	 */
	struct StereoParams
	{
		Size			imageSize;		// ͼ��ֱ���
		CameraParams	cameraParams1;	// ��������궨����
		CameraParams	cameraParams2;	// ��������궨����
		Mat				rotation;		// ��ת����
		Mat				translation;	// ƽ������
		Mat				essential;		// ���ʾ���
		Mat				foundational;	// ��������
		int				flags;			// ˫Ŀ�궨���õı�־λ
        double          alpha;          // ˫ĿУ��Ч��������ϵ����ȡֵ 0~1 �� -1
	};

	/***
	 *	˫ĿУ�����������
	 */
	struct  RemapMatrixs
	{
		cv::Mat		mX1;	// ����ͼ X ��������ӳ�����
		cv::Mat		mY1;	// ����ͼ Y ��������ӳ�����
		cv::Mat		mX2;	// ����ͼ X ��������ӳ�����
		cv::Mat		mY2;	// ����ͼ Y ��������ӳ�����
		cv::Mat		Q;		// ���ڼ�����ά���Ƶ� Q ����
		cv::Rect	roi1;	// ����ͼ��Ч����ľ���
		cv::Rect	roi2;	// ����ͼ��Ч����ľ���
	};

	/***
	 *	˫ĿУ������
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

