#include <pthread.h>
#include "robot.h"
#include "Behaviour.h"
#include <cstdlib>

#define CHECK_ROBOT_ON \
if (!robot_on) {\
    httpdOutput(server, "Robot not ON!");\
    return;\
}

extern "C" {
    using Robot::Behaviour;
    using Robot::Walking;
    using Robot::Action;

    pthread_t rbt_tid;
    pthread_mutex_t rbt_mutex;

    bool robot_on = false;
    bool robot_resting = false;

    void start(httpd* server) {
        if (robot_on) {
            httpdOutput(server, "Already started.");
            return;
        }
        pthread_mutex_lock(&rbt_mutex);

        Behaviour::GetInstance()->ActionNext(9);
        robot_on = true;

        pthread_mutex_unlock(&rbt_mutex);
        httpdOutput(server, "start");
    }
    void rest(httpd* server) {
        CHECK_ROBOT_ON 
        pthread_mutex_lock(&rbt_mutex);

        Walking::GetInstance()->m_Joint.SetEnableBody(false);
        Action::GetInstance()->m_Joint.SetEnableBody(true);
        Action::GetInstance()->Start(15);

        robot_resting = true;
        robot_on = false;

        pthread_mutex_unlock(&rbt_mutex);
        httpdOutput(server, "rest");
    }

    void walk(httpd* server) {
        CHECK_ROBOT_ON 
        pthread_mutex_lock(&rbt_mutex);
        Behaviour::GetInstance()->Walk(Behaviour::STEPPING);
        pthread_mutex_unlock(&rbt_mutex);
        httpdOutput(server, "walk");
    }

    void walkstop(httpd* server) {
        CHECK_ROBOT_ON 
        pthread_mutex_lock(&rbt_mutex);
        Behaviour::GetInstance()->WalkStop();
        pthread_mutex_unlock(&rbt_mutex);
        httpdOutput(server, "walkstop");
    }

    void action(httpd* server) {
        CHECK_ROBOT_ON 
        httpVar* id_var = httpdGetVariableByName(server, "id");//TODO
        if (id_var) {
            int id = atoi(id_var->value);
            pthread_mutex_lock(&rbt_mutex);
            Behaviour::GetInstance()->ActionNext(id);
            pthread_mutex_unlock(&rbt_mutex);
            httpdOutput(server, "Action $id");
        } else
            httpdOutput(server, "Please specify the action ID!");
    }


    void* robot_func(void*) {
        while (true) {
            if (robot_on) {
                pthread_mutex_lock(&rbt_mutex);
                Behaviour::GetInstance()->Process();
                pthread_mutex_unlock(&rbt_mutex);
            }
            if (robot_resting) {
                pthread_mutex_lock(&rbt_mutex);
                while(Action::GetInstance()->IsRunning() == true) usleep(8000);
                robot_resting = false;
                pthread_mutex_unlock(&rbt_mutex);
            }
            usleep(10000);
        }
        return NULL;
    }
}
