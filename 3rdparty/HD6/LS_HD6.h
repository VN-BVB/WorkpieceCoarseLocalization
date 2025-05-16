/********************************** LS_HD6系列镭烁激光轮廓仪开发库  ************************************************
**--------------文件信息--------------------------------------------------------------------------------
**文件名: LS_HD6.h
**时间: 20190131
**描述: 用户接口C++头文件, 必须采用C++语法调用。

**------------修订历史记录----------------------------------------------------------------------------
** 版  本: 1.0.0
** 日　期: 20190131
** 描　述: 版本建立
** 版  本: 1.0.1
** 日　期: 20190508
** 描　述: 版本更新
** 版  本: 1.0.2
** 日　期: 20190701
** 描　述: 版本更新
** 版  本: 1.0.3
** 日　期: 20190716
** 描　述: 版本更新：增加配置文件绝对路径输入,增加平均高度计算
** 版  本: 1.0.4
** 日　期: 20190821
** 描　述: 版本更新：无效点处理优化
** 版  本: 1.0.5
** 日　期: 20190826
** 描　述: 版本更新：增加原图数据转轮廓数据功能,设置阈值功能
** 版  本: 1.0.6
** 日　期: 20190926
** 描　述: 版本更新：增加0100W传感器适配，增加传感器温度监视功能
** 版  本: 1.0.7
** 日　期: 20191029
** 描　述: 版本更新：增加像素坐标转物理坐标功能,获取轮廓仪ROI区域
** 版  本: 1.0.8
** 日　期: 20191121
** 描　述: 版本更新：函数库增加了命名空间“LshdProfile”,最大连接数量6提高到8,更新回调函数格式（C++增加void*参数）
** 版  本: 1.0.9
** 日　期: 20200316
** 描　述: 版本更新：增加100DB传感器适配,优化内部算法
** 版  本: 1.1.0
** 日　期: 20200412
** 描　述: 版本更新：增加清除批处理轮廓函数，更新存储批处理轮廓
** 版  本: 1.1.1
** 日　期: 20200520
** 描　述: 版本更新：增加查询方式获取轮廓或原图数据
** 版  本: 1.1.2
** 日　期: 20200629
** 描　述: 版本更新：增加0200和0800传感器适配
** 版  本: 1.2.0
** 日　期: 20200709
** 描　述: 版本更新：内存优化，初始化方式更新，设置回调模式更新,取消LSHD6_SetCameraParameter，LSHD6_SetCameraParameterAbsolutePath
		   取消LSHD6_SetCameraBining ;LSHD6_SetCameraParameterAbsolutePath  LSHD6_SetSysFilePath
** 版  本: 1.2.1
** 日　期: 20200821
** 描　述: 版本更新：增加原图设置ROI获取轮廓功能
** 版  本: 1.2.2
** 日　期: 20200910
** 描　述: 版本更新：修改增益设置（SetGain函数参数由bool型修改为double型）,增加0045W传感器适配
** 版  本: 1.2.3
** 日　期: 20201124
** 描　述: 版本更新：修改0007传感器输出点数及间距
** 版  本: 1.2.4
** 日　期: 20210218
** 描　述: 版本更新：新增激光亮度调整
** 版  本: 1.2.5
** 日　期: 20210304
** 描　述: 版本更新：像素数组转轮廓数组增加输出原始轮廓数组Z
** 版  本: 1.2.6
** 日　期: 20210412
** 描　述: 版本更新：内存优化
** 版  本: 1.2.7
** 日　期: 20210626
** 描　述: 版本更新：LSHD6_GetProfileFromRawImage增加提取类型
** 版  本: 1.3.0
** 日　期: 20210907
** 描　述: 版本更新：增加相机类型类型
** 版  本: 1.3.1
** 日　期: 20211122
** 描　述: 版本更新：将相机回调放入后台线程
** 版  本: 1.3.2
** 日　期: 20211231
** 描　述: 版本更新：
           1.增加0050W 2K分辨率cmos适配,使用LSHD6_SetCameraROI2设置ROI
           2.增加实时频率监测 LSHD6_GetCurFrq;
           3.增加掉线监测日志功能LSHD6_SetLogFuncOn;
           4.增加获取相机物理分辨率LSHD6_GetCMOSSize
** 版  本: 1.3.3
** 日　期: 20211122
** 描　述: 版本更新：LSHD6_GetProfileFromPixelArray增加提取类型
** 版  本: 1.3.4
** 日　期: 20220110
** 描　述: 版本更新：新增0080W传感器适配最高工作频率45HZ
** 版  本: 1.3.5
** 日　期: 20220219
** 描　述: 版本更新：
           1.新增0045W 1900分辨率传感器，最高工作频率45HZ
           2.新增0050W 1900分辨率传感器，最高工作频率50HZ
** 版  本: 1.3.6
** 日　期: 20220505
** 描　述: 版本更新： 
		   1.增加一些日志纪录
		   2.开光激光返回值修改为bool
** 版  本: 1.3.7
** 日　期: 20220519
** 描　述: 版本更新：
		   1.增加UK50,UK55传感器适配，需要在初始化时将相机类型设置为2
		   2.stop函数内置100ms延时提高稳定性
** 版  本: 1.3.8
** 日　期: 20220610
** 描　述: 版本更新：
1.增加0075W适配
** 版  本: 1.4.0
** 日　期: 20220810
** 描　述: 版本更新：
1.优化掉线检测
** 版  本: 1.4.1
** 日　期: 20220902
** 描　述: 版本更新：
1.轮廓补偿优化
** 版  本: 1.4.2
** 日　期: 20220930
** 描　述: 版本更新：
1.增加0045W 2K分辨率相机适配
** 版  本: 1.4.3
** 日　期: 20221019
** 描　述: 版本更新：
1.UK系列已知异常修复
** 版  本: 1.4.4
** 日　期: 20221019
** 描　述: 版本更新：
UK系列已知异常修复
** 版  本: 1.4.5
** 日　期: 20221208
** 描　述: 版本更新：
UK系列增加读写key功能
** 版  本: 1.4.6
** 日　期: 20221212
** 描　述: 版本更新：
增加GV系列适配
** 版  本: 1.4.7
** 日　期: 20230203
** 描　述: 版本更新：
增加HR系列适配
** 版  本: 1.4.8
** 日　期: 20230321
** 描　述: 版本更新：
增加0012W适配
** 版  本: 1.4.9
** 日　期: 20230403
** 描　述: 版本更新：
修正了UK系列的直接获取原图数据及存图的异常
** 版  本: 1.5.0
** 日　期: 20230424
** 描　述: 版本更新：
修正了GV系列的直接获取轮廓数据的异常
** 版  本: 1.5.1
** 日　期: 20230512
** 描　述: 版本更新：
增加0570W传感器适配
** 版  本: 1.5.2
** 日　期: 20230609
** 描　述: 版本更新：
更新回调模式设置函数：LSHD6_SetSingleCallBackMode，增加轮廓重心回调功能
** 版  本: 1.5.3
** 日　期: 20230701
** 描　述: 版本更新：
修正了已知错误
** 版  本: 1.5.4
** 日　期: 20230808
** 描　述: 版本更新：
优化了UK系列的稳定性
** 版  本: 1.5.5
** 日　期: 20230917
** 描　述: 版本更新：
1.优化了UK系列多台同时使用的稳定性
2.增加了激光重心提取函数LSHD6_GetLaserCentroidFromImage
** 版  本: 1.5.6
** 日　期: 20231108
** 描　述: 版本更新：
1.增加0070W适配
**------------------------------------------------------------------------------------------------------

********************************************************************************************************/

#ifndef _LS_HD6_INCLUDE_H
#define _LS_HD6_INCLUDE_H

#ifdef LS_HD6_EXPORTS
#define LS_HD6_API __declspec(dllexport)
#else
#define LS_HD6_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif 

	namespace LshdProfile{

		//报错代码定义
		enum ERR_CODE_HD6
		{
			HD6_ERRCODE_UNKNOWN = 1,//未知错误
			HD6_ERRCODE_FINDNODEVICE = 2,//无相机可连接
			HD6_ERRCODE_ALREADYBEUSED = 3,//相机已被占用
			HD6_ERRCODE_IPWRONG = 4,//无法连接相机:可能是相机未连接到上位机，或者上位机和相机IP不在同一网段
			HD6_ERRCODE_IPUSING = 5,//上位机已被其他相机或应用程序占用
			HD6_ERRCODE_MISSDATAFILE = 6,//未找到传感器配置文件
            HD6_ERRCODE_UNAUTHORIZED = 7,//未授权
        };
		/*********************************************************
		回调函数格式声明
		**********************************************************/
		/***轮廓仪采集到单个轮廓的回调函数（低速，每帧一次回调）
		@param profileX   单个轮廓的X轴数据指针
		@param profileZ   单个轮廓的Z轴数据指针
		@param profileZ   单个轮廓的点数，一般固定为500~1000
		@param pImage   原始图像数据指针
		@param imgWidth   原始图像宽度（单位：像素）
		@param imgHeight   原始图像高度（单位：像素）
		@param imgHeight   原始图像高度（单位：像素）
		@param timeStamp 时间戳
		轮廓和原图数据不会同时回传，可通过LSHD6_SetSingleCallBackMode进行切换
		***/
		typedef void(_cdecl*pCallbackSingleProfile)(double* profileX, double* profileZ, int count, unsigned char* pImage, int imgWidth, int imgHeight,unsigned int timeStamp, void* userDefine);
		/***轮廓仪采集到批量轮廓的回调函数（高速，全部帧结束后回调）
		@param profileZ   批量轮廓的Z轴数据指针(数据长度为xcount*ycount）
		@param profileX   批量轮廓的X轴数据指针(数据长度为xcount）
		@param timeStamp  批量轮廓的时间戳数组
		@param callbackIndex  批量轮廓的回调批次
		@param xcount	  批量轮廓中单个轮廓的数据点数，一般为500~1000(每个单个轮廓都是一样的）
		@param ycount	  批量轮廓的总数量，一般为20000以下
		@param userDefine 用户自定义数据指针，一般可传入this方便调用用户自己class中函数
		***/
		typedef void(_cdecl*pCallbackBatchProfile)(double* profileZ, double* profileX, unsigned int* timeStamp,int callbackIndex,int xcount, int ycount,void* userDefine);
		/*************************************************************
		说明：获取开发库版本号
		输入：无
		输出：无
		返回值：开发库版本号
		*************************************************************/
		LS_HD6_API  double __stdcall LSHD6_GetVersion();

		/*************************************************************
		说明：初始化轮廓仪带轮廓显示功能
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param cameraSerialNumber 轮廓仪配置文件，可使用相对路径，例如 “SN6-0020-1143989”配置文件需放入工程目录下，发布程序时放入exe同级目录下
		也可使用绝对路径，例如“D:\\HDLaser\\filePath\\SN6-0020-1143989”,不能含中文字符
		@param hWnd 显示控件的窗口句柄，如不需要显示可以设置 NULL，（参数imgLeft，imgLeft，imgWidth，imgHeight将不起作用）
		@param imgLeft：原始图像或轮廓曲线在显示控件区域左上角行坐标（单位pixel）
		@param imgTop：原始图像或轮廓曲线在显示控件区域左上角行坐标（单位pixel）
		@param imgWidth:原始图像或轮廓曲线在显示控件区域宽度（单位pixel）
		@param imgHeight:原始图像或轮廓曲线在显示控件区域高度（单位pixel）
		@param AcquisitionFrameRate:帧速（单位：帧/秒，常用50~300帧/秒)
		@param highGain:增益（高增益可设置true，低增益设置false。低增益可降低图片噪声但需要更高的曝光时长）
		@param TriggerMode：0软件触发采集轮廓，1外部IO低频率触发（100帧/秒以下），2编码器高速触发
		@param HBinning 水平方向binning，true则开启，false关闭，默认关闭状态（合并水平相邻的两个像素，可以降低精度提高速度）
		@param VBinning 垂直方向binning，true则开启，false关闭，默认关闭状态（合并垂直相邻的两个像素，可以降低精度提高速度）
		@param cameraType 相机类型 1 HD6/8系列 2 UK系列 3 HK系列
		@param cameraIP相机IP，例如"192.168.2.2"
		返回值：初始化成功返回0，其他参看错误码
		*************************************************************/
		LS_HD6_API  int __stdcall LSHD6_InitialCamera( int cameraIndex,  char* cameraSerialNumber,  HWND hWnd,
			 int imgLeft,  int imgTop,  int imgWidth,  int imgHeight,
			 double ExposureTime, double AcquisitionFrameRate, bool highGain = true, int TriggerMode = 0, bool HBinning = false, bool VBinning = false, int cameraType = 1, const char* cameraIP = "");

		/*************************************************************
		说明：初始化轮廓仪不带轮廓显示功能
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param cameraSerialNumber 轮廓仪配置文件，可使用相对路径，例如 “SN6-0020-1143989”配置文件需放入工程目录下，发布程序时放入exe同级目录下
		也可使用绝对路径，例如“D:\\HDLaser\\filePath\\SN6-0020-1143989”,不能含中文字符
		@param ExposureTime:曝光时间（单位：微秒，一般使用100~3000)
		@param AcquisitionFrameRate:帧速（单位：帧/秒，常用50~300帧/秒)
		@param highGain:增益（高增益可设置true，低增益设置false。低增益可降低图片噪声但需要更高的曝光时长）
		@param TriggerMode：0软件触发采集轮廓，1外部IO低频率触发（100帧/秒以下），2编码器高速触发
		@param HBinning 水平方向binning，true则开启，false关闭，默认关闭状态（合并水平相邻的两个像素，可以降低精度提高速度）
		@param VBinning 垂直方向binning，true则开启，false关闭，默认关闭状态（合并垂直相邻的两个像素，可以降低精度提高速度）
                @param cameraType 相机类型 1 HD6/8系列 2 UK系列 3 HK系列
		@param cameraIP相机IP，例如"192.168.2.2"
		返回值：初始化成功返回0，其他参看错误码
		*************************************************************/
                LS_HD6_API  int __stdcall LSHD6_InitialCameraWithoutUI(int cameraIndex,  char* cameraSerialNumber,
			double ExposureTime, double AcquisitionFrameRate, bool highGain = true, int TriggerMode = 0, bool HBinning = false, bool VBinning = false, int cameraType = 1, const char* cameraIP = "");
		/*************************************************************
		说明：获取可用轮廓仪数量
		输入：
		@param cameraType 相机类型 1 HD6/8系列 2 HC系列 3 HK系列
		输出：无
		返回值：可用轮廓仪数量
		*************************************************************/
		LS_HD6_API  int __stdcall LSHD6_GetEnableCamereNumber(int cameraType = 1);
		/*************************************************************
		说明：控制是否显示轮廓曲线或者原始图像，一般用来预览和调试时设置显示，运行时设置为不显示
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param showUI 是否显示轮廓曲线或者原始图像，true为显示，false为不显示.较高速度运行时需要设置false以降低CPU负荷
		输出：无
		返回值：无
		*************************************************************/
		LS_HD6_API  void __stdcall LSHD6_SetshowUI(int cameraIndex, bool showUI);
		/*************************************************************
		说明：设置单轮廓回调函数
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param func 用户回调函数
		@param userDefine 回传用户自定义数据指针
		输出：无
		返回值：无
		*************************************************************/
		LS_HD6_API  void __stdcall LSHD6_SetSingleCallBack(int cameraIndex, pCallbackSingleProfile func,void* userDefine=NULL);
		/*************************************************************
		说明：设置数据回调模式，必须在相机停止状态下设置
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param dataMode 数据回传模式，如需在回调函数或读取函数中获取轮廓设置0，获取原始图像设置为1，获取重心设置为2，其他值默认为轮廓

		输出：无
		返回值：true设置成功，false设置失败
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_SetSingleCallBackMode(int cameraIndex, int dataMode=0);
		/*************************************************************
		说明：设置批处理轮廓回调函数
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param func 用户回调函数
		@param userDefine 回传用户自定义数据指针
		输出：无
		返回值：无
		*************************************************************/
		LS_HD6_API  void __stdcall LSHD6_SetBatchCallBack(int cameraIndex, pCallbackBatchProfile func, void* userDefine = NULL);
		/*************************************************************
		说明：设置批处理轮廓回调函数参数
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param CntOneTime 每次回调或读取批量轮廓数量，200~5000
		输出：无
		返回值：无
		*************************************************************/
		LS_HD6_API  void __stdcall LSHD6_SetBatchCallBackParam(int cameraIndex,int CntOneTime);

		/*************************************************************
		说明：获取轮廓仪温度
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		输出：无
		返回值：轮廓仪温度，-99为无效值
		*************************************************************/
		LS_HD6_API  double __stdcall LSHD6_GetTemperature(int cameraIndex);
		/*************************************************************
		说明：停止轮廓仪工作，进入待机状态
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		输出：无
		返回值：无
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_Stop(int cameraIndex);
		/*************************************************************
		说明：开启激光
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param brightnessLevel 激光亮度级别1~10;亮度随数值增大而增大
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_SetLaserOn(int cameraIndex,int brightnessLevel=5);

		/*************************************************************
		说明：关闭激光
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_SetLaserOff(int cameraIndex);


		/*************************************************************
		说明：同步时间戳
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_ReSetTimeStamp(int cameraIndex);
		/*************************************************************
		说明：获取PC时间戳
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		输出：无
		返回值：PC的时间戳
		*************************************************************/
		LS_HD6_API  unsigned int __stdcall LSHD6_GetPCTimeStamp(int cameraIndex);


		/*************************************************************
		说明：内部软件触发方式进行连续取像，在显示UI的情况下建议帧速50以下，否则会造成系统卡顿，不显示UI时则可以根据需要提高
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param frequency 轮廓仪低速帧速（1~100帧/秒），目前无效，可通过LSHD6_SetFrameRate设置
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_LowSpeedGrab(int cameraIndex, double frequency);
		/*************************************************************
		说明：外部触发方式进行连续取像，外部每到来一个上升沿信号则触发取像一次
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_LowSpeedGrabOutter(int cameraIndex);
		/*************************************************************
		说明：内部软件触发方式进行固定帧数取像，每次执行前需要执行LSHD6_Stop,UK系列不支持
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param snapNumber 触发次数，最少为1
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_Snap(int cameraIndex, int snapNumber);

		/*************************************************************
		说明：设置轮廓仪取像区域及显示区域，轮廓仪需要在初始化完成后，且在停止状态（单位：像素）
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param ROIleft 轮廓仪取像区域左上列坐标（0~1200，16的倍数）
		@param ROItop 轮廓仪取像区域左上行坐标（0~1024，1的倍数）
		@param ROIWidthSize 轮廓仪取像区域宽度（64~1200，16的倍数）
		@param ROIHeightSize 轮廓仪取像区域高度（1~1023，1的倍数）
		@param imgLeft：如需要显示原始图像或轮廓曲线，在此处传入显示控件区域左上角行坐标（单位pixel）
		@param imgTop：如需要显示原始图像或轮廓曲线，在此处传入显示控件区域左上角行坐标（单位pixel）
		@param imgWidth:如需要显示原始图像或轮廓曲线，在此处传入显示控件区域宽度（单位pixel）
		@param imgHeight:如需要显示原始图像或轮廓曲线，在此处传入显示控件区域高度（单位pixel）
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_SetCameraROI(int cameraIndex, int ROIleft, int ROItop, int ROIWidthSize, int ROIHeightSize,
			int imgLeft=0, int imgTop=0, int imgWidth=0, int imgHeight=0);
		/*************************************************************
		说明：设置轮廓仪取像区域及显示区域，轮廓仪需要在初始化完成后，且在停止状态(（单位：像素。适合1900以上分辨率传感器）
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param ROIleft 轮廓仪取像区域左上列坐标（0~1200，16的倍数）
		@param ROItop 轮廓仪取像区域左上行坐标（0~1024，1的倍数）
		@param ROIWidthSize 轮廓仪取像区域宽度（64~1600，16的倍数）
		@param ROIHeightSize 轮廓仪取像区域高度（1~1500，1的倍数）
		@param imgLeft：如需要显示原始图像或轮廓曲线，在此处传入显示控件区域左上角行坐标（单位pixel）
		@param imgTop：如需要显示原始图像或轮廓曲线，在此处传入显示控件区域左上角行坐标（单位pixel）
		@param imgWidth:如需要显示原始图像或轮廓曲线，在此处传入显示控件区域宽度（单位pixel）
		@param imgHeight:如需要显示原始图像或轮廓曲线，在此处传入显示控件区域高度（单位pixel）
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_SetCameraROI2(int cameraIndex, int ROIleft, int ROItop, int ROIWidthSize, int ROIHeightSize,
			int imgLeft=0, int imgTop=0, int imgWidth=0, int imgHeight=0);
		/*************************************************************
		说明：获取轮廓仪当前取像区域（单位：像素）
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		输出：
		@param ROIleft 轮廓仪取像区域左上列坐标（0~1200，16的倍数）
		@param ROItop 轮廓仪取像区域左上行坐标（0~1024，1的倍数）
		@param ROIWidthSize 轮廓仪取像区域宽度（64~1200，16的倍数）
		@param ROIHeightSize 轮廓仪取像区域高度（1~1023，1的倍数）
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_GetCameraROI(int cameraIndex, int& ROIleft, int& ROItop, int& ROIWidthSize, int& ROIHeightSize);
		/*************************************************************
		说明：获取轮廓仪相机物理分辨率（单位：像素）
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		输出：
		@param Width CMOS宽度
		@param Height CMOS高度
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_GetCMOSSize(int cameraIndex,  int& Width, int& Height);
		/*************************************************************
		说明：设置轮廓仪曝光
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param value 曝光时间（单位微秒，一般使用100~3000)
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_SetExposureTime(int cameraIndex, double value);
		/*************************************************************
		说明：获取轮廓仪曝光
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		输出：无
		返回值：轮廓仪曝光值，单位：微秒，-99代表获取失败
		*************************************************************/
		LS_HD6_API  double __stdcall LSHD6_GetExposureTime(int cameraIndex);
		/*************************************************************
		说明：设置轮廓仪增益
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param gainValue 轮廓仪增益值（1~8）
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_SetGain(int cameraIndex, double gainValue);

		/*************************************************************
		说明：设置轮廓仪帧速，必须在相机停止取向状态下以及触发模式设置为软件触发时使用
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param value 轮廓仪帧速（1~400帧/秒,不同型号轮廓仪可设置的最大值不同，300mm量程以下一般全幅面200帧/秒，300mm量程以上可全幅面400帧/秒)
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_SetFrameRate(int cameraIndex, double value);

		/*************************************************************
		说明：获取轮廓仪当前帧速，在LSHD6_LowSpeedGrab触发成功后可作为传感器是否工作过程中发生掉线的监控方法
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		输出：
		@param frq 轮廓仪当前帧速	
		返回值：传感器在线返回true，离线返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_GetCurFrq(int cameraIndex, double& frq);

		/*************************************************************
		说明：获取轮廓仪帧速，必须在触发模式设置为软件触发时使用
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		输出：无
		返回值：轮廓仪帧速，-99代表获取失败
		*************************************************************/
		LS_HD6_API  double __stdcall LSHD6_GetFrameRate(int cameraIndex);

		/*************************************************************
		说明：设置轮廓仪触发模式，需在轮廓仪停止状态下，UK系列不支持，默认为软件触发
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param value：触发模式，0软件触发采集轮廓，1外部IO低频率触发（100帧/秒以下），2编码器高速触发
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_SetTriggerMode(int cameraIndex, int value);

		/*************************************************************
		说明：计算安装角度
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param LineStart 计算角度起始端
		@param LineStart 计算角度结束端
		输出：无
		返回值：根据标定物计算出的设备安装角度（单位：弧度），标定物要尽可能的水平以提高系统综合精度
		*************************************************************/
		LS_HD6_API  double __stdcall LSHD6_CalDeviceAngle(int cameraIndex, double LineStart, double LineEnd);

		/*************************************************************
		说明：安装角度补偿
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param deviceAngle 安装角度
		输出：无
		返回值：无
		*************************************************************/
		LS_HD6_API  void __stdcall LSHD6_SetDeviceAngle(int cameraIndex, double deviceAngle);

		/*************************************************************
		说明：计算平均高度
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param LineStart 计算角度起始端
		@param LineStart 计算角度结束端
		输出：无
		返回值：区域平均高度
		*************************************************************/
		LS_HD6_API  double __stdcall LSHD6_CalPlaneAverageHeight(int cameraIndex, double LineStart, double LineEnd);

		/*************************************************************
		说明：设备安装高度补偿，一般在多台并用时使用
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param heightOffset 高度补偿
		输出：无
		返回值：无
		*************************************************************/
		LS_HD6_API  void __stdcall LSHD6_SetDeviceHeightOffset(int cameraIndex, double heightOffset);


		/*************************************************************
		说明：无效点填补
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param type 补间类型，0不补间，1垂直补间，2直线补间
		@param compansationCnt ,补间距离阈值，连续无效点数量小于该值则转换为有效点
		@param downLevel ,补偿数据下沉量
		输出：无
		返回值：无
		*************************************************************/
		LS_HD6_API  void __stdcall LSHD6_SetCompansation(int cameraIndex, int type, int compansationCnt, double downLevel=0);

		/*************************************************************
		说明：轮廓滤波
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param type 滤波类型，0不滤波，1均值滤波，2中值滤波，3均值滤波+中值滤波
		@param SmoothNumber 均值滤波值
		@param SmoothTimes 均值滤波次数
		@param MidNumber 中值滤波值
		输出：无
		返回值：无
		*************************************************************/
		LS_HD6_API  void __stdcall LSHD6_SetFilter(int cameraIndex, int type, int SmoothNumber, int SmoothTimes, int MidNumber);

		/*************************************************************
		说明：获取轮廓尺寸信息
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param profileXMax 轮廓X方向最大值
		@param profileZMax 轮廓Z方向最大值
		输出：无
		返回值：无
		*************************************************************/
		LS_HD6_API  void __stdcall LSHD6_GetProfileSize(int cameraIndex, double& profileXMax, double& profileZMax);
		/*************************************************************
		说明：获取轮廓点数和间隔信息
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param profileCount 轮廓点数
		@param profileXpitch 轮廓X间隔
		输出：无
		返回值：无
		*************************************************************/
		LS_HD6_API  void __stdcall LSHD6_GetProfileCountAndPitch(int cameraIndex, int& profileCount, double& profileXpitch);

		/*************************************************************
		说明：申请批处理内存
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param ProfileCnt：轮廓个数，最大不超过20000
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_SetBatchProfileBuffer(int cameraIndex, int ProfileCnt);

		/*************************************************************
		说明：相机高速扫描--固定帧数内部触发高速扫描
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param snapNumber：轮廓个数，最大不超过申请内存的轮廓个数
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_InnerTriggerHighSpeedScan(int cameraIndex, int snapNumber);

		/*************************************************************
		说明：相机高速扫描--内部触发高速循环扫描，不固定帧数，200帧以下可长时间工作，200帧以上内存循环存满后自动停止
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_InnerTriggerHighSpeedConstantScan(int cameraIndex);

		/*************************************************************
		说明：相机高速扫描--固定帧数编码器触发高速扫描
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param snapNumber：轮廓个数，最大不超过申请内存的轮廓个数
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_EncoderTriggerHighSpeedScan(int cameraIndex, int snapNumber);

		/*************************************************************
		说明：相机高速扫描--编码器触发高速循环扫描，不固定帧数，200帧以下可长时间工作，200帧以上内存循环存满后自动停止
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_EncoderTriggerHighSpeedConstantScan(int cameraIndex);

		/*************************************************************
		说明：获取处理轮廓的数量
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		输入：无
		输出：
		@param triggerNumber 触发次数
		@param treatNumberMid 缓存数量
		@param treatNumber 生成轮廓数量
		@param udpSendNumber UDP发送轮廓数量
		返回值：无
		*************************************************************/
		LS_HD6_API  void __stdcall LSHD6_GetConstantScanningNumber(int cameraIndex, int& triggerNumber, int& treatNumberMid,
			int& treatNumber, int& udpSendNumber);

		/*************************************************************
		说明：获创建UDP服务器
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		输入：
		@param LocalIp0 本地ip第1段
		@param LocalIp1 本地ip第2段
		@param LocalIp2 本地ip第3段
		@param LocalIp3 本地ip第4段
		@param LocalPortNumer 本地端口号
		@param TargetIp0 目标ip第1段
		@param TargetIp1 目标ip第2段
		@param TargetIp2 目标ip第3段
		@param TargetIp3 目标ip第4段
		@param TargetPortNumber 目标端口号
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_CreateUDPserver(int cameraIndex, int LocalIp0, int LocalIp1, int LocalIp2, int LocalIp3, int LocalPortNumer,
			int TargetIp0, int TargetIp1, int TargetIp2, int TargetIp3, int TargetPortNumber);

		/*************************************************************
		说明：销毁UDP服务器
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_UDPDestroy(int cameraIndex);

		/*************************************************************
		说明：UDP发送数据
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param UdpMessage发送内容
		@param length发送字节数
		输出：无
		返回值：无
		*************************************************************/
		LS_HD6_API  void __stdcall LSHD6_SendUDPbroadcast(int cameraIndex, char* UdpMessage, int length);

		/*************************************************************
		说明：是否启用UDP发送数据
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param udpsendOrNot 如需UDP发送数据则设置true，否则设置为false
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_UDPEnable(int cameraIndex, bool udpsendOrNot);
		/*************************************************************
		说明：保存当前轮廓
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param fileName为文件名，不需要加文件扩展名。系统会在当前目录下创建文件夹 MeasureData并保存txt文本文件
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_SaveCurrentSingleProfile(int cameraIndex, char* fileName);

		/*************************************************************
		说明：保存批处理轮廓
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param fileName为文件名，不需要加文件扩展名。系统会在当前目录下创建文件夹 MeasureData并保存csv文本文件
		@param profileCnt为要存储的轮廓数量，该值不能大于设置的扫描数量
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_SaveBatchProfile(int cameraIndex, char* fileName, int profileCnt);

		/*************************************************************
		说明：清除当前缓存中的批处理轮廓，全部置为无效值-99.999。清除数量为当前一帧设置的扫描轮廓数量，一般在存储完当前轮廓后调用，
		当下次设置扫描数量较大而实际扫描轮廓数量较少时不会存储重复数据
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_ClearBatchProfile(int cameraIndex);
		/*************************************************************
		说明：保存高度图
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param fileName为文件名，不需要加文件扩展名。系统会在当前目录下创建文件夹 Picture并保存bmp8位灰度图
		@param heightUpper 高度上限，映射灰度255
		@param heightLower 高度下限，映射灰度0，中间高度等比例映射
		输出：无
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_SaveHeightPicture(int cameraIndex, char* fileName, double heightUpper, double heightLower);

		/*************************************************************
		说明：保存原始图像
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		输出：无
		返回值：成功返回true，失败返回false.系统在程序根目录下创建Picture文件夹，文件名为LSXRawImage.bmp，X表示轮廓仪序号
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_SaveRawImage(int cameraIndex,char* imgName);
		/*************************************************************
		说明：从原图中获取激光重心像素坐标
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param ImageData 图片数据数组,尺寸需要和从相机获取的尺寸一致
		@param grayT 灰度阈值，低于该值的激光线不提取,一般设置50~100
		@param widthT 宽度阈值，高于该值的激光线不提取，一般设置10~30
		@param type提取类型，0为默认类型，1为抗反射类型
		输出：
		@param pixelX 激光像素坐标X数组
		@param pixelZ 激光像素坐标Z数组
		@param pixelCnt 激光像素坐标点数
		返回值：无
		*************************************************************/
		LS_HD6_API  void __stdcall LSHD6_GetLaserCentroidFromImage(int cameraIndex, unsigned char ImageData[], int grayT, int widthT, int type, double pixelX[], double pixelZ[], int& pixelCnt);
		/*************************************************************
		说明：原始图像转轮廓数据。适合特殊场合原始图像受到多重反射或镜面反射或其他干扰无法获取良好的轮廓，可以先获取
		原始图像数据，用户自行编写算法过滤干扰后再使用该函数获取轮廓数据，可以获取质量较好的轮廓
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param ImageData 图片数据数组
		@param type提取类型，0为默认类型，1为抗反射类型
		输出：
		@param profileX 轮廓坐标X数组
		@param profileZ 轮廓坐标Z数组
		@param profileCnt 轮廓点数
		返回值：无
		*************************************************************/
		LS_HD6_API  void __stdcall LSHD6_GetProfileFromRawImage(int cameraIndex, unsigned char ImageData[], double profileX[], double profileZ[], int& profileCnt,int type=0);
		/*************************************************************
		说明：原始图像(带ROI)转轮廓数据。适合特殊场合原始图像受到多重反射或镜面反射或其他干扰无法获取良好的轮廓，可以先获取
		原始图像数据，用户自行编写算法过滤干扰后再使用该函数获取轮廓数据，可以获取质量较好的轮廓
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param ImageData 图片数据数组
		@param ROILeft 图片ROI左
		@param ROITop 图片ROI上
		@param ROIWidth 图片ROI宽度
		@param ROIHeight 图片ROI高度
		输出：
		@param profileX 轮廓坐标X数组
		@param profileZ 轮廓坐标Z数组
		@param profileCnt 轮廓点数
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_GetProfileFromRawImageWithROI(int cameraIndex, unsigned char ImageData[],int ROILeft,int ROITop,int ROIWidth,int ROIHeight, double profileX[], double profileZ[], int& profileCnt);
		/*************************************************************
		说明：像素(数组）坐标转轮廓数据。适合特殊场合原始图像受到多重反射或镜面反射或其他干扰无法获取良好的轮廓，可以先获取
		原始图像数据，用户自行编写算法过滤干扰并提取像素坐标后再使用该函数获取轮廓数据，可以获取质量较好的轮廓
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param pixelX 像素坐标X数组
		@param pixelZ 像素坐标Z数组
		@param pixelCnt 像素点数
		@param type 提取类型 0表示平滑1表示不平滑
		输出：
		@param profileX 轮廓坐标X数组
		@param profileZ 轮廓坐标Z数组
		@param profileCnt 轮廓点数
		@param profileZraw 原始轮廓坐标Z数组（未填补）
		返回值：无
		*************************************************************/
		LS_HD6_API  void __stdcall LSHD6_GetProfileFromPixelArray(int cameraIndex, double pixelX[], double pixelZ[], int pixelCnt, double profileX[], double profileZ[], int& profileCnt, double profileZraw[],int type=0);
		/*************************************************************
		说明：像素（单个）坐标转轮廓数据。适合特殊场合原始图像受到多重反射或镜面反射或其他干扰无法获取良好的轮廓，可以先获取
		原始图像数据，用户自行编写算法定位单点像素坐标后再使用该函数获取轮廓数据，可以获取稳定的特征点
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param pixelX 像素坐标X
		@param pixelZ 像素坐标Z
		输出：
		@param profileX 轮廓坐标X
		@param profileZ 轮廓坐标Z
		返回值：无
		*************************************************************/
		LS_HD6_API  void __stdcall LSHD6_GetProfileFromSinglePixel(int cameraIndex, double pixelX, double pixelZ, double& profileX, double& profileZ);
		/*************************************************************
		说明：设置从原始图像提取轮廓像素坐标的亮度阈值
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param threshold 亮度阈值（10~250）
		输出：无
		返回值：无
		*************************************************************/
		LS_HD6_API  void __stdcall LSHD6_SetRawImageThreshold(int cameraIndex, int threshold);
		/*************************************************************
		说明：查询获取单个轮廓数据，此函数会阻塞线程直至获取到新的轮廓数据，因此建议用户在独立的线程中执行此函数
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param timeout 超时时间 1~999999单位：毫秒
		输出：
		@param profileX 轮廓X数组
		@param profileZ 轮廓Z数组
		@param profileCnt 轮廓点数
		@param  timeStamp 时间戳
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_GetProfileData(int cameraIndex, double profileX[], double profileZ[], int& profileCnt,unsigned int& timeStamp, int timeout);
		/*************************************************************
		说明：查询获取原图数据，此函数会阻塞线程直至获取到新的原图数据，因此建议用户在独立的线程中执行此函数
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param timeout 超时时间 1~999999单位：毫秒
		输出：
		@param ImgData 图像数据
		@param imgW 图像宽度
		@param imgH 图像高度
		@param  timeStamp 时间戳
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_GetImageData(int cameraIndex, unsigned char ImgData[], int& imgW, int& imgH, unsigned int& timeStamp, int timeout);
		/*************************************************************
		说明：查询获取批处理轮廓数据，此函数会阻塞线程直至获取到新的批处理轮廓数据，因此建议用户在独立的线程中执行此函数
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param timeout 超时时间 1~999999单位：毫秒
		输出：
		@param batchProfile 批处理轮廓数据
		@param PointCnt 每条轮廓点数
		@param batchCnt 轮廓数量
		@param  timeStamp 时间戳数组
		@param  batchIndex 回调批次
		返回值：成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_GetBatchProfData(int cameraIndex, double batchProfile[], int& PointCnt, int& batchCnt, unsigned int timeStamp[], int& batchIndex, int timeout);
		/*************************************************************
		说明：销毁轮廓仪，重新初始化前需执行
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		输出：无
		返回值：停止成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  bool __stdcall LSHD6_DestroyCamera(int cameraIndex);

		/*************************************************************
		说明：打开日志存储开关，记录传感器运行状况
		输入：
		@param cameraIndex 轮廓仪编号，如果为连接的第一台轮廓仪则设置1，第二台设置为2，以此类推，最大值8
		@param open true表示打开，open表示关闭
		输出：无
		返回值：停止成功返回true，失败返回false
		*************************************************************/
		LS_HD6_API  void __stdcall LSHD6_SetLogSwtichOn(int cameraIndex, bool open = true);
		/*************************************************************
		说明：读取用户key，最大64个,UK系列支持
		输入：
		@param index:相机序号1~8
		@param length:读出字节长度，1~64
		输出：
		@param byte:读出的key数组
		返回值：
		0,//读取成功
		-1,//读取失败
		*************************************************************/
		LS_HD6_API bool __stdcall LSHD6_GetUserKey(int cameraIndex, char key[], int length);
		/*************************************************************
		说明：设置用户key，最大64个，UK系列支持
		输入：
		@param index:相机序号1~8
		@param byte:写入的key数组
		@param length:写入字节长度，1~64
		输出：

		返回值：
		0,//设置成功
		-1,//设置失败
		*************************************************************/
		LS_HD6_API bool __stdcall LSHD6_SetUserKey(int cameraIndex, char key[], int length);

	}
	
#ifdef  __cplusplus
}
#endif
#endif
