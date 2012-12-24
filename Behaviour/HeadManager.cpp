#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <term.h>
#include <ncurses.h>
#include <string>
#include "Behaviour.h"
#include "HeadManager.h"

using namespace Robot;

int HeadManager::m_Pan;
int HeadManager::m_Tilt;

int readbyte(int id) {
	int value;
	if(Behaviour::cm730.ReadByte(id, RX28M::P_TORQUE_ENABLE, &value, 0) == CM730::SUCCESS)
	{
		if(value == 1)
		{
			if(Behaviour::cm730.ReadWord(id, RX28M::P_GOAL_POSITION_L, &value, 0) == CM730::SUCCESS)
				return value;
			else
				return 0;
		}
		else
			return 0;
	}
}

bool HeadManager::ReadMotionData() {
printf("start read\n");
	m_Pan = readbyte(JointData::ID_HEAD_PAN);
printf("success1\n");
	m_Tilt = readbyte(JointData::ID_HEAD_TILT);
printf("success2\n");
	return true;
}

void HeadManager::MoveToImageCenter(Image* camImg, Point2D pos) {
	Point2D center = Point2D(camImg->m_Width/2, camImg->m_Height/2);
	Point2D offset = pos - center;
	offset *= -1; // Inverse X-axis, Y-axis
	offset.X *= (Camera::VIEW_H_ANGLE / (double)camImg->m_Width); // pixel per angle
	offset.Y *= (Camera::VIEW_V_ANGLE / (double)camImg->m_Height); // pixel per angle
	Head::GetInstance()->MoveTracking(offset);
}
