#include "stdafx.h"
#include "GUI.h"
#include "CIGRuleConfig.h"
#include "ChessBoard.h"

namespace CIG
{
	bool CIG::CIGRuleConfig::checkConstrains()
	{
		typedef unsigned long long U64;

		if (sizeof(U64) != 8)					//检测过之后, 认为sizeof(U64)==8, 这样会化简一些除法.
		{
			ostringstream oss;
			oss << "系统字长与期望不同. \n" << "sizeof(U64) = " << sizeof(U64) << " != 8";
			GUI::inform(oss.str(), true);
			return false;
		}

		return true;
	}

	const float CIGRuleConfig::TIME_LIMIT = -1;

	const CIGRuleConfig::OPERATIONS CIGRuleConfig::operationGraph[END][END + 1] =
	{
		// TO-DO
	};

	const CIGRuleConfig::CHESSMAN_TYPES CIGRuleConfig::INI_BOARD[PLAYER_NUM][1 << INI_BOARD_HEIGHT_LOG2][1 << INI_BOARD_WIDTH_LOG2] =
	{
		// TO-DO
	};

	const bool CIGRuleConfig::BOARD_RANGE[1 << INI_BOARD_HEIGHT_LOG2][1 << INI_BOARD_WIDTH_LOG2] =
	{
		// TO-DO
	};

	//const int CIGRuleConfig::EVALUATIONS[PLAYER_NUM][CHESSMAN_TYPE_NUM][1 << INI_BOARD_HEIGHT_LOG2][1 << INI_BOARD_WIDTH_LOG2] =
	//{// TO-DO

	//};

	char* CIGRuleConfig::OPERATION_NAME[END] =
	{
		"NOMORE",
		"BEGIN",
		"ADD",
		"PICK",
		"PUT",
		"CAPTURE",
		"PROMOTION",
		"DECOVER",
	};
}
