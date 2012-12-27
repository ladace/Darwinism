#include "LinuxDARwIn.h"
#include "robot.h"
#include "video.h"
#include <cassert>

#define INI_FILE_PATH       "config.ini"

Image* camera_buf = NULL;
void* send_buf = NULL;

pthread_mutex_t buf_mutex;

void video_initialize() {
    pthread_mutex_init(&buf_mutex, NULL);

    minIni* ini = new minIni(INI_FILE_PATH); // need not be deleted?

    printf("============= Camera Initializing =============\n");
    LinuxCamera::GetInstance()->Initialize(0);
    LinuxCamera::GetInstance()->SetCameraSettings(CameraSettings());
    LinuxCamera::GetInstance()->LoadINISettings(ini);
    printf("------------- Camera Initialized --------------\n");

    camera_buf = new Image(Camera::WIDTH, Camera::HEIGHT, Image::RGB_PIXEL_SIZE);
    send_buf = malloc(Camera::WIDTH * Camera::HEIGHT * Image::RGB_PIXEL_SIZE);
}

void send_snapshot(httpd* server) {
    pthread_mutex_lock(&rbt_mutex);
        LinuxCamera::GetInstance()->CaptureFrame();
        memcpy(camera_buf->m_ImageData, LinuxCamera::GetInstance()->fbuffer->m_RGBFrame->m_ImageData, LinuxCamera::GetInstance()->fbuffer->m_RGBFrame->m_ImageSize);
    pthread_mutex_unlock(&rbt_mutex);

    assert(camera_buf->m_PixelSize == Image::RGB_PIXEL_SIZE)

    pthread_mutex_lock(&buf_mutex);

        size_t size = jpeg_utils::compress_rgb_to_jpeg(camera_buf->m_ImageData, send_buf, camera_buf->m_ImageSize, 80);

        httpdSetContentType(server, "image/jpeg");
        httpdSendHeader(server);
        _httpd_net_write(server->ClientSock, send_buf, size)
    pthread_mutex_unlock(&buf_mutex);
}
