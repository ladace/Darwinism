#ifndef _MANAGER_HEADMANAGER_H_
#define _MANAGER_HEADMANAGER_H_


#include <stdio.h>
#include "LinuxDARwIn.h"
#include "Kinematics.h"
#include "Point.h"

namespace Robot
{
	class HeadManager
	{
	public:
		HeadManager() {}
		virtual ~HeadManager() {}
		static int m_Pan, m_Tilt;
		static bool ReadMotionData();
		static void MoveToImageCenter(Image* camImg, Point2D pos);
		virtual void Process() = 0;
	};

	class HeadFixed : public HeadManager
	{
	public:
		HeadFixed() {
			this->pan  = 0;
			this->tilt = Kinematics::EYE_TILT_OFFSET_ANGLE - 30.0;
		}
		void SetAngle(double pan, double tilt) {
			this->pan  = pan;
			this->tilt = tilt;
		}
		void Process() {
			//ReadMotionData();
			printf("pan = %lf, tilt = %lf\n", m_Pan, m_Tilt);
			Head::GetInstance()->MoveByAngle(pan, tilt);
		}

		double pan, tilt;
	};
}

#endif

