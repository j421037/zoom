#include "stdafx.h"
#include <string>
#include <vector>
#include "sdk_demo_app.h"

using namespace DuiLib;

// 获取显示器信息
class Monitors {
public:
	Monitors() {
		EnumDisplayMonitors(0, 0, MonitorEnum, (LPARAM)this);
	}

	std::vector<MONITORINFO> monitorInfos;
	static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lpRect, LPARAM pData)
	{
		MONITORINFO iMonitor;
		iMonitor.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMon, &iMonitor);
		Monitors* pThis = reinterpret_cast<Monitors*>(pData);
		pThis->monitorInfos.push_back(iMonitor);

		return TRUE;
	}
};


DWORD WINAPI _ThreadCheckSplitScreen(LPVOID lpParam);


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
	DWORD ThreadId;
	HANDLE ThreadCheckSplitScreen;
	
	CSDKDemoApp *app_ = new CSDKDemoApp;
	app_->Run(hInstance);
	ThreadCheckSplitScreen = CreateThread(NULL, 0, _ThreadCheckSplitScreen, app_, NULL, &ThreadId);
	
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (WM_QUIT == msg.message)
		{
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	TerminateProcess(ThreadCheckSplitScreen, 0);

	return 0;
}


// 子线程监听多个显示器
DWORD WINAPI _ThreadCheckSplitScreen(LPVOID lpParam)
{
	HWND w1 = NULL;
	HWND w2 = NULL;
	CSDKDemoApp *app_ = static_cast<CSDKDemoApp*>(lpParam);
	ZOOM_SDK_NAMESPACE::SplitScreenInfo screenInfo;
	ZOOM_SDK_NAMESPACE::IMeetingService *service = nullptr;
	ZOOM_SDK_NAMESPACE::IMeetingUIController *ui = nullptr;
	ZOOM_SDK_NAMESPACE::ISettingService *setting = nullptr;
	ZOOM_SDK_NAMESPACE::IGeneralSettingContext *ctx = nullptr;
	ZOOM_SDK_NAMESPACE::IMeetingLiveStreamController *stream;
	CCustomizeInMeetingUIMgr* mgr = nullptr;


	while (true)
	{
		// ui = SDKInterfaceWrap::GetInst().GetMeetingService()->GetUIController();
		service = SDKInterfaceWrap::GetInst().GetMeetingService();
		// setting = SDKInterfaceWrap::GetInst().GetSettingService();
		
		if (nullptr != service ) {

			ui = service->GetUIController();
			// ctx = setting->GetGeneralSettings();
			

			if (nullptr != ui && GetSystemMetrics(SM_CMONITORS) > 0) {
				

				ZOOM_SDK_NAMESPACE::SDKError err = ui->GetMeetingUIWnd(w1, w2);

				

				Monitors monitor;

				ZOOM_SDK_NAMESPACE::MeetingStatus state = service->GetMeetingStatus();
				// MessageBoxA(NULL, to_string(state).c_str(), "会议状态", 0);
				//if (nullptr == mgr && ZOOM_SDK_NAMESPACE::MeetingStatus::MEETING_STATUS_INMEETING == state) {
				//	
				//	// MessageBoxA(NULL, "fcccc", "aa", 0);
				//}

				if (monitor.monitorInfos.size() > 1 && ZOOM_SDK_NAMESPACE::MeetingStatus::MEETING_STATUS_INMEETING == state) {
					

					std::string str = "";
					LONG top = monitor.monitorInfos[0].rcMonitor.top;
					LONG bottom = monitor.monitorInfos[0].rcMonitor.bottom;
					LONG left = monitor.monitorInfos[0].rcMonitor.left;
					LONG right = monitor.monitorInfos[0].rcMonitor.right;

					LONG top_ = monitor.monitorInfos[1].rcMonitor.top;
					LONG bottom_ = monitor.monitorInfos[1].rcMonitor.bottom;
					LONG left_ = monitor.monitorInfos[1].rcMonitor.left;
					LONG right_ = monitor.monitorInfos[1].rcMonitor.right;
					
					// MessageBoxA(NULL, (str + "left: " + to_string(left) + "right : " + to_string(right) + ", top:" + to_string(top) + ", bottom:" + to_string(bottom)).c_str(), "显示器信息", 0);
					
					app_->GetCustomUIHWND(&w1, &w2);

					SetWindowPos(w1, HWND_TOP, 0, 0, right - left, bottom, SWP_NOZORDER);
					SetWindowPos(w2, HWND_TOP, left_, 0, right_ - left_, bottom_, SWP_NOZORDER);
				}
				
				
			}
			
		}
		else {
			// service = SDKInterfaceWrap::GetInst().GetMeetingService();
		}
		
		
		Sleep(3000);
	}
	return 0;
}