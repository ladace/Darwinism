#include "actionEditor.h"
#include <iostream>

int main()
{
	LinuxCM730 linux_cm730("/dev/ttyUSB0");
	CM730 cm730(&linux_cm730);
	actionEditor* ae = new actionEditor(&cm730);
	/*
	getchar();
	while(!ae->turnOnOff(0));
	ae->setPage(100);
	getchar();
	while(!ae->turnOnOff(ALL));
	printf("on\n");
	ae->printAll();
	getchar();
	while(!ae->turnOnOff(0));
	printf("off\n");
	getchar();
	printf("input pageNo");
	int pageNo;
	std::cin >> pageNo;
	ae->setPage(pageNo);
	getchar();
	printf("set 7 step\n");
	for(int i = 0; i < 7; i++)
	{
		while(!ae->turnOnOff(0));
		printf("turn off, press any key to form step\n");
		getchar();
		while(!ae->turnOnOff(ALL));
		printf("turn on, press any key\n");
		getchar();
		ae->readStep();
		ae->printAll();
		ae->writeStep(i);
	}
	ae->saveFile();
	ae->turnOnOff(0);
	*/
	getchar();
	ae->setPage(100);

	ae->setStepNum(5);
	char* name = new char[10];
	name[0] = 't';
	name[1] = 'e';
	name[2] = 's';
	name[3] = 't';
	name[4] = 0;
	ae->setPageName(name);
	for(int i = 0; i < 5; i++)
	{
		printf("press to pre form step %d\n", i);
		getchar();
		ae->preFormStep(0);
		printf("press to form step %d\n", i);
		getchar();
		ae->formStep(i, 100, 100);
	}
	ae->saveFile();
	printf("press to play\n");
	getchar();

	ae->play();
	//ae->playStep(2);
	delete ae;
	return 0;
}
