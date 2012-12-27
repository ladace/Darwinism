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

Vision* vision;
extern "C" {

void video_initialize() {
    pthread_mutex_init(&buf_mutex, NULL);

    printf("============= Camera Initializing =============\n");
    vision = new Vision();
    printf("------------- Camera Initialized --------------\n");

    camera_buf = new Image(Camera::WIDTH, Camera::HEIGHT, Image::RGB_PIXEL_SIZE);
    send_buf = (unsigned char*)malloc(Camera::WIDTH * Camera::HEIGHT * Image::RGB_PIXEL_SIZE);
}

void send_snapshot(httpd* server) {
    pthread_mutex_lock(&rbt_mutex);
        printf("prepared to capture\n");
        Image* frame = vision->getFrame();
        printf("captured\n");
        printf("%d", frame->m_ImageSize);
        memcpy(camera_buf->m_ImageData, frame->m_ImageData, frame->m_ImageSize);
    pthread_mutex_unlock(&rbt_mutex);

    assert(camera_buf->m_PixelSize == Image::RGB_PIXEL_SIZE);

    httpdSetContentType(server, "image/jpeg");
    httpdSendHeaders(server);

    printf("prepared to send jpeg\n");
    pthread_mutex_lock(&buf_mutex);

        size_t size = jpeg_utils::compress_rgb_to_jpeg(camera_buf, send_buf, camera_buf->m_ImageSize, 80);

        httpd_send_data(server, send_buf, size);
    pthread_mutex_unlock(&buf_mutex);
}

}
