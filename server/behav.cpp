#include <pthread.h>
#include "behav.h"
#include "Behaviour.h"

extern "C" {
    using Robot::Behaviour;
    using Robot::Walking;
    using Robot::Action;

    pthread_t beh_tid;
    pthread_mutex_t beh_mutex;

    bool robot_on = false;
    bool robot_resting = false;

    void start(httpd* server) {
        if (robot_on) return;
        pthread_mutex_lock(&beh_mutex);
        Behaviour::GetInstance();
        pthread_mutex_unlock(&beh_mutex);
    }
    void end(httpd* server) {
        if (!robot_on) return;
        pthread_mutex_lock(&beh_mutex);

        Walking::GetInstance()->m_Joint.SetEnableBody(false);
        Action::GetInstance()->m_Joint.SetEnableBody(true);
        Action::GetInstance()->Start(15);

        robot_resting = true;
        robot_on = false;

        pthread_mutex_unlock(&beh_mutex);
    }

    void walk(httpd* server) {
        pthread_mutex_lock(&beh_mutex);
        Behaviour::GetInstance()->Walk(Behaviour::STEPPING);
        pthread_mutex_unlock(&beh_mutex);
        httpdOutput(server, "walk");
    }

    void walkstop(httpd* server) {
        pthread_mutex_lock(&beh_mutex);
        Behaviour::GetInstance()->WalkStop();
        pthread_mutex_unlock(&beh_mutex);
    }

    void action(httpd* server) {
        int id = 0;//TODO
        pthread_mutex_lock(&beh_mutex);
        Behaviour::GetInstance()->ActionNext(id);
        pthread_mutex_unlock(&beh_mutex);
    }


    void* behaviour_func(void*) {
        while (true) {
            if (robot_on) {
                pthread_mutex_lock(&beh_mutex);
                Behaviour::GetInstance()->Process();
                pthread_mutex_unlock(&beh_mutex);
            }
            if (robot_resting) {
                pthread_mutex_lock(&beh_mutex);
                while(Action::GetInstance()->IsRunning() == true) usleep(8000);
                pthread_mutex_unlock(&beh_mutex);
            }
            usleep(10000);
        }
    }
}
