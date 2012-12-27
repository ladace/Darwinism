#include <unistd.h>
#include <libgen.h>

#include "Camera.h"
#include "minIni.h"
#include "LinuxCamera.h"

#define INI_FILE_PATH       "../config.ini"

using namespace Robot;

class Vision
{
	Vision();

	static Vision* m_pVision;
public:
	~Vision();

        static Vision* GetInstance() {
		if (!m_pVision) m_pVision = new Vision();
		return m_pVision;
	}

	Image* GetFrame();
};
