#include <pthread.h>
#include "robot.h"
#include "Behaviour.h"
#include "WalkerManager.h"
#include <cstdlib>
#include <map>
#include <sstream>

/* Helper function */
int get_param_int(httpd* server, const char* name, int* p) {
    httpVar* var = httpdGetVariableByName(server, const_cast<char*>(name));
    if (!var) {
        return 0;
    }
    if (sscanf(var->value, "%d", p) == 0)
        return -1;
    return 1;
}

extern "C" {
    using Robot::Behaviour;
    using Robot::Walking;
    using Robot::Action;
    using Robot::WalkerManager;

    pthread_t rbt_tid;
    pthread_mutex_t rbt_mutex;

    /* Behaviour */

    void rest(httpd* server) {
        pthread_mutex_lock(&rbt_mutex);
            Behaviour::GetInstance()->ActionNext(Behaviour::SIT_DOWN);
        pthread_mutex_unlock(&rbt_mutex);

        httpdOutput(server, "rest");
    }

    void walk(httpd* server) {
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

    void walk_stop(httpd* server) {
        pthread_mutex_lock(&rbt_mutex);
            Behaviour::GetInstance()->WalkStop();
        pthread_mutex_unlock(&rbt_mutex);

        httpdOutput(server, "stopped");
    }

    void action(httpd* server) {
        int id;
        int ret = get_param_int(server, "id", &id);
        if (ret == 1) {
            pthread_mutex_lock(&rbt_mutex);
                Behaviour::GetInstance()->ActionNext(id);
            pthread_mutex_unlock(&rbt_mutex);

            httpdOutput(server, "Action $id");
        } else {
            if (ret == 0)
                httpdOutput(server, "Please specify the action ID!");
            else
                httpdOutput(server, "The value is invalid!");
        }
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
    };

    typedef std::map<std::string, int> param_map_t;
    param_map_t param_map;

    void walk_get_param(httpd* server) {
        httpVar* param_var = httpdGetVariableByName(server, "param");
        if (param_var) {
            const char* param_name = param_var->value;

            param_map_t::iterator it = param_map.find(param_name);
            if (it != param_map.end()) {
                pthread_mutex_lock(&rbt_mutex);
                    double v = WalkerManager::GetInstance()->GetParameterValue(it->second);
                pthread_mutex_unlock(&rbt_mutex);

                static char buf[50];
                sprintf(buf, "%lf", v);
                httpdOutput(server, buf);
            } else httpdOutput(server, "Parameter $param not found!");
        } else
            httpdOutput(server, "Please specify the param name!");
    }

    void walk_set_param(httpd* server) {
        httpVar *param_var = httpdGetVariableByName(server, "param"),
                *value_var = httpdGetVariableByName(server, "value");

        if (param_var && value_var) {
            double value;
            if (0 == sscanf(value_var->value, "%lf", &value)) {
                httpdOutput(server, "The value is invalid!");
            }

            const char* param_name = param_var->value;

            param_map_t::iterator it = param_map.find(param_name);
            if (it != param_map.end()) {
                pthread_mutex_lock(&rbt_mutex);
                    WalkerManager::GetInstance()->SetParameter(it->second, value);
                    //TODO save parset file?
                pthread_mutex_unlock(&rbt_mutex);

                httpdOutput(server, "Parameter $param has been set as $value.");
            } else httpdOutput(server, "Parameter $param not found!");
        } else
            httpdOutput(server, "Please specify the param name and the value!");
    }

    void walk_load_parset(httpd* server) {
        int id;
        int ret = get_param_int(server, "id", &id);
        if (ret == 1) {
            pthread_mutex_lock(&rbt_mutex);
                WalkerManager::GetInstance()->LoadParSet(id);
            pthread_mutex_unlock(&rbt_mutex);

            httpdOutput(server, "ParSet loaded");
        } else {
            if (ret == 0) {
                pthread_mutex_lock(&rbt_mutex);
                    WalkerManager::GetInstance()->LoadParSet();
                pthread_mutex_unlock(&rbt_mutex);

                walk_get_cur_parset(server); // output the parameters
            } else
                httpdOutput(server, "ID is invalid!");
        }
    }

    void walk_save_parset(httpd* server) {
        int id;
        int ret = get_param_int(server, "id", &id);

        if (ret == 1) {
            pthread_mutex_lock(&rbt_mutex);
                WalkerManager::GetInstance()->SaveParSet(id);
            pthread_mutex_unlock(&rbt_mutex);

            httpdOutput(server, "ParSet saved");
        } else {
            if (ret == 0) {
                pthread_mutex_lock(&rbt_mutex);
                    WalkerManager::GetInstance()->SaveParSet();
                pthread_mutex_unlock(&rbt_mutex);

                httpdOutput(server, "ParSet saved");
            } else
                httpdOutput(server, "ID is invalid!");
        }
    }
    void walk_save_new_parset(httpd* server) {
        pthread_mutex_lock(&rbt_mutex);
            WalkerManager::GetInstance()->SaveParSet(WalkerManager::GetInstance()->GetSetSize());
        pthread_mutex_unlock(&rbt_mutex);

        httpdOutput(server, "ParSet saved");
    }

    void walk_del_parset(httpd* server) {
        int id;
        int ret = get_param_int(server, "id", &id);

        if (ret == 1) {
            pthread_mutex_lock(&rbt_mutex);
                if (WalkerManager::GetInstance()->DelParSet(id))
                    httpdOutput(server, "ParSet deleted");
                else
                    httpdOutput(server, "Failed to delete ParSet!");
            pthread_mutex_unlock(&rbt_mutex);
        } else {
            if (ret == 0) {
                pthread_mutex_lock(&rbt_mutex);
                    if (WalkerManager::GetInstance()->DelParSet())
                        httpdOutput(server, "ParSet deleted");
                    else
                        httpdOutput(server, "Failed to delete ParSet!");
                pthread_mutex_unlock(&rbt_mutex);
            } else
                httpdOutput(server, "ID is invalid!");
        }
    }

    void walk_start(httpd* server) {
        int id;
        int ret = get_param_int(server, "id", &id);
        if (ret == 1) {
            pthread_mutex_lock(&rbt_mutex);
                WalkerManager::GetInstance()->LoadParSet(id);
                Behaviour::GetInstance()->Walk();
            pthread_mutex_unlock(&rbt_mutex);

            httpdOutput(server, "started to walk: $id");
        } else {
            if (ret == 0) {
                pthread_mutex_lock(&rbt_mutex);
                    Behaviour::GetInstance()->Walk();
	        pthread_mutex_unlock(&rbt_mutex);

                httpdOutput(server, "started to walk");
            } else
                httpdOutput(server, "ID is invalid!");
        }
    }

    void walk_get_cur_parset(httpd* server) {
        int id;
        std::vector<double> pars;

        pthread_mutex_lock(&rbt_mutex);
            WalkerManager::GetInstance()->GetCurParSet(pars);
        pthread_mutex_unlock(&rbt_mutex);

        std::ostringstream os;
        os << '[';
        for (std::vector<double>::iterator it = pars.begin(); it != pars.end(); ++it) {
            os << *it << ',';
        }
        os << ']';
        httpdOutput(server, const_cast<char*>(os.str().c_str()));
    }

    void walk_get_cur_parset_norm(httpd* server) {
        int id;
        std::vector<double> pars, pars_norm;
        pthread_mutex_lock(&rbt_mutex);
            WalkerManager::GetInstance()->GetCurParSetNormalization(pars, pars_norm);
        pthread_mutex_unlock(&rbt_mutex);

        std::ostringstream os;
        os << "[[";
        for (std::vector<double>::iterator it = pars.begin(); it != pars.end(); ++it)
            os << *it << ',';
        os << "],[";
        for (std::vector<double>::iterator it = pars_norm.begin(); it != pars_norm.end(); ++it)
            os << *it << ',';
        os << "]]";
        httpdOutput(server, const_cast<char*>(os.str().c_str()));
    }

    void walk_get_parset_num(httpd* server) {
        pthread_mutex_lock(&rbt_mutex);
            int n = WalkerManager::GetInstance()->GetSetSize();
        pthread_mutex_unlock(&rbt_mutex);
        std::ostringstream os;
        os << n;
        httpdOutput(server, const_cast<char*>(os.str().c_str()));
    }

    void walk_get_par_minmax(httpd* server) {
        int id;
        std::vector<double> min, max;
        pthread_mutex_lock(&rbt_mutex);
            WalkerManager::GetInstance()->GetParameterRanges(min, max);
        pthread_mutex_unlock(&rbt_mutex);

        std::ostringstream os;
        os << "[[";
        for (std::vector<double>::iterator it = min.begin(); it != min.end(); ++it)
            os << *it << ',';
        os << "],[";
        for (std::vector<double>::iterator it = max.begin(); it != max.end(); ++it)
            os << *it << ',';
        os << "]]";
        httpdOutput(server, const_cast<char*>(os.str().c_str()));
    }

    //walk_stop is defined above

    void initialize() {
        Behaviour::GetInstance(); //Initialize the robot
        int n_param = sizeof(param_list) / sizeof(_param);
        for (int i = 0; i < n_param; ++i) param_map[param_list[i].name]=param_list[i].id;
    }

    void* robot_func(void*) {
        initialize();
        while (true) {
            pthread_mutex_lock(&rbt_mutex);
                Behaviour::GetInstance()->Process();
            pthread_mutex_unlock(&rbt_mutex);
            usleep(10000);
        }
        return NULL;
    }
}
