/*
 * main.cpp
 *
 *  Created on: 2011. 1. 4.
 *	  Author: robotis
 */

#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <libgen.h>
#include <iostream>
#include <string>
using namespace std;

#include "mjpg_streamer.h"
#include "LinuxDARwIn.h"
#include "Behaviour.h"


int main(void)
{
    printf("start\n");
    getchar();
    Behaviour::GetInstance();
    printf("start\n");
    for (int i=0;i<100;i++) {
        usleep(10000);
        Behaviour::GetInstance()->Process();
    }
    printf("end\n");
    while (1) {
        //cout << "input cmd" << endl;
        string str;
        cin >> str;
        if (str == "walk") {
            Behaviour::GetInstance()->Walk(Behaviour::STEPPING);
        } else if (str == "stop") {
            Behaviour::GetInstance()->WalkStop();
        } else if (str == "action") {
            int id;
            cin >> id;
            Behaviour::GetInstance()->ActionNext(id);
        } else if (str == "exit") {
            break;
        } else printf("invalid cmd\n");
        for (int i=0;i<100;i++) {
            usleep(10000);
            Behaviour::GetInstance()->Process();
        }
    }
    printf("end\n");
    getchar();
    Walking::GetInstance()->m_Joint.SetEnableBody(false);
    Action::GetInstance()->m_Joint.SetEnableBody(true);
    Action::GetInstance()->Start(15);
    while(Action::GetInstance()->IsRunning() == true) usleep(8000);

    return 0;
}

