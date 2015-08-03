﻿#include <Platform.h>

#ifdef PLATFORM_IS_LINUX
#include <Aris_Control.h>
#endif

#include <Aris_ControlData.h>
#include <Aris_Socket.h>
#include <Robot_Gait.h>
#include <HexapodIII.h>
#include <string>
#include <sstream>
#include <map>

#include <memory>

namespace Robots
{
	typedef std::function<Aris::Core::MSG(const std::string &cmd, const std::map<std::string, std::string> &params)> PARSE_FUNC;
	
	enum ROBOT_CMD_ID
	{
		ENABLE,
		DISABLE,
		HOME,
		RESET_ORIGIN,
		RUN_GAIT,

		ROBOT_CMD_COUNT
	};
	
	class ROBOT_SERVER
	{
	public:
		static ROBOT_SERVER * GetInstance()
		{
			static ROBOT_SERVER instance;
			return &instance;
		}

		void LoadXml(const char *fileName);
		void AddGait(std::string cmdName, GAIT_FUNC gaitFunc, PARSE_FUNC parseFunc);
		void Start();

	private:
		ROBOT_SERVER() = default;
		ROBOT_SERVER(const ROBOT_SERVER&) = delete;

		void DecodeMsg(const Aris::Core::MSG &msg, std::string &cmd, std::map<std::string, std::string> &params);
		void GenerateCmdMsg(const std::string &cmd, const std::map<std::string, std::string> &params, Aris::Core::MSG &msg);
		void ExecuteMsg(const Aris::Core::MSG &m);
		
		void inline p2a(const int *phy, int *abs, int num = 18)
		{
			for (int i = 0; i<num; ++i)
			{
				abs[i] = mapPhy2Abs[phy[i]];
			}
		}
		void inline a2p(const int *abs, int *phy, int num = 18)
		{
			for (int i = 0; i<num; ++i)
			{
				phy[i] = mapAbs2Phy[abs[i]];
			}
		}

		int home(Robots::ROBOT_BASE *pRobot, const Robots::GAIT_PARAM_BASE *param, Aris::RT_CONTROL::CMachineData &data);
		int enable(Robots::ROBOT_BASE *pRobot, const Robots::GAIT_PARAM_BASE *param, Aris::RT_CONTROL::CMachineData &data);
		int disable(Robots::ROBOT_BASE *pRobot, const Robots::GAIT_PARAM_BASE *param, Aris::RT_CONTROL::CMachineData &data);
		int resetOrigin(Robots::ROBOT_BASE *pRobot, const Robots::GAIT_PARAM_BASE *param, Aris::RT_CONTROL::CMachineData &data);
		int runGait(Robots::ROBOT_BASE *pRobot, const Robots::GAIT_PARAM_BASE *pParam, Aris::RT_CONTROL::CMachineData &data);

		int execute_cmd(int count, char *cmd, Aris::RT_CONTROL::CMachineData &data);
		static int tg(Aris::RT_CONTROL::CMachineData &data, Aris::Core::RT_MSG &recvMsg, Aris::Core::RT_MSG &sendMsg);
	private:
		std::unique_ptr<Robots::ROBOT_BASE> pRobot;
		std::map<std::string, int> mapName2ID;
		std::vector<GAIT_FUNC> allGaits;
		std::vector<PARSE_FUNC> allParsers;

		Aris::Core::CONN server;
		std::string ip,port;

		double homeEE[18], homeIn[18];
		int homeCount[18];

		int mapPhy2Abs[18];
		int mapAbs2Phy[18];
#ifdef PLATFORM_IS_LINUX
		Aris::RT_CONTROL::ACTUATION cs;
#endif
	};

}


