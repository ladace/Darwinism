#ifndef _MANAGER_BEHAVIOUR_H_
#define _MANAGER_BEHAVIOUR_H_


#include <vector>
#include "LinuxDARwIn.h"

#include "WalkerManager.h"
#include "HeadManager.h"
#include "MotionStatus.h"

namespace Robot
{
	class Behaviour
	{
	public:
		enum {//ActionID
			STAND_INIT	= 1 ,
			WALK_READY	= 9 ,
			FORW_GETUP	= 10,
			BACK_GETUP	= 11,
			RIGHT_KICK	= 12,
			LEFT_KICK	= 13,
			SIT_DOWN	= 15,
			START_UP	= 16,
			MULT_ACTION	= 17,
			RIGHT_PASS	= 70,
			LEFT_PASS	= 71,
		};

		enum {//WalkingID
			DEFAULT		= 0 ,
			STEPPING	= 1 ,
			WALK_FORW	= 2 ,
			WALK_BACK	= 3 ,
			WALK_LEFT	= 4 ,
			WALK_RIGHT	= 5 ,
			TURN_LEFT	= 6 ,
			TURN_RIGHT	= 7
		};
		enum {//BUTTONID
			BTN_MODE = 1,
			BTN_START = 2
		};

	private:
		static Behaviour* m_UniqueInstance;
		Behaviour();
		void CheckButton();
		bool CheckIsActionWalkReadyAfter(int ActionID);
		bool CheckIsActionWalkReadyBefore(int ActionID);
		bool m_Real_Acting;
		int  m_Next_Action;
		bool m_Real_walking;
		bool m_Ctrl_Walking;
		bool m_IsWalkReady;
		HeadManager * m_Head_Mode;

		int m_old_btn;

	public:
		static CM730 cm730;
		static Behaviour* GetInstance() {
			if (!m_UniqueInstance) m_UniqueInstance = new Behaviour();
			return m_UniqueInstance;
		}

		void Walk(int WalkingID = -1);
		void Walk(int X, int Y, int A);//use the default ParSet with speed
		void WalkStop();

		void ActionNext(int ActionID);
		void ActionStop(bool RightNow = false);

		void Process();

		void SetSpeed(int X, int Y, int A);//use the current ParSet with speed
		void SetSpeedUp(int deltaX, int deltaY, int deltaA);//use the current ParSet with speed Delta
	};
}

#endif

