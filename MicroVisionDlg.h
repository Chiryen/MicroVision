
// MicroVisionDlg.h : 头文件
//

#pragma once

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <MVGigE.h>
#include <MVImage.h>
#include <map>
#include "afxwin.h"
#include "MicroVisionCali.h"

using namespace std;
using namespace cv;

#define CAM_MAX 3

// CMicroVisionDlg 对话框
class CMicroVisionDlg : public CDialogEx
{
// 构造
public:
	CMicroVisionDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MICROVISION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP();

private:
	HANDLE m_hCam[CAM_MAX];
	MVImage m_image[CAM_MAX];
	MV_PixelFormatEnums m_PixelFormat[CAM_MAX];
	int m_nCams;
	MVCamInfo m_camInfo[CAM_MAX];
	std::map<CString, int> caminfo_idx;
	int m_imgWidth;
	int m_imgHeight;
	int m_boardWidth;
	int m_boardHeight;
	int m_squareSize;
	int m_boardCount;
	Mat img[CAM_MAX];
	CString m_workDir;
	MicroVisionCali m_stereoCali;
public:
	afx_msg void OnBnClickedBtnScancamera();
	CComboBox m_ComboOneList;
	CComboBox m_ComboTwoList;
	CComboBox m_ComboThreeList;
	afx_msg void OnCbnSelchangeComboCameraone();
	afx_msg void OnBnClickedBtnStartcamera();

	int OnStreamCB0( MV_IMAGE_INFO *pInfo);
	int OnStreamCB1( MV_IMAGE_INFO *pInfo);

	void ImgRead( MV_IMAGE_INFO *pInfo, int idx );
	void F_ShowImage(Mat img, UINT ID);
	CString F_InitWorkDir();
	afx_msg void OnBnClickedBtnStopcamera();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnDocalibration();
	afx_msg void OnBnClickedBtnChessdefault();
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnStopcalibration();
	afx_msg void OnBnClickedBtnPositioning();
	afx_msg void OnBnClickedBtnRebuild();
	afx_msg void OnBnClickedBtnRecogntion();
};
