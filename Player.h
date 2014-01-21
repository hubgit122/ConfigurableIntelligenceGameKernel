
#ifndef __PLAYER_H__
#define __PLAYER_H__
#include "utilities.h"
#include "CIGObject.h"
#include "Array.h"
#include "Stack.h"
#include "GUI.h"
#include "CIGRuleConfig.h"

namespace CIG
{
	class Chessboard;
	// ע��Player����ȫ������Chesssboard��, ��Ϊ���ӵ�ƫ����. ���Գ�ʼ��ʱ���û��Chesssboard����Ϣ, ҲҪ����ֵ�����Ǹ���.
	// ӵ�е�����Ҳ�ǹ���������̵Ĺ��캯����߼���ȥ��.
	class Player: public CIGObject
	{
		public:
			Player();
			Player(CIGRuleConfig::PLAYER_NAMES p, void (*makeBestMove_)(Chessboard*cb, void*) = GUI::askForMove , Chessboard* cb = NULL);
			Player(const Player& p, Chessboard* cb = NULL);
			virtual ~Player();

			void operator = (const Player& p);

			CIGRuleConfig::PLAYER_NAMES NAME;
			Stack<Chessman, CIGRuleConfig::INI_CHESSMAN_GROUP_SIZE, 0> ownedChessmans;
			Chessboard* chessboard;

			void (*makeBestMove)(Chessboard*cb, void* op);
			//static const unsigned char color[CIGRuleConfig::PLAYER_NUM][3];
			//static const HBITMAP bitMap;
	};
}

#endif /*__PLAYER_H_*/

