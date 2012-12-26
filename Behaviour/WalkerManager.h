#ifndef _MANAGER_WALKERMANAGER_H_
#define _MANAGER_WALKERMANAGER_H_


#include <vector>
#include "LinuxDARwIn.h"

namespace Robot
{
	class WalkerManager
	{
	public:
		enum {
			X_OFFSET,
			Y_OFFSET,
			Z_OFFSET,
			ROLL_OFFSET,
			PITCH_OFFSET,
			YAW_OFFSET,
			HIP_PITCH_OFFSET,
			PERIOD_TIME,
			DSP_RATIO,
			STEP_FORWARDBACK_RATIO,
			STEP_FORWARDBACK,
			STEP_RIGHTLEFT,
			STEP_DIRECTION,
			FOOT_HEIGHT,
			SWING_RIGHTLEFT,
			SWING_TOPDOWN,
			PELVIS_OFFSET,
			ARM_SWING_GAIN,
			PARAMETER_NUM
		};

	private:
		static WalkerManager* m_UniqueInstance;
		WalkerManager();
		int curParSetID;
		std::vector<double*> ParSet;

		bool start;
		double SpeedX, SpeedY, SpeedA;

	public:
		static WalkerManager* GetInstance() {
			if (!m_UniqueInstance) m_UniqueInstance = new WalkerManager();
			return m_UniqueInstance;
		}

		void ResetAll();
		void Start(int parSetID = -1);//default parameter set is 0
		void Stop();
		bool IsRunning();

		void GetCurParSet(std::vector<double> &result);
		void GetCurParSetNormalization(std::vector<double> &result, std::vector<double> &norResult);

		int GetSetSize();
		void LoadParSet(int parSetID = -1);//default parameter set is 0
		void SaveParSet(int parSetID = -1);//id must not be 0, and if id is the set.size(), then pushback a new int[]
		bool DelParSet(int parSetID = -1);

		void SetSpeed(int x, int y, int a);

		double GetParameterValue(int parameterID);
		void SetParameter(int parameterID, double value);
                void GetParameterRanges(std::vector<double>& min, std::vector<double>& max);
		void ChgParameter(int parameterID, double delta);
		void SetParameterNormalization(int parameterID, double value);//the value must be in [0,1]

		double ChkParameterRange(int parameterID, double value);

		void PrintParSet();

		void LoadParSetFile(std::string fileName);//load size() ~ size()+n-1
		void SaveParSetFile(std::string fileName);//save 1 ~ size()-1

	private:
		double& GetParameter(int parameterID);

	};
}

#endif

