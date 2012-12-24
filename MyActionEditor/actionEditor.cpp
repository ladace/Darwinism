#include "actionEditor.h"


#ifdef RX28M_1024
#define MOTION_FILE_PATH    "../../../Data/motion_1024.bin"
#else
#define MOTION_FILE_PATH    "../../../Data/motion_4096.bin"
#endif

void sighandler(int sig)
{
	struct termios term;
	tcgetattr( STDIN_FILENO, &term );
	term.c_lflag |= ICANON | ECHO;
	tcsetattr( STDIN_FILENO, TCSANOW, &term );

	exit(0);
}

int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

actionEditor::actionEditor(CM730* cm730)
{
	//MotionManager::GetInstance()->AddModule((MotionModule*)Action::GetInstance());
	//Action::GetInstance()->m_Joint.SetEnableBody(false);
	this->cm730 = cm730;
	indexPage = 15;
	char* filename = new char[100];
	signal(SIGABRT, &sighandler);
	signal(SIGTERM, &sighandler);
	signal(SIGQUIT, &sighandler);
	signal(SIGINT, &sighandler);
	strcpy(filename, MOTION_FILE_PATH);
	if(Action::GetInstance()->LoadFile(filename) == false)
	{
		if(Action::GetInstance()->CreateFile(filename) == false)
		{
			inited = false;
			printf("Fail to create %s\n", filename);
			exit(0);
		}
	}
	if(MotionManager::GetInstance()->Initialize(cm730) == false)
	{
		inited = false;
		printf("Initializing Motion Manager failed!\n");
		exit(0);
	}
	MotionManager::GetInstance()->AddModule((MotionModule*)Action::GetInstance());
	LinuxMotionTimer::Initialize(MotionManager::GetInstance());
	LinuxMotionTimer::Stop();
	Action::GetInstance()->LoadPage(indexPage, &page);
	pageToStep(0);
	play();
	inited = true;
}

actionEditor::~actionEditor()
{
}

void actionEditor::play()
{
	int value;

	for(int i=0; i<page.header.stepnum; i++)
	{
		for(int id=JointData::ID_R_SHOULDER_PITCH; id<JointData::NUMBER_OF_JOINTS; id++)
		{
			if(page.step[i].position[id] & Action::INVALID_BIT_MASK)
			{
				printf("Exist invalid joint value");
				return;
			}
		}
	}

	for(int id=JointData::ID_R_SHOULDER_PITCH; id<JointData::NUMBER_OF_JOINTS; id++)
	{
		if(cm730->ReadByte(id, RX28M::P_TORQUE_ENABLE, &value, 0) == CM730::SUCCESS)
		{
			if(value == 0)
			{
				if(cm730->ReadWord(id, RX28M::P_PRESENT_POSITION_L, &value, 0) == CM730::SUCCESS)
					MotionStatus::m_CurrentJoints.SetValue(id, value);
			}
			else
			{
				if(cm730->ReadWord(id, RX28M::P_GOAL_POSITION_L, &value, 0) == CM730::SUCCESS)
					MotionStatus::m_CurrentJoints.SetValue(id, value);
			}
		}
	}
	printf("Playing... ('s' to stop, 'b' to brake)");
	LinuxMotionTimer::Start();
	MotionManager::GetInstance()->SetEnable(true);
	if(Action::GetInstance()->Start(indexPage, &page) == false)
	{
		printf("Failed to play this page!");
		MotionManager::GetInstance()->SetEnable(false);
		return;
	}
	//set_stdin();
	while(1)
	{
		if(Action::GetInstance()->IsRunning() == false)
			break;
		if(kbhit())
		{
			int key = getchar();
			if(key == 's')
			{
				Action::GetInstance()->Stop();
				printf("\r] Stopping...                                  ");
			}
			else if(key == 'b')
			{
				Action::GetInstance()->Brake();
				printf("\r] Braking...                                   ");
			}
			else
				printf("\r] Playing... ('s' to stop, 'b' to brake)");
		}
		usleep(10000);
	}
	//reset_stdin();

	MotionManager::GetInstance()->SetEnable(false);
	LinuxMotionTimer::Stop();
}

bool actionEditor::loadFile(char* filename)
{
	if(Action::GetInstance()->LoadFile(filename) == false)
	{
		if(Action::GetInstance()->CreateFile(filename) == false)
		{
			return false;
		}
	}
	if(MotionManager::GetInstance()->Initialize(cm730) == false)
	{
		return false;
	}
	Action::GetInstance()->ResetPage(&page);
	return true;
}

bool actionEditor::saveFile()
{
	if(Action::GetInstance()->SavePage(indexPage, &page) == true)
		return true;
	else
		return false;
}

bool actionEditor::pageToStep(int stepNo)
{
	if(stepNo >= 0 && stepNo < Action::MAXNUM_STEP)
	{
		for(int id=0; id<31; id++)
		{
			if(id >= JointData::ID_R_SHOULDER_PITCH && id <= JointData::ID_HEAD_TILT)
			{
				step.position[id] = page.step[stepNo].position[id];
			}
		}
		return true;
	}
	else
		return false;
}

bool actionEditor::writeStep(int stepNo) {
	for(int id=0; id<31; id++)
	{
		if(id >= JointData::ID_R_SHOULDER_PITCH && id <= JointData::ID_HEAD_TILT)
		{
			page.step[stepNo].position[id] = step.position[id];
		}
	}
	return true;
}

bool actionEditor::readStep()
{
	int value;
	for(int id=0; id<31; id++)
	{
		if(id >= JointData::ID_R_SHOULDER_PITCH && id <= JointData::ID_HEAD_TILT)
		{
			if(cm730->ReadByte(id, RX28M::P_TORQUE_ENABLE, &value, 0) == CM730::SUCCESS)
			{
				if(value == 1)
				{
					if(cm730->ReadWord(id, RX28M::P_GOAL_POSITION_L, &value, 0) == CM730::SUCCESS)
						step.position[id] = value;
					else
						step.position[id] = Action::INVALID_BIT_MASK;
				}
				else
					step.position[id] = Action::TORQUE_OFF_BIT_MASK;
			}
			else
				step.position[id] = Action::INVALID_BIT_MASK;
		}
		else
			step.position[id] = Action::INVALID_BIT_MASK;
	}
	return true;
}

bool actionEditor::turnOnOff(int flags)
{
	printf("flags %d\n",flags);
	for(int id = JointData::ID_R_SHOULDER_PITCH; id < JointData::NUMBER_OF_JOINTS; id++)
	{
		//printf("flags %d %d \n", id, (flags >> (id-1) & 1));
		if((flags >> (id-1) & 1) == 0) {
			if(cm730->WriteByte(id, RX28M::P_TORQUE_ENABLE, 0, 0) != CM730::SUCCESS)
			{
				printf("id %d failed to turn off\n", id);
				return false;
			}
		}
		else
			if(cm730->WriteByte(id, RX28M::P_TORQUE_ENABLE, 1, 0) != CM730::SUCCESS)
			{
				printf("id %d failed to turn on\n", id);
				return false;
			}
	}
	printf("on off successful\n");
	return true;
}

int actionEditor::getValue(int flags)
{
	for(int id = JointData::ID_R_SHOULDER_PITCH; id < JointData::NUMBER_OF_JOINTS; id++)
	{
		if((flags >> (id-1) & 1) == 1) {
			return step.position[id];
		}
	}
	return 0;
}

bool actionEditor::setValue(int value, int flags)
{
	for(int id = JointData::ID_R_SHOULDER_PITCH; id < JointData::NUMBER_OF_JOINTS; id++)
	{
		if((flags >> (id-1) & 1) == 1) {
			if(value >= 0 && value <= RX28M::MAX_VALUE)
				{
					if(!(step.position[id] & Action::INVALID_BIT_MASK) && !(step.position[id] & Action::TORQUE_OFF_BIT_MASK))
					{
						int error;
						if(cm730->WriteWord(id, RX28M::P_GOAL_POSITION_L, value, &error) == CM730::SUCCESS)
						{
							if(!(error & CM730::ANGLE_LIMIT))
							{
								step.position[id] = value;
								return true;
							}
						}
					}
				}
		}
	}
	return false;
}
void actionEditor::printAll()
{
	int value;
	for(int id = JointData::ID_R_SHOULDER_PITCH; id < JointData::NUMBER_OF_JOINTS; id++)
	{
		printf("id%d:%.3d\n", id, step.position[id]);
		cm730->ReadWord(id, RX28M::P_GOAL_POSITION_L, &value, 0);
		printf("id%d:%.3d\n", id, value);
	}
	
}

bool actionEditor::setPage(int index)
{
	if(index > 0 && index < Action::MAXNUM_PAGE)
	{
		indexPage = index;
		Action::GetInstance()->LoadPage(indexPage, &page);
		pageToStep(0);
		return true;
	}
	else
		return false;
}

bool actionEditor::setStepNum(int stepNum)
{
	if(stepNum > 0 && stepNum <= Action::MAXNUM_STEP)
	{
		page.header.stepnum = stepNum;
		return true;
	}
	else
		return false;
}

bool actionEditor::setPageLink(int pageLink)
{
	page.header.next = pageLink;
	return true;
}

bool actionEditor::setStepTime(int stepTime, int stepNo)
{
	if(stepNo >= 0 && stepNo < Action::MAXNUM_STEP)
	{
		page.step[stepNo].time = stepTime;
		return true;
	}
	else
		return false;
}

bool actionEditor::setStepPause(int stepPause, int stepNo)
{
	if(stepNo >= 0 && stepNo < Action::MAXNUM_STEP)
		{
			page.step[stepNo].pause = stepPause;
			return true;
		}
		else
			return false;
}

bool actionEditor::setPageName(char* pageName)
{
	for(int i=0; i<=Action::MAXNUM_NAME; i++)
	{
		if(pageName[i] == 0) break;
		page.header.name[i] = pageName[i];
	}
	return true;
}

bool actionEditor::preFormStep(int flags)
{
	while(!turnOnOff(flags));
	return true;
}

bool actionEditor::formStep(int stepNo, int stepTime, int stepPause)
{
	while(!turnOnOff(ALL));
	readStep();
	writeStep(stepNo);
	page.step[stepNo].time = stepTime;
	page.step[stepNo].pause = stepPause;
	return true;
}

bool actionEditor::playStep(int stepNo)
{
	Action::PAGE tmpPage;
	if(stepNo < page.header.stepnum)
	{

		Action::GetInstance()->ResetPage(&tmpPage);
		tmpPage.header.stepnum = 1;
		for(int id = JointData::ID_R_SHOULDER_PITCH; id < JointData::NUMBER_OF_JOINTS; id++)
		{
			tmpPage.step[0].position[id] = page.step[stepNo].position[id];
		}
		if(page.step[stepNo].time > 0)
		{
			tmpPage.step[0].time = page.step[stepNo].time;
			tmpPage.step[0].pause = page.step[stepNo].pause;
		}
		else
		{
			tmpPage.step[0].time = 150;
			tmpPage.step[0].pause = 50;
		}
	}

	printf("Playing... ('s' to stop, 'b' to brake)");
	LinuxMotionTimer::Start();
	MotionManager::GetInstance()->SetEnable(true);
	if(Action::GetInstance()->Start(200, &tmpPage) == false)
	{
		printf("Failed to play this page!");
		MotionManager::GetInstance()->SetEnable(false);
		return false;
	}
	//set_stdin();
	while(1)
	{
		if(Action::GetInstance()->IsRunning() == false)
			break;
		if(kbhit())
		{
			int key = getchar();
			if(key == 's')
			{
				Action::GetInstance()->Stop();
				printf("\r] Stopping...                                  ");
			}
			else if(key == 'b')
			{
				Action::GetInstance()->Brake();
				printf("\r] Braking...                                   ");
			}
			else
				printf("\r] Playing... ('s' to stop, 'b' to brake)");
			}
	usleep(10000);
	}
	//reset_stdin();
	printf("done\n");
	MotionManager::GetInstance()->SetEnable(false);
	LinuxMotionTimer::Stop();
	return true;
}
