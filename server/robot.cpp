#include <pthread.h>
#include "robot.h"
#include "Behaviour.h"
#include "WalkerManager.h"
#include <cstdlib>
#include <map>

#define CHECK_ROBOT_ON \
if (!robot_on) {\
    httpdOutput(server, "Robot not ON!");\
    return;\
}

extern "C" {
    using Robot::Behaviour;
    using Robot::Walking;
    using Robot::Action;
    using Robot::WalkerManager;

    pthread_t rbt_tid;
    pthread_mutex_t rbt_mutex;

    bool robot_on = false;

    /* Behaviour */

    void start(httpd* server) {
        if (robot_on) {
            httpdOutput(server, "Already started.");
            return;
        }
        pthread_mutex_lock(&rbt_mutex);

        Behaviour::GetInstance();
        robot_on = true;

        pthread_mutex_unlock(&rbt_mutex);
        httpdOutput(server, "start");
    }
    void rest(httpd* server) {
        CHECK_ROBOT_ON 
        pthread_mutex_lock(&rbt_mutex);

        Behaviour::GetInstance()->ActionNext(Behaviour::SIT_DOWN);

        pthread_mutex_unlock(&rbt_mutex);
        httpdOutput(server, "rest");
    }

    void walk(httpd* server) {
        CHECK_ROBOT_ON 
        httpVar* motion_var = httpdGetVariableByName(server, "motion");
        if (motion_var) {
            //now use id
            int id;
            sscanf(motion_var->value, "%d", &id);
            
            pthread_mutex_lock(&rbt_mutex);
            Behaviour::GetInstance()->Walk(id);
            pthread_mutex_unlock(&rbt_mutex);
            httpdOutput(server, "walk - motion: $motion");
        } else {
            pthread_mutex_lock(&rbt_mutex);
            Behaviour::GetInstance()->Walk();
            pthread_mutex_unlock(&rbt_mutex);
            httpdOutput(server, "walk (no motion specified)");
        }
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
            int id;
            if (0 == sscanf(id_var->value, "%d", &id)) {
                httpdOutput(server, "The value is invalid!");
                return;
            }
            pthread_mutex_lock(&rbt_mutex);
            Behaviour::GetInstance()->ActionNext(id);
            pthread_mutex_unlock(&rbt_mutex);
            httpdOutput(server, "Action $id");
        } else
            httpdOutput(server, "Please specify the action ID!");
    }

    /* Walk Tuning */
    struct _param {
        const char* name;
        int id;
    } const param_list[] = {
        { "X-OFFSET"              , WalkerManager::X_OFFSET               },
        { "Y-OFFSET"              , WalkerManager::Y_OFFSET               },
        { "Z-OFFSET"              , WalkerManager::Z_OFFSET               },
        { "ROLL-OFFSET"           , WalkerManager::ROLL_OFFSET            },
        { "PITCH-OFFSET"          , WalkerManager::PITCH_OFFSET           },
        { "YAW-OFFSET"            , WalkerManager::YAW_OFFSET             },
        { "HIP-PITCH-OFFSET"      , WalkerManager::HIP_PITCH_OFFSET       },
        { "PERIOD-TIME"           , WalkerManager::PERIOD_TIME            },
        { "DSP-RATIO"             , WalkerManager::DSP_RATIO              },
        { "STEP-FORWARDBACK-RATIO", WalkerManager::STEP_FORWARDBACK_RATIO },
        { "STEP-FORWARDBACK"      , WalkerManager::STEP_FORWARDBACK       },
        { "STEP-RIGHTLEFT"        , WalkerManager::STEP_RIGHTLEFT         },
        { "STEP-DIRECTION"        , WalkerManager::STEP_DIRECTION         },
        { "FOOT-HEIGHT"           , WalkerManager::FOOT_HEIGHT            },
        { "SWING-RIGHTLEFT"       , WalkerManager::SWING_RIGHTLEFT        },
        { "SWING-TOPDOWN"         , WalkerManager::SWING_TOPDOWN          },
        { "PELVIS-OFFSET"         , WalkerManager::PELVIS_OFFSET          },
        { "ARM-SWING-GAIN"        , WalkerManager::ARM_SWING_GAIN         },
        { "PARAMETER-NUM"         , WalkerManager::PARAMETER_NUM          },
    };
    std::map <std::string, int> param_map;
    void get_walk_param(httpd* server) {
        CHECK_ROBOT_ON
        httpVar* param_var = httpdGetVariableByName(server, "param");
        if (param_var) {
            const char* param_name = param_var->value;
            if (param_map.find(param_name) != param_map.end()) {
                pthread_mutex_lock(&rbt_mutex);
                double v = WalkerManager::GetInstance()->GetParameterValue(param_map[param_name]);
                pthread_mutex_unlock(&rbt_mutex);

                static char buf[50];
                sprintf(buf, "%lf", v);
                httpdOutput(server, buf);
            } else httpdOutput(server, "Parameter $param not found!");
        } else
            httpdOutput(server, "Please specify the param name!");
    }
    void set_walk_param(httpd* server) {
        CHECK_ROBOT_ON
        httpVar *param_var = httpdGetVariableByName(server, "param"),
                *value_var = httpdGetVariableByName(server, "value");

        if (param_var && value_var) {
            double value;
            if (0 == sscanf(value_var->value, "%lf", &value)) {
                httpdOutput(server, "The value is invalid!");
            }

            const char* param_name = param_var->value;
            if (param_map.find(param_name) != param_map.end()) {
                pthread_mutex_lock(&rbt_mutex);
                WalkerManager::GetInstance()->SetParameter(param_map[param_name], value);
                pthread_mutex_unlock(&rbt_mutex);

                httpdOutput(server, "Parameter $param has been set as value.");
            } else httpdOutput(server, "Parameter $param not found!");
        } else
            httpdOutput(server, "Please specify the param name and the value!");
    }

    void initialize() {
        int n_param = sizeof(param_list) / sizeof(_param);
        for (int i = 0; i < n_param; ++i) param_map[param_list[i].name]=param_list[i].id;
    }

    void* robot_func(void*) {
        initialize();
        while (true) {
            if (robot_on) {
                pthread_mutex_lock(&rbt_mutex);
                Behaviour::GetInstance()->Process();
                pthread_mutex_unlock(&rbt_mutex);
            }
            usleep(10000);
        }
        return NULL;
    }
}
