#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <term.h>
#include <ncurses.h>
#include "Behaviour.h"

using namespace Robot;

#define MOTION_FILE_PATH	"/darwin/Data/motion_4096.bin"

#define WALKER_FILE_PATH	"../walker.ini"

#define U2D_DEV_NAME0	   "/dev/ttyUSB0"
#define U2D_DEV_NAME1	   "/dev/ttyUSB1"

LinuxCM730 linux_cm730(U2D_DEV_NAME0);
CM730 Behaviour::cm730(&linux_cm730);

Behaviour* Behaviour::m_UniqueInstance = 0;

HeadFixed headFixedMode;

Behaviour::Behaviour() {
        printf("=========Behaviour Initializing==========\n");

	if(MotionManager::GetInstance()->Initialize(&cm730) == false) {
		linux_cm730.SetPortName(U2D_DEV_NAME1);
		if(MotionManager::GetInstance()->Initialize(&cm730) == false)
		{
			fprintf(stderr, "Fail to initialize Motion Manager!\n");
			exit(0);
		}
	}

        printf("Motion Manager Initialized.\n");

	MotionManager::GetInstance()->AddModule((MotionModule*)Action::GetInstance());
	MotionManager::GetInstance()->AddModule((MotionModule*)Head::GetInstance());
	MotionManager::GetInstance()->AddModule((MotionModule*)Walking::GetInstance());
	LinuxMotionTimer::Initialize(MotionManager::GetInstance());

        printf("All Motion Modules Initialized.\n");

	int firm_ver = 0;
	if(cm730.ReadByte(JointData::ID_HEAD_PAN, RX28M::P_VERSION, &firm_ver, 0)  != CM730::SUCCESS)
	{
		fprintf(stderr, "Can't read firmware version from Dynamixel ID %d!! \n\n", JointData::ID_HEAD_PAN);
		exit(0);
	}

	if(0 < firm_ver && firm_ver < 27)
	{
		fprintf(stderr, "RX-28M's firmware is not support 4096 resolution!! \n");
		fprintf(stderr, "Upgrade RX-28M's firmware to version 27(0x1B) or higher.\n\n");
		exit(0);
	}
	else if(27 <= firm_ver)
	{
		Action::GetInstance()->LoadFile(MOTION_FILE_PATH);
	}
	else
		exit(0);


	WalkerManager::GetInstance()->LoadParSetFile(WALKER_FILE_PATH);

    Head::GetInstance()->m_Joint.SetEnableHeadOnly(true);
    Walking::GetInstance()->m_Joint.SetEnableBody(false);
    Action::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true);
	MotionManager::GetInstance()->SetEnable(true);
	cm730.WriteByte(CM730::P_LED_PANNEL, 0x01|0x02|0x04, NULL);

	LinuxActionScript::PlayMP3("/darwin/Data/mp3/Demonstration ready mode.mp3");

	m_Real_walking = false;
	m_Real_Acting  = false;
	m_Ctrl_Walking = false;
	m_IsWalkReady  = false;
	m_Next_Action = SIT_DOWN;
	m_Head_Mode = &headFixedMode;
	//headFixedMode.SetAngle(Head::GetInstance()->GetPanAngle(), Head::GetInstance()->GetTiltAngle())
	m_old_btn   = 0;

        printf("---------Behaviour Initialized-----------\n");
}

void Behaviour::CheckButton() {

    if(m_old_btn == MotionStatus::BUTTON)
        return;
    m_old_btn = MotionStatus::BUTTON;
    if(m_old_btn & BTN_MODE)
    {
        fprintf(stderr, "Mode button pressed.. \n");
        //TODO
    }

    if(m_old_btn & BTN_START)
    {
        fprintf(stderr, "Start button pressed.. \n");
        ActionNext(START_UP);
        //TODO
    }
}

bool Behaviour::CheckIsActionWalkReadyAfter(int ActionID) {
	return (ActionID!=STAND_INIT && ActionID!=SIT_DOWN);
}

bool Behaviour::CheckIsActionWalkReadyBefore(int ActionID) {
	return (ActionID!=STAND_INIT && ActionID!=WALK_READY
		 && ActionID!=SIT_DOWN && ActionID!=START_UP);
}

void Behaviour::Walk(int parSetID) {
	if (parSetID>=0) WalkerManager::GetInstance()->LoadParSet(parSetID);
	m_Ctrl_Walking = true;
}

void Behaviour::Walk(int X, int Y, int A) {
	WalkerManager::GetInstance()->LoadParSet(0);
	WalkerManager::GetInstance()->SetSpeed(X,Y,A);
	m_Ctrl_Walking = true;
}

void Behaviour::WalkStop() {
	m_Ctrl_Walking = false;
}

void Behaviour::ActionNext(int actionID) {
	m_Next_Action = actionID;
	m_Ctrl_Walking = false;
}

void Behaviour::ActionStop(bool RightNow) {
	if (RightNow) Action::GetInstance()->Brake();
	Action::GetInstance()->Stop();
}

void Behaviour::Process() {
	CheckButton();
	m_Real_walking = WalkerManager::GetInstance()->IsRunning();
    m_Real_Acting = Action::GetInstance()->IsRunning();

    if (MotionStatus::FALLEN != STANDUP || m_Next_Action != -1) {
    	WalkerManager::GetInstance()->Stop();
        if (m_Real_walking || m_Real_Acting) return;
        if (MotionStatus::FALLEN != STANDUP) {
            Head::GetInstance()->m_Joint.SetEnableBody(false);
            Walking::GetInstance()->m_Joint.SetEnableBody(false);
            Action::GetInstance()->m_Joint.SetEnableBody(true);
            if(MotionStatus::FALLEN == FORWARD)
                Action::GetInstance()->Start(FORW_GETUP);   // FORWARD GETUP
            else if(MotionStatus::FALLEN == BACKWARD)
                Action::GetInstance()->Start(BACK_GETUP);   // BACKWARD GETUP
        	m_IsWalkReady = true;
        } else {
            Head::GetInstance()->m_Joint.SetEnableHeadOnly(true);
            Walking::GetInstance()->m_Joint.SetEnableBody(false);
            Action::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true);
        	if (!m_IsWalkReady && CheckIsActionWalkReadyBefore(m_Next_Action)) {
        		Action::GetInstance()->Start(WALK_READY);
                m_Real_Acting = true;
                m_IsWalkReady = true;
                return;
        	}
    		Action::GetInstance()->Start(m_Next_Action);
		m_IsWalkReady = CheckIsActionWalkReadyAfter(m_Next_Action);
        }
        m_Real_Acting = true;
        m_Next_Action = -1;
        return;
    }

    //if (m_Head_Mode) m_Head_Mode->Process();

    if (m_Real_Acting) return;

    if (!m_Real_walking && m_Ctrl_Walking) {//start Walking
    	if (!m_IsWalkReady) {
    		Action::GetInstance()->Start(WALK_READY);
            m_Real_Acting = true;
            m_IsWalkReady = true;
            return;
    	}
        Head::GetInstance()->m_Joint.SetEnableHeadOnly(true);
        Walking::GetInstance()->m_Joint.SetEnableBodyWithoutHead(true);
        Action::GetInstance()->m_Joint.SetEnableBody(false);
        WalkerManager::GetInstance()->Start();
        return;
    }

    if (m_Real_walking && !m_Ctrl_Walking) {//stop Walking
    	WalkerManager::GetInstance()->Stop();
        return;
    }
}

void Behaviour::SetSpeed(int X, int Y, int A) {
	WalkerManager::GetInstance()->SetSpeed(X,Y,A);
}

void Behaviour::SetSpeedUp(int deltaX, int deltaY, int deltaA) {
	WalkerManager::GetInstance()->ChgParameter(WalkerManager::STEP_FORWARDBACK, deltaX);
	WalkerManager::GetInstance()->ChgParameter(WalkerManager::STEP_RIGHTLEFT, deltaY);
	WalkerManager::GetInstance()->ChgParameter(WalkerManager::STEP_DIRECTION, deltaA);
}

