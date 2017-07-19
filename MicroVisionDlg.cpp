
// MicroVisionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MicroVision.h"
#include "MicroVisionDlg.h"
#include "afxdialogex.h"
#include "conio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMicroVisionDlg 对话框



CMicroVisionDlg::CMicroVisionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMicroVisionDlg::IDD, pParent)
	, m_boardWidth(0)
	, m_boardHeight(0)
	, m_squareSize(0)
	, m_boardCount(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMicroVisionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CameraOne, m_ComboOneList);
	DDX_Control(pDX, IDC_COMBO_CameraTwo, m_ComboTwoList);
	DDX_Control(pDX, IDC_COMBO_CameraThree, m_ComboThreeList);
	DDX_Text(pDX, IDC_EDIT_Width, m_boardWidth);
	DDX_Text(pDX, IDC_EDIT_Height, m_boardHeight);
	DDX_Text(pDX, IDC_EDIT_SquareSize, m_squareSize);
	DDX_Text(pDX, IDC_EDIT_BoardCount, m_boardCount);
}

BEGIN_MESSAGE_MAP(CMicroVisionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_Btn_ScanCamera, &CMicroVisionDlg::OnBnClickedBtnScancamera)
	ON_CBN_SELCHANGE(IDC_COMBO_CameraOne, &CMicroVisionDlg::OnCbnSelchangeComboCameraone)
	ON_BN_CLICKED(IDC_Btn_StartCamera, &CMicroVisionDlg::OnBnClickedBtnStartcamera)
	ON_BN_CLICKED(IDC_Btn_StopCamera, &CMicroVisionDlg::OnBnClickedBtnStopcamera)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_Btn_DoCalibration, &CMicroVisionDlg::OnBnClickedBtnDocalibration)
	ON_BN_CLICKED(IDC_Btn_ChessDefault, &CMicroVisionDlg::OnBnClickedBtnChessdefault)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_Btn_StopCalibration, &CMicroVisionDlg::OnBnClickedBtnStopcalibration)
	ON_BN_CLICKED(IDC_Btn_Positioning, &CMicroVisionDlg::OnBnClickedBtnPositioning)
	ON_BN_CLICKED(IDC_Btn_ReBuild, &CMicroVisionDlg::OnBnClickedBtnRebuild)
	ON_BN_CLICKED(IDC_Btn_Recogntion, &CMicroVisionDlg::OnBnClickedBtnRecogntion)
END_MESSAGE_MAP()


// CMicroVisionDlg 消息处理程序

BOOL CMicroVisionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	MVInitLib();

	// 获取程序工作路径
	m_workDir = F_InitWorkDir();
	m_stereoCali.setWorkDir(CStringA(m_workDir).GetBuffer(0));

	GetDlgItem(IDC_Btn_StartCamera)->EnableWindow(FALSE);
	GetDlgItem(IDC_Btn_StopCamera)->EnableWindow(FALSE);
	AllocConsole();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMicroVisionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMicroVisionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMicroVisionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMicroVisionDlg::OnBnClickedBtnScancamera()
{
	// TODO: 在此添加控件通知处理程序代码
	int nCams = 0;
	MVGetNumOfCameras(&nCams);
	m_nCams = min(nCams,CAM_MAX);
	if( m_nCams == 0 )
	{
		MessageBox(_T("没有找到相机,请确认连接和相机IP设置"),_T("提示"),MB_ICONWARNING);
	}
	
	char istr[100];
	CString camstr;
	for(int i=0; i<m_nCams; i++)
	{
		MVSTATUS_CODES r = MVOpenCamByIndex(i, &m_hCam[i]);
		if( m_hCam[i] == NULL || r == MVST_ACCESS_DENIED)
		{
			MessageBox(_T("无法启动相机，可能正被别的软件控制"),_T("提示"),MB_ICONWARNING);
		}
		else
		{
			GetDlgItem(IDC_Btn_StartCamera)->EnableWindow(TRUE);
			GetDlgItem(IDC_Btn_StopCamera)->EnableWindow(TRUE);
		}
 		MVGetCameraInfo(i, &m_camInfo[i]);
		sprintf_s(istr, "%s (%s)", m_camInfo[i].mModelName, m_camInfo[i].mSerialNumber);
		camstr = istr;
 		m_ComboOneList.AddString(camstr);
		m_ComboTwoList.AddString(camstr);
		m_ComboThreeList.AddString(camstr);
		caminfo_idx[camstr] = i;			// 相机信息与id的映射

		int w,h;			
		MVGetWidth(m_hCam[i],&w);
		MVGetHeight(m_hCam[i],&h);
		MVGetPixelFormat(m_hCam[i],&m_PixelFormat[i]);

		m_imgWidth = w;
		m_imgHeight = h;

		if( m_PixelFormat[i] == PixelFormat_Mono8 )
			m_image[i].Create(w,h,8);
		else
		{
			m_image[i].Create(w,h,24);
			img[i].create(h,w,CV_8UC3);
		}
	}
	UpdateData( FALSE );		//将成员变量的值赋值给控件
	GetDlgItem(IDC_Btn_ScanCamera)->EnableWindow(FALSE);
}


void CMicroVisionDlg::OnCbnSelchangeComboCameraone()
{
	// TODO: 在此添加控件通知处理程序代码
}


int StreamCB0(MV_IMAGE_INFO *pInfo, long nUserVal)
{
	CMicroVisionDlg *pDlg = (CMicroVisionDlg *)nUserVal;
	return ( pDlg->OnStreamCB0(pInfo) );
}

int StreamCB1(MV_IMAGE_INFO *pInfo, long nUserVal)
{
	CMicroVisionDlg *pDlg = (CMicroVisionDlg *)nUserVal;
	return ( pDlg->OnStreamCB1(pInfo) );
}

void CMicroVisionDlg::OnBnClickedBtnStartcamera()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strChoosed;
	int cursel[CAM_MAX];
	cursel[0] = m_ComboOneList.GetCurSel();
	if (cursel[0] >= 0 && cursel[0] < CAM_MAX)
	{
		m_ComboOneList.GetLBText(m_ComboOneList.GetCurSel(), strChoosed);
		int camId = caminfo_idx[strChoosed];
 		if (m_hCam[camId] != NULL) {
 			MVStartGrab(m_hCam[camId], StreamCB0, (long)this);
		}
	}
	
	cursel[1] = m_ComboTwoList.GetCurSel();
	if (cursel[1] == cursel[0])
	{
		MessageBox(_T("视图1和视图2所选相机相同"),_T("提示"),MB_ICONWARNING);
	}
	else if (cursel[1] >= 0 && cursel[1] < CAM_MAX)
	{
		m_ComboTwoList.GetLBText(m_ComboTwoList.GetCurSel(), strChoosed);
		int camId = caminfo_idx[strChoosed];
		if (m_hCam[camId] != NULL)
			MVStartGrab(m_hCam[camId], StreamCB1, (long)this);
	}
	double fFPSMin, fFPSMax, fFPS;
	fFPS = 24.0;
	MVGetFrameRateRange(m_hCam[0], &fFPSMin, &fFPSMax);
//	_cprintf("%lf, %lf\n", fFPSMin, fFPSMax);
	SetTimer(1, 1000.0/fFPS, NULL);
}


int CMicroVisionDlg::OnStreamCB0( MV_IMAGE_INFO *pInfo)
{
	ImgRead(pInfo,0);
	return 0;
}

int CMicroVisionDlg::OnStreamCB1( MV_IMAGE_INFO *pInfo)
{
	ImgRead(pInfo,1);
	return 0;
}

void CMicroVisionDlg::ImgRead( MV_IMAGE_INFO *pInfo, int idx )
{
	m_imgWidth = m_image[idx].GetWidth();
	m_imgHeight = m_image[idx].GetHeight();

	if( m_PixelFormat[idx] == PixelFormat_Mono8 )
	{
		// 单通道
		ASSERT( m_image[idx].GetPitch() == m_image[idx].GetWidth() );
		memcpy(m_image[idx].GetBits(),pInfo->pImageBuffer,m_imgWidth*m_imgHeight);
	}
	else
	{
		// RGB三通道
		MVBayerToBGR(m_hCam[idx],pInfo->pImageBuffer,m_image[idx].GetBits(),m_image[idx].GetPitch(),m_imgWidth,m_imgHeight,m_PixelFormat[idx]);
		MVBayerToBGR(m_hCam[idx],pInfo->pImageBuffer,img[idx].data,m_image[idx].GetPitch(),m_imgWidth,m_imgHeight,m_PixelFormat[idx]);
	}

}

void CMicroVisionDlg::F_ShowImage(Mat img, UINT ID)
{
	MVImage mvImage;
	mvImage.Create(m_imgWidth, m_imgHeight, 24);
	if (img.channels() == 3)
	{
		memcpy(mvImage.GetBits(), img.data, m_imgWidth*m_imgHeight*3);
	}
	else if (img.channels() == 1)
	{
		cvtColor(img, img, COLOR_GRAY2BGR);
		memcpy(mvImage.GetBits(), img.data, m_imgWidth*m_imgHeight*3);
	}
	
	CDC* pDC = GetDlgItem(ID) ->GetDC();
	CRect rect;
	GetDlgItem(ID) ->GetClientRect( &rect );
	if (pDC != NULL)
		pDC->SetStretchBltMode(COLORONCOLOR);
	mvImage.Draw(pDC->GetSafeHdc(), 0, 0, rect.right-rect.left, rect.bottom-rect.top);
	ReleaseDC(pDC);
}

CString CMicroVisionDlg::F_InitWorkDir()
{
	CStringA strPath;
	TCHAR path[_MAX_PATH];
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	::GetModuleFileName(AfxGetApp()->m_hInstance, path, _MAX_PATH);
	_tsplitpath_s(path, drive, dir, fname, ext);

	strPath.Empty();
	strPath += CString(drive);
	strPath += CString(dir);

	return CString(strPath);
}

void CMicroVisionDlg::OnBnClickedBtnStopcamera()
{
	// TODO: 在此添加控件通知处理程序代码
	for(int i = 0; i < m_nCams; i++)
	{
		if( m_hCam[i] != NULL )
		{
			MVStopGrab(m_hCam[i]);
		}
	}
}


void CMicroVisionDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	for(int i = 0; i < m_nCams; i++)
	{
		if( m_hCam[i] != NULL )
		{
			MVStopGrab(m_hCam[i]);
		}
	}
	MVTerminateLib();
}

void CMicroVisionDlg::OnBnClickedBtnChessdefault()
{
	// TODO: 在此添加控件通知处理程序代码
	m_boardWidth = 9;
	m_boardHeight = 6;
	m_squareSize = 20;
	m_boardCount = 10;
	UpdateData(FALSE);
}

void CMicroVisionDlg::OnBnClickedBtnDocalibration()
{
	// TODO: 在此添加控件通知处理程序代码

	KillTimer(1);
	int nFoundBoards = 0, nFrame = 0, board_dt = 5;
	Mat frame1, frame2, img1, img2;
	Size imageSize, boardSize;
	MicroVisionCali::CornerData cornerData;			//角点数据
	MicroVisionCali::StereoParams stereoParams;		//双目定标的输出数据
	MicroVisionCali::RemapMatrixs remapMatrixs;		//双目校正的重映射矩阵
	MicroVisionCali::RECTIFYMETHOD method = MicroVisionCali::RECTIFY_BOUGUET;

	UpdateData(TRUE);
	try
	{
		GetDlgItem(IDC_Btn_DoCalibration)->EnableWindow(FALSE);

		CStringA filePath(m_workDir);
		filePath.AppendFormat("CornerData.yml");
		const char* cornerFile = filePath.GetBuffer(0);

		imageSize = Size(m_imgWidth, m_imgHeight);
		boardSize = Size(m_boardWidth, m_boardHeight);
		//初始化角点数据集
		m_stereoCali.initCornerData(m_boardCount, imageSize, boardSize, m_squareSize, cornerData);

		// 开始检测角点
		MSG msg;
		while(nFoundBoards < m_boardCount)
		{
			// MFC 窗口消息处理
			if(::PeekMessage(&msg, this->m_hWnd, 0, 0, PM_REMOVE)) 
			{ 
				if(msg.message == WM_QUIT) 
				{ 
					LPCTSTR errMsg = _T("手动退出双目标定!");
					throw errMsg; 
				}	

				::TranslateMessage(&msg); 
				::DispatchMessage(&msg);
			}
			for(int i = 0; i < m_nCams; i++)
				MVTriggerSoftware(m_hCam[i]);
			img1 = img[0].clone();
			img2 = img[1].clone();

			// 检测角点
			if(m_stereoCali.detectCorners(img1, img2, cornerData, nFoundBoards))
			{
				//Skip every board_dt frames to allow user to move chessboard
				if(++nFrame >= board_dt)
				{
					CStringA imgName( m_workDir );
					imgName.AppendFormat("Imgs\\one%03d.jpg",nFoundBoards);
					string imgname = imgName.GetBuffer(0);
					imwrite(imgname, img[0]);
					imgName = m_workDir;
					imgName.AppendFormat("Imgs\\two%03d.jpg",nFoundBoards);
					imgname = imgName.GetBuffer(0);
					imwrite(imgname, img[1]);

					imgName.ReleaseBuffer();

					nFoundBoards ++;
					nFrame = 0;
				}
				if(nFoundBoards > 0 && nFrame < board_dt)
				{
					//使图像反色，表明同时找到完整的棋盘角点
					bitwise_not(img1, img1);
					bitwise_not(img2, img2);
				}
			}

			// 显示检测到的角点
			F_ShowImage( img1, IDC_ViewOne );
			F_ShowImage( img2, IDC_ViewTwo );
		}
		if (nFoundBoards < 4)
		{
			LPCTSTR errMsg = _T("检测成功的棋盘图像数小于4个，退出双目标定!");
			throw errMsg;
		}
		AfxMessageBox(_T("棋盘角点检测结束"));


		// 保存角点坐标数据
		m_stereoCali.saveCornerData(cornerFile, cornerData);

		//设置标定参数
// 		stereoParams.cameraParams1.flags = CV_CALIB_FIX_PRINCIPAL_POINT + CV_CALIB_USE_INTRINSIC_GUESS +
// 			CV_CALIB_FIX_ASPECT_RATIO + CV_CALIB_ZERO_TANGENT_DIST ;
// 		stereoParams.cameraParams2.flags = CV_CALIB_FIX_PRINCIPAL_POINT + CV_CALIB_USE_INTRINSIC_GUESS +
// 			CV_CALIB_FIX_ASPECT_RATIO + CV_CALIB_ZERO_TANGENT_DIST ;

		stereoParams.cameraParams1.flags = CV_CALIB_FIX_K3 ;
		stereoParams.cameraParams2.flags = CV_CALIB_FIX_K3 ;
		stereoParams.flags = 0;

		//进行摄像机标定
		m_stereoCali.calibrateStereoCamera(cornerData, stereoParams);

		// 计算标定误差
		double avgErr = 0;
		m_stereoCali.getStereoCalibrateError(cornerData, stereoParams, avgErr);
		char info[50];
		sprintf_s( info, "标定误差 = %7.4g", avgErr );
		CString ss;
		ss = info;
		AfxMessageBox(ss);

		// 执行双目校正
		m_stereoCali.rectifyStereoCamera(cornerData, stereoParams, remapMatrixs, method);

		AfxMessageBox(_T("已完成双目校正"));

		// 保存摄像头定标参数	
		filePath = m_workDir;
		filePath.AppendFormat("calib_paras.xml");
		m_stereoCali.saveCalibrationDatas(filePath.GetBuffer(0), method, cornerData, stereoParams, remapMatrixs);

		AfxMessageBox(_T("已保存定标参数"));

		//////////////////////////////////////////////////////////////////////////
		// 显示标定效果
		{
			// 启用退出摄像机定标按钮
			GetDlgItem(IDC_Btn_StopCalibration)->EnableWindow(TRUE);

			MSG msg;
			while (true)
			{
				// MFC 窗口消息处理
				if(::PeekMessage(&msg, this->m_hWnd, 0, 0, PM_REMOVE)) 
				{ 
					if(msg.message == WM_QUIT) 
					{
						break; 
					}	
					::TranslateMessage(&msg); 
					::DispatchMessage(&msg);
				}

				// 载入图像
				for(int i = 0; i < m_nCams; i++)
					MVTriggerSoftware(m_hCam[i]);
				img1 = img[0].clone();
				img2 = img[1].clone();		
				if ( img1.empty() || img2.empty())
					break;

				// 校正图像
				UpdateData(FALSE);
				CButton* m_pCheck = (CButton*)GetDlgItem(IDC_Chk_ShowCaliResult);
				bool showCalibrated = m_pCheck->GetCheck() > 0;
				if ( showCalibrated )
				{
					m_stereoCali.remapImage(img1, img2, img1, img2, remapMatrixs);
				}
				F_ShowImage(img1, IDC_ViewOne);
				F_ShowImage(img2, IDC_ViewTwo);   
			}
		}

		filePath.ReleaseBuffer();
	}
	// 错误信息处理
	catch (LPCTSTR errMsg)
	{
		AfxMessageBox(errMsg);
	}
	catch (cv::Exception& e)
	{
		char err[2048];
		sprintf_s(err, "发生错误: %s", e.what());
		CStringW errInfo(err);
		AfxMessageBox(errInfo);
	}
	catch (...)
	{
		AfxMessageBox(_T("发生未知错误！"));
	}

}  
	


void CMicroVisionDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_hCam[0] != NULL && img[0].isContinuous())
		F_ShowImage(img[0], IDC_ViewOne);
	if(m_hCam[1] != NULL && img[1].isContinuous())
		F_ShowImage(img[1], IDC_ViewTwo);
	if(m_hCam[2] != NULL && img[2].isContinuous())
		F_ShowImage(img[2], IDC_ViewThree);

	CDialogEx::OnTimer(nIDEvent);
}


void CMicroVisionDlg::OnBnClickedBtnStopcalibration()
{
	// TODO: 在此添加控件通知处理程序代码
	// 发送退出消息
	::PostMessage(this->m_hWnd, WM_QUIT, 0, 0);

	// 禁用停止角点检测按钮
	GetDlgItem(IDC_Btn_StopCalibration)->EnableWindow(FALSE);
	GetDlgItem(IDC_Btn_DoCalibration)->EnableWindow(TRUE);
}


void CMicroVisionDlg::OnBnClickedBtnPositioning()
{
	// TODO: 在此添加控件通知处理程序代码
	Mat img1, img2, imgGray1, imgGray2;
	vector<Point2f> corners1, corners2;
	MSG msg;
	while (true)
	{
		// MFC 窗口消息处理
		if(::PeekMessage(&msg, this->m_hWnd, 0, 0, PM_REMOVE)) 
		{ 
			if(msg.message == WM_QUIT) 
			{
				break; 
			}	
			::TranslateMessage(&msg); 
			::DispatchMessage(&msg);
		}

		// 载入图像
		for(int i = 0; i < m_nCams; i++)
			MVTriggerSoftware(m_hCam[i]);
		img1 = img[0].clone();
		img2 = img[1].clone();		
		if ( img1.empty() || img2.empty())
			break;

		cvtColor(img1, imgGray1, CV_BGR2GRAY);
		cvtColor(img2, imgGray2, CV_BGR2GRAY);

		goodFeaturesToTrack(imgGray1, corners1, 1, 0.01, 10, Mat(), 3, false, 0.04);
		goodFeaturesToTrack(imgGray2, corners2, 1, 0.01, 10, Mat(), 3, false, 0.04);

		for(int i = 0; i < corners1.size(); i++)
		{
			circle(img1, corners1[i], 20, Scalar(0, 0, 255), -1);
			_cprintf("img1 [%d] (%f, %f)\n", i, corners1[i].x, corners1[i].y);
		}
		for(int i = 0; i < corners2.size(); i++)
		{
			circle(img2, corners2[i], 20, Scalar(0, 0, 255), -1);
			_cprintf("img2 [%d] (%f, %f)\n", i, corners2[i].x, corners2[i].y);
		}
		F_ShowImage(img1, IDC_ViewOne);
		F_ShowImage(img2, IDC_ViewTwo);   
	}
}

void CMicroVisionDlg::OnBnClickedBtnRebuild()
{
	// TODO: 在此添加控件通知处理程序代码

	try
	{	
		CStringA filePath(m_workDir);
		filePath.AppendFormat("calib_paras.xml");
		const char* calibFile = filePath.GetBuffer(0);
		cv::FileStorage fs(calibFile, cv::FileStorage::READ);

		Mat remapX1, remapY1, remapX2, remapY2;
		fs["remapX1"] >> remapX1;
		fs["remapY1"] >> remapY1;
		fs["remapX2"] >> remapX2;
		fs["remapY2"] >> remapY2;

		CStringA imgName(m_workDir);
		imgName.AppendFormat("Imgs\\one000.jpg");
		Mat img1 = imread(imgName.GetBuffer(0));
		imgName.Empty();
		imgName.AppendFormat(m_workDir + "Imgs\\two000.jpg");
		Mat img2 = imread(imgName.GetBuffer(0));

		if ( !remapX1.empty() && !remapY1.empty() )
		{
			remap( img1, img1, remapX1, remapY1, INTER_LINEAR );
		}
		if ( !remapX2.empty() && !remapY2.empty() )
		{
			remap( img2, img2, remapX2, remapY2, INTER_LINEAR );
		}
		Size imageSize, boardSize;
		MicroVisionCali::CornerData cornerData;			//角点数据
		imageSize = Size(m_imgWidth, m_imgHeight);
		boardSize = Size(m_boardWidth, m_boardHeight);
		//初始化角点数据集
		m_stereoCali.initCornerData(m_boardCount, imageSize, boardSize, m_squareSize, cornerData);
		m_stereoCali.detectCorners(img1, img2, cornerData, 0);
		vector<Point2f> imagePoints1 , imagePoints2;	//第0张图的像素坐标
		imagePoints1 = cornerData.imagePoints1[0];
		imagePoints2 = cornerData.imagePoints2[0];

		Mat Q;
		fs["QMatrix"] >> Q;
		Mat cor = Mat(4, 1, CV_64FC1);
		Mat p;
		for(int i = 0; i < imagePoints1.size(); i++)
		{
			cor.at<double>(0, 0) = imagePoints1[i].x;
			cor.at<double>(1, 0) = imagePoints1[i].y;
			cor.at<double>(2, 0) = imagePoints1[i].x - imagePoints2[i].x;
			cor.at<double>(3, 0) = 1;
 			p = Q * cor;
			_cprintf("%lf %lf %lf\n", p.at<double>(0,0)/p.at<double>(3,0), p.at<double>(1,0)/p.at<double>(3,0), p.at<double>(2,0)/p.at<double>(3,0));
		}
	}
	catch (CMemoryException* e)
	{
		
	}
	catch (CFileException* e)
	{
	}
	catch (CException* e)
	{
		_cprintf("error msg = %s", e);
	}
	
 }



/************************************************************************/
/* 先腐蚀后膨胀，消除细小白点                                            */
/************************************************************************/
void morphOps(Mat &thresh){

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle
	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));

	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);

	dilate(thresh,thresh,dilateElement);
	dilate(thresh,thresh,dilateElement);
}

void CMicroVisionDlg::OnBnClickedBtnRecogntion()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(1);
	Mat img1, img2;

	Mat remapX1, remapY1, remapX2, remapY2, Q;
	CStringA filePath(m_workDir);
	try
	{
		filePath.AppendFormat("calib_paras.xml");
		const char* calibFile = filePath.GetBuffer(0);
		cv::FileStorage fs(calibFile, cv::FileStorage::READ);

		
		fs["remapX1"] >> remapX1;
		fs["remapY1"] >> remapY1;
		fs["remapX2"] >> remapX2;
		fs["remapY2"] >> remapY2;
		fs["QMatrix"] >> Q;
	}
	catch (CException* e)
	{
		_cprintf("error: read remapX, remapY and Q failed!");
	}
	


	MSG msg;
	while(true)
	{
		// MFC 窗口消息处理
		if(::PeekMessage(&msg, this->m_hWnd, 0, 0, PM_REMOVE)) 
		{ 
			if(msg.message == WM_QUIT) 
			{
				break; 
			}	
			::TranslateMessage(&msg); 
			::DispatchMessage(&msg);
		}

		// 载入图像
		for(int i = 0; i < m_nCams; i++)
			MVTriggerSoftware(m_hCam[i]);
		img1 = img[0].clone();
		img2 = img[1].clone();		
		if ( img1.empty() || img2.empty())
			break;

		Mat imgHSV1(img1.rows, img1.cols, CV_8UC3);
		Mat imgHSV2(img2.rows, img2.cols, CV_8UC3);
		Mat threshold1(img1.rows, img1.cols, CV_8UC1);
		Mat threshold2(img2.rows, img2.cols, CV_8UC1);
		cvtColor(img1, imgHSV1, COLOR_BGR2HSV);
		cvtColor(img2, imgHSV2, COLOR_BGR2HSV);
 		inRange(imgHSV1, Scalar(150, 120, 30), Scalar(180, 170, 90), threshold1);
 		inRange(imgHSV2, Scalar(150, 120, 30), Scalar(180, 170, 90), threshold2);
		morphOps(threshold1);
		morphOps(threshold2);

		if ( !remapX1.empty() && !remapY1.empty() )
		{
			remap( threshold1, threshold1, remapX1, remapY1, INTER_LINEAR );
			remap( img1, img1, remapX1, remapY1, INTER_LINEAR );
		}
		if ( !remapX2.empty() && !remapY2.empty() )
		{
			remap( threshold2, threshold2, remapX2, remapY2, INTER_LINEAR );
			remap( img2, img2, remapX2, remapY2, INTER_LINEAR );
		}
			
		int posX1 = 0, posY1 = 0, posX2 = 0, posY2 = 0;
		int count1 = 0, count2 = 0;

		for(int i = 0; i < threshold1.rows; i++)
		{
			// 每一行图像的指针
			const uchar* inData = threshold1.ptr<uchar>(i);
			for(int j = 0; j < threshold1.cols; j = j++)
			{
				if (inData[j] == 255)
				{
					posX1 += j;
					posY1 += i;
					count1 ++;
				}
			}
		}

		for(int i = 0; i < threshold2.rows; i++)
		{
			// 每一行图像的指针
			const uchar* inData = threshold2.ptr<uchar>(i);
			for(int j = 0; j < threshold2.cols; j = j++)
			{
				if (inData[j] == 255)
				{
					posX2 += j;
					posY2 += i;
					count2 ++;
				}
			}
		}

		if (count1 > 0 && count2 > 0)
		{
			posX1 /= count1;
			posY1 /= count1;
			posX2 /= count2;
			posY2 /= count2;
			circle(img1, cv::Point(posX1, posY1), 20, cv::Scalar(0, 255, 0), 10);
			circle(img2, cv::Point(posX2, posY2), 20, cv::Scalar(0, 255, 0), 10);

			Mat cor = Mat(4, 1, CV_64FC1);
			cor.at<double>(0, 0) = posX1;
			cor.at<double>(1, 0) = posY1;
			cor.at<double>(2, 0) = posX1 - posX2;
			cor.at<double>(3, 0) = 1;
			Mat p = Q * cor;
			_cprintf("%lf %lf %lf\n", p.at<double>(0,0)/p.at<double>(3,0), p.at<double>(1,0)/p.at<double>(3,0), p.at<double>(2,0)/p.at<double>(3,0));

		}	
		
		F_ShowImage(img1, IDC_ViewOne);
		F_ShowImage(img2, IDC_ViewTwo); 
	}
}
