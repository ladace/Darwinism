#include "LinuxDARwIn.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <term.h>
#include <fcntl.h>
#include <ncurses.h>

using namespace Robot;

#define R_SHOULDER_PITCH     0x00000001
#define L_SHOULDER_PITCH     0x00000002
#define R_SHOULDER_ROLL      0x00000004
#define L_SHOULDER_ROLL      0x00000008
#define R_ELBOW              0x00000010
#define L_ELBOW              0x00000020
#define R_HIP_YAW            0x00000040
#define L_HIP_YAW            0x00000080
#define R_HIP_ROLL           0x00000100
#define L_HIP_ROLL           0x00000200
#define R_HIP_PITCH          0x00000400
#define L_HIP_PITCH          0x00000800
#define R_KNEE               0x00001000
#define L_KNEE               0x00002000
#define R_ANKLE_PITCH        0x00004000
#define L_ANKLE_PITCH        0x00008000
#define R_ANKLE_ROLL         0x00010000
#define L_ANKLE_ROLL         0x00020000
#define HEAD_PAN             0x00040000
#define HEAD_TILT            0x00080000
#define ALL					  0xffffffff

class actionEditor
{
private:

	CM730* cm730;
	bool inited;
	Action::PAGE page;
	Action::STEP step;
	//int stepNo;
	int indexPage;

public:
	actionEditor(CM730* cm730);//初始化
	~actionEditor();//析构函数
	bool turnOnOff(int flags);//开启、关闭电机
	void printValue();//打印所有电机值
	bool setValue(int value, int flags);//修改当前step对应电机值
	int getValue(int flags);//获取当前step对应电机值
	bool loadFile(char* filename);//载入文件
	bool saveFile();//存储文件
	bool writeStep(int stepNo);//写当前step到page的第stepNo帧中
	void printAll();
	bool setPage(int index);
	bool readStep();//将电机当前位置读入step中
	bool pageToStep(int stepNo);//将page的第stepNo帧写入当前step
	void play();
	bool setStepNum(int stepNum);
	bool setPageLink(int pageLink);
	bool setStepTime(int stepTime, int stepNo);
	bool setStepPause(int stepPaulse, int stepNo);
	bool setPageName(char* pageName);
	bool preFormStep(int flags);
	bool formStep(int stepNo, int stepTime, int stepPause);
	bool playStep(int stepNo);
};
