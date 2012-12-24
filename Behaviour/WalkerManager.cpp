#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <term.h>
#include <ncurses.h>
#include <string>
#include "WalkerManager.h"

using namespace Robot;

// Position of Row
static const char* WALKING_ARGUMENT_NAME[] {
	"X offset(mm)            ",
	"Y offset(mm)            ",
	"Z offset(mm)            ",
	"Roll(x) offset(degree)  ",
	"Pitch(y) offset(degree) ",
	"Yaw(z) offset(degree)   ",
	"Hip pitch offset(degree)",
	"Period time(msec)       ",
	"DSP ratio               ",
	"Step forward/back ratio ",
	"Step forward/back(mm)   ",
	"Step right/left(mm)     ",
	"Step direction(degree)  ",
	"Foot height(mm)         ",
	"Swing right/left(mm)    ",
	"Swing top/down(mm)      ",
	"Pelvis offset(degree)   ",
	"Arm swing gain          ",
	"P gain                  ",
	"I gain                  ",
	"D gain                  "
};

static const double NORMALIZE[] {
	1,
	1,
	1,
	0.1,
	0.1,
	0.1,
	0.1,
	1,
	0.01,
	0.01,
	1,
	1,
	1,
	1,
	0.1,
	1,
	0.1,
	0.1,
	1,
	1,
	1
};

WalkerManager* WalkerManager::m_UniqueInstance = 0;

WalkerManager::WalkerManager() {
	//MotionManager::GetInstance()->AddModule((MotionModule*)Walking::GetInstance());
	//Walking::GetInstance()->m_Joint.SetEnableBody(false);
	ParSet.clear();
	ParSet.push_back(new double[PARAMETER_NUM]);
	SpeedX=0;
	SpeedY=0;
	SpeedA=0;
	start=false;
	for (int i=0;i<PARAMETER_NUM;i++) {
		ParSet[0][i] = GetParameter(i);
	}
	curParSetID = 0;
}

void WalkerManager::ResetAll() {
	Stop();
	for (int i=0;i<PARAMETER_NUM;i++) {
		GetParameter(i) = ParSet[0][i];
	}
	curParSetID = 0;
	for (int i=1;i<ParSet.size();i++) {
		delete ParSet[i];
	}
	double * defaultSet = ParSet[0];
	ParSet.clear();
	ParSet.push_back(defaultSet);
}

void WalkerManager::Start(int parSetID) {
	LoadParSet(parSetID);
	if (!start) {
		SetSpeed(SpeedX, SpeedY, SpeedA);
		start=true;
	}
	Walking::GetInstance()->Start();
}

void WalkerManager::Stop() {
	if (start) {
		start=false;
		SpeedX = Walking::GetInstance()->X_MOVE_AMPLITUDE;
		SpeedY = Walking::GetInstance()->Y_MOVE_AMPLITUDE;
		SpeedA = Walking::GetInstance()->A_MOVE_AMPLITUDE;
	}
	Walking::GetInstance()->Stop();
}

bool WalkerManager::IsRunning() {
	return Walking::GetInstance()->IsRunning();
}

const double * WalkerManager::GetParSet(int parSetID) {
	if (parSetID>=0 && parSetID<ParSet.size()) {
		return ParSet[parSetID];
	}
	return ParSet[curParSetID];
}

void WalkerManager::LoadParSet(int parSetID) {
	if (parSetID>=0 && parSetID<ParSet.size()) {
		curParSetID = parSetID;
	}
	for (int i=0;i<PARAMETER_NUM;i++) {
		GetParameter(i) = ParSet[curParSetID][i];
	}
}

void WalkerManager::SaveParSet(int parSetID) {
	if (parSetID==ParSet.size()) {
		ParSet.push_back(new double[PARAMETER_NUM]);
	}
	if (parSetID>=1 && parSetID<ParSet.size()) {
		curParSetID = parSetID;
	}
	if (curParSetID>=1) {
		for (int i=0;i<PARAMETER_NUM;i++) {
			ParSet[curParSetID][i] = GetParameter(i);
		}
	}
}

bool WalkerManager::DelParSet(int parSetID) {
	if (parSetID == -1) parSetID = curParSetID;
	if (parSetID>=1 && parSetID<ParSet.size()) {
		delete ParSet[parSetID];
		ParSet.erase(ParSet.begin()+parSetID);
		if (curParSetID == parSetID) curParSetID = 0;
		if (curParSetID >  parSetID) curParSetID--;
		return true;
	}
	return false;
}

void WalkerManager::SetSpeed(int x, int y, int a) {
	SpeedX = Walking::GetInstance()->X_MOVE_AMPLITUDE = x;
	SpeedY = Walking::GetInstance()->Y_MOVE_AMPLITUDE = y;
	SpeedA = Walking::GetInstance()->A_MOVE_AMPLITUDE = a;
}

double WalkerManager::GetParameterValue(int parameterID) {
	return double(GetParameter(parameterID));
}

void WalkerManager::SetParameter(int parameterID, double value) {
	GetParameter(parameterID) = value;
}

void WalkerManager::ChgParameter(int parameterID, double delta) {
	GetParameter(parameterID) += delta;
}

void WalkerManager::ChgParameterNormalization(int parameterID, int delta) {
	GetParameter(parameterID) += delta*NORMALIZE[parameterID];
}

void WalkerManager::PrintParSet() {
	for (int i=0;i<PARAMETER_NUM;i++) {
		printf("%s: %0.2lf\n", WALKING_ARGUMENT_NAME[i], GetParameter(i));
	}
}

void WalkerManager::LoadParSetFile(std::string fileName) {
	FILE* fin = fopen(fileName.c_str(), "r");
	int n=0;
	fscanf(fin, "%d", &n);
	for (int i=0;i<n;i++) {
		double * newparset = new double[PARAMETER_NUM];
		for (int j=0;j<PARAMETER_NUM;j++) fscanf(fin, "%lf", newparset+j);
		ParSet.push_back(newparset);
	}
	fclose(fin);
}

void WalkerManager::SaveParSetFile(std::string fileName) {
	FILE* fout = fopen(fileName.c_str(), "w");
	int n=ParSet.size();
	fprintf(fout, "%d\n", n-1);
	for (int i=1;i<n;i++) {
		for (int j=0;j<PARAMETER_NUM;j++) fprintf(fout, "%0.2lf ", ParSet[i][j]);
		fprintf(fout, "\n");
	}
	fclose(fout);
}

double& WalkerManager::GetParameter(int parameterID) {
	switch(parameterID)
	{
	case X_OFFSET:
		return Walking::GetInstance()->X_OFFSET;
	case Y_OFFSET:
		return Walking::GetInstance()->Y_OFFSET;
	case Z_OFFSET:
		return Walking::GetInstance()->Z_OFFSET;
	case ROLL_OFFSET:
		return Walking::GetInstance()->R_OFFSET;
	case PITCH_OFFSET:
		return Walking::GetInstance()->P_OFFSET;
	case YAW_OFFSET:
		return Walking::GetInstance()->Y_OFFSET;
	case HIP_PITCH_OFFSET:
		return Walking::GetInstance()->HIP_PITCH_OFFSET;
	case PERIOD_TIME:
		return Walking::GetInstance()->PERIOD_TIME;
	case DSP_RATIO:
		return Walking::GetInstance()->DSP_RATIO;
	case STEP_FORWARDBACK_RATIO:
		return Walking::GetInstance()->STEP_FB_RATIO;
	case STEP_FORWARDBACK:
		if (!start) return SpeedX;
		return Walking::GetInstance()->X_MOVE_AMPLITUDE;
	case STEP_RIGHTLEFT:
		if (!start) return SpeedY;
		return Walking::GetInstance()->Y_MOVE_AMPLITUDE;
	case STEP_DIRECTION:
		if (!start) return SpeedA;
		return Walking::GetInstance()->A_MOVE_AMPLITUDE;
	case FOOT_HEIGHT:
		return Walking::GetInstance()->Z_MOVE_AMPLITUDE;
	case SWING_RIGHTLEFT:
		return Walking::GetInstance()->Y_SWAP_AMPLITUDE;
	case SWING_TOPDOWN:
		return Walking::GetInstance()->Z_SWAP_AMPLITUDE;
	case PELVIS_OFFSET:
		return Walking::GetInstance()->PELVIS_OFFSET;
	case ARM_SWING_GAIN:
		return Walking::GetInstance()->ARM_SWING_GAIN;
	}
}


