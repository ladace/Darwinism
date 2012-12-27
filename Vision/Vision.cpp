#include "Vision.h"

Vision* Vision::m_pVision = NULL;

Vision::Vision()
{
	minIni* ini = new minIni(INI_FILE_PATH);
	LinuxCamera::GetInstance()->Initialize(0);
	//LinuxCamera::GetInstance()->SetCameraSettings(CameraSettings());
	LinuxCamera::GetInstance()->LoadINISettings(ini);
}

Image* Vision::GetFrame()
{
	LinuxCamera::GetInstance()->CaptureFrame();
	return LinuxCamera::GetInstance()->fbuffer->m_RGBFrame;
}
