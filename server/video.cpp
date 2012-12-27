#include "LinuxDARwIn.h"
#include "Image.h"
#include "LinuxCamera.h"
#include "robot.h"
#include "video.h"
#include "jpeg_utils.h"
#include "Vision.h"
#include <cassert>

Image* camera_buf = NULL;
unsigned char* send_buf = NULL;

pthread_mutex_t buf_mutex;

extern "C" {

void video_initialize() {
    pthread_mutex_init(&buf_mutex, NULL);

    camera_buf = new Image(Camera::WIDTH, Camera::HEIGHT, Image::RGB_PIXEL_SIZE);
    send_buf = (unsigned char*)malloc(Camera::WIDTH * Camera::HEIGHT * Image::RGB_PIXEL_SIZE);

    printf("============= Camera Initializing =============\n");
    Vision::GetInstance();
    printf("------------- Camera Initialized --------------\n");
}

void send_snapshot(httpd* server) {
    pthread_mutex_lock(&buf_mutex);

    pthread_mutex_lock(&rbt_mutex);
        Image* frame = Vision::GetInstance()->GetFrame();
        memcpy(camera_buf->m_ImageData, frame->m_ImageData, frame->m_ImageSize);
    pthread_mutex_unlock(&rbt_mutex);

    assert(camera_buf->m_PixelSize == Image::RGB_PIXEL_SIZE);

    httpdSetContentType(server, "image/jpeg");
    httpdSendHeaders(server);

    size_t size = jpeg_utils::compress_rgb_to_jpeg(camera_buf, send_buf, camera_buf->m_ImageSize, 80);

    httpd_send_data(server, send_buf, size);
    pthread_mutex_unlock(&buf_mutex);
}

}
