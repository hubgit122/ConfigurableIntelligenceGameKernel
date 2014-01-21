
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
			Motion(ChessmanIndex ci, CIGRuleConfig::OPERATIONS op, PointOrVector dist = 0);
			virtual ~Motion();
			void operator=(const Motion& op);
			bool operator==(const Motion& op)const;

			friend std::ostringstream& operator<<(std::ostringstream& oss, const Motion& op)						///�������÷���, �ͻ���ÿ������캯��, id���������߰���.
			{
				oss << "Operation::\n" << "\toperation: " << op.operation << "\n\tchessman: ";
				//oss<<(const Chessman&)(*op.chessman);				// TO-DO
				return oss;
			}

			CIGRuleConfig::OPERATIONS operation;			//��������
			ChessmanIndex chessmanIndex;				//������������Һź���������е�������
			PointOrVector distination;								//�¼���λ��
	};
}

#endif /*__CHESSMANMOTION_H_*/
