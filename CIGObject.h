#ifndef __CIGOBJECT_H__
#define __CIGOBJECT_H__

#include "utilities.h"
#include "CIGRuleConfig.h"
#include "GUI.h"
/*************************************************************************
  ����������ϵͳ�������Ļ���, ���������������ͳ�Ա�ĸ���.
  Ҫ�������ַ����������ڳ��ڸ���.
**************************************************************************/
namespace CIG
{
	class CIGObject
	{
		public:
			inline CIGObject() {};
			inline CIGObject(const CIGObject& o) {};
			inline virtual ~CIGObject() {};

			inline virtual void informError(const string& str)const
			{
				ostringstream oss;
				oss << *this;
				GUI::inform(str + oss.str(), true);		///C++��̬������Ա�������������.
			}

			friend ostream& operator << (ostream& os, const CIGObject& o)
			{
				ostringstream oss;
				oss << o;
				os << oss.str();
				return os;
			}

			friend ostringstream& operator<<(ostringstream& oss, const CIGObject& o)						///�������÷���, �ͻ���ÿ������캯��, id��������߰���.
			{
				oss << "CIGObject::\n*******************************************************************************\n";				// TO-DO ����ַ���
				return oss;
			}
	};
}
#endif /*__CIGOBJECT_H_*/

