
#ifndef __CHESSMANMOTION_H__
#define __CHESSMANMOTION_H__

#include "CIGObject.h"
#include "CIGRuleConfig.h"
#include "Chessman.h"
#include "ChessmanIndex.h"

namespace CIG
{
	class Motion
	{
		public:
			Motion();
			Motion(const Motion& op);
			Motion(ChessmanIndex ci, CIGRuleConfig::OPERATIONS op, PointOrVector dist = 0, PointOrVector pre = 0);
			virtual ~Motion();
			void operator=(const Motion& op);
			bool operator==(const Motion& op)const;

			friend std::ostringstream& operator<<(std::ostringstream& oss, const Motion& op)						///不加引用符号, 就会调用拷贝构造函数, id管理得乱七八糟.
			{
				oss << "Operation::\n" << "\toperation: " << op.operation << "\n\tchessman: ";
				//oss<<(const Chessman&)(*op.chessman);				// TO-DO
				return oss;
			}

			CIGRuleConfig::OPERATIONS operation;			//操作类型
			ChessmanIndex chessmanIndex;						//棋子所属的玩家号和在玩家手中的索引号
			PointOrVector distination;								//事件的位置
			PointOrVector savedCoodinate;						//为了事件的恢复, 保存初始位置
	};
}

#endif /*__CHESSMANMOTION_H_*/

