#include "nekoPrecompiledHeader.h"
#include "nekoVariable.h"
#include "nekoUtility.h"
#include "nekoGame.h"
#include "NekoNovel.h"

namespace neko
{
	static const char8 *unknownVar = "존재하지 않는 변수";

	/// Constructor
	nekoVariable::nekoVariable()
		: mType(EVT_STRING)
	{

	}

	/// Destructor
	nekoVariable::~nekoVariable()
	{

	}

	/// Set Values
	void nekoVariable::SetValue(const char8 *value)
	{
		mType = GetTypeFrom(value);

		if(mType == EVT_STRING)
		{
			uint32 si = 0;
			uint32 l = 0;
			bool start = false;
			const char8 *p = value;

			while(*p != '\0')
			{
				if(*p == '\"')
				{
					start = !start;
					if(start)
					{
						si = p - value + 1;
						p++;
						continue;
					}
					else
						break;
				}

				if(*p == '\\' && *(p + 1) == '\"')
				{
					p++;
					l++;
				}

				p++;
				l++;
			}

			if(l)
				mString = string(value).substr(si, l);
			else
				mString.clear();

			return;
		}
		else if(mType == EVT_UNKNOWN)
		{
			nekoVariable *var = GetNekoNovel()->GetCurrentGame()->GetVariable(value);
			if(!var)
			{
				mString = unknownVar;
				mType = EVT_STRING;
				return;
			}

			mString = var->GetValue();
			mType = var->GetType();

			return;
		}

		mString = value;
	}

	/// Operate value
	result nekoVariable::OperateValue(E_OPERATOR_TYPE opType, const char8 *value)
	{
		E_VARIABLE_TYPE varType = GetTypeFrom(value);

		// 만약 변수일 경우, 가져오기
		if(varType == EVT_UNKNOWN)
		{
			nekoVariable *var;

			if(!(var = GetNekoNovel()->GetCurrentGame()->GetVariable(value)))
			{
				value = unknownVar;
				varType = EVT_STRING;
			}
			else
			{
				value = var->GetValue();
				varType = var->GetType();
			}
		}

		switch(opType)
		{
		case EOT_ADDEQUAL:
			switch(mType)
			{
			case EVT_STRING:
				{
					uint32 si = 0;
					uint32 l = 0;
					bool start = false;
					const char8 *p = value;

					while(*p != '\0')
					{
						if(*p == '\"')
						{
							start = !start;
							if(start)
							{
								si = p - value + 1;
								p++;
								continue;
							}
							else
								break;
						}

						if(*p == '\\' && *(p + 1) == '\"')
						{
							p++;
							l++;
						}

						p++;
						l++;
					}

					mString += string(value).substr(si, l);
				}
				break;
			case EVT_INTEGER:
				{
					if(varType == EVT_STRING)
						return E_FAIL;

					switch(varType)
					{
					case EVT_FLOAT:
						mType = EVT_FLOAT;
						mString = nekoUtility::FormString("%f", atoi(mString.c_str()) + atof(value));
						break;
					case EVT_INTEGER:
						mString = nekoUtility::FormString("%d", atoi(mString.c_str()) + atoi(value));
						break;
					default:
						nekoAssert(false, "연산 오류.");
					}
				}
				break;
			case EVT_FLOAT:
				{
					if(varType == EVT_STRING)
						return E_FAIL;

					mString = nekoUtility::FormString("%f", atof(mString.c_str()) + atof(value));
				}
				break;
			}
			break;
		case EOT_SUBTRACTEQUAL:
			switch(mType)
			{
			case EVT_STRING:
				return E_FAIL;
				break;
			case EVT_INTEGER:
				{
					if(varType == EVT_STRING)
						return E_FAIL;

					switch(varType)
					{
					case EVT_FLOAT:
						mType = EVT_FLOAT;
						mString = nekoUtility::FormString("%f", atoi(mString.c_str()) - atof(value));
						break;
					case EVT_INTEGER:
						mString = nekoUtility::FormString("%d", atoi(mString.c_str()) - atoi(value));
						break;
					default:
						nekoAssert(false, "연산 오류.");
					}
				}
				break;
			case EVT_FLOAT:
				{
					if(varType == EVT_STRING)
						return E_FAIL;

					mString = nekoUtility::FormString("%f", atof(mString.c_str()) - atof(value));
				}
				break;
			}
			break;
		case EOT_MULTIPLYEQUAL:
			switch(mType)
			{
			case EVT_STRING:
				return E_FAIL;
				break;
			case EVT_INTEGER:
				{
					if(varType == EVT_STRING)
						return E_FAIL;

					switch(varType)
					{
					case EVT_FLOAT:
						mType = EVT_FLOAT;
						mString = nekoUtility::FormString("%f", atoi(mString.c_str()) * atof(value));
						break;
					case EVT_INTEGER:
						mString = nekoUtility::FormString("%d", atoi(mString.c_str()) * atoi(value));
						break;
					default:
						nekoAssert(false, "연산 오류.");
					}
				}
				break;
			case EVT_FLOAT:
				{
					if(varType == EVT_STRING)
						return E_FAIL;

					mString = nekoUtility::FormString("%f", atof(mString.c_str()) * atof(value));
				}
				break;
			}
			break;
		case EOT_DIVIDEEQUAL:
			switch(mType)
			{
			case EVT_STRING:
				return E_FAIL;
				break;
			case EVT_INTEGER:
				{
					if(varType == EVT_STRING)
						return E_FAIL;

					switch(varType)
					{
					case EVT_FLOAT:
						mType = EVT_FLOAT;
						mString = nekoUtility::FormString("%f", atoi(mString.c_str()) / atof(value));
						break;
					case EVT_INTEGER:
						mString = nekoUtility::FormString("%d", atoi(mString.c_str()) / atoi(value));
						break;
					default:
						nekoAssert(false, "연산 오류.");
					}
				}
				break;
			case EVT_FLOAT:
				{
					if(varType == EVT_STRING)
						return E_FAIL;

					mString = nekoUtility::FormString("%f", atof(mString.c_str()) / atof(value));
				}
				break;
			}
			break;
		case EOT_DIVIDELEFTEQUAL:
			switch(mType)
			{
			case EVT_STRING:
			case EVT_FLOAT:
				return E_FAIL;
				break;
			case EVT_INTEGER:
				{
					if(varType == EVT_STRING || atoi(value) == 0)
						return E_FAIL;

					switch(varType)
					{
					case EVT_INTEGER:
						mString = nekoUtility::FormString("%d", atoi(mString.c_str()) % atoi(value));
						break;
					default:
						nekoAssert(false, "연산 오류.");
					}
				}
				break;
			}
			break;
		}

		return S_OK;
	}

	bool nekoVariable::If(E_OPERATOR_TYPE opType, string rvar1, string rvar2)
	{
		E_VARIABLE_TYPE vtype1 = GetTypeFrom(rvar1.c_str());
		E_VARIABLE_TYPE vtype2 = GetTypeFrom(rvar2.c_str());

		// 변수인듯 ?
		if(vtype1 == EVT_UNKNOWN)
		{
			nekoVariable *var = GetNekoNovel()->GetCurrentGame()->GetVariable(rvar1.c_str());
			if(var)
			{
				vtype1 = var->GetType();
				rvar1 = var->GetValue();
			}
		}
		else if(vtype1 == EVT_STRING)
		{
			// 왼쪽, 오른쪽 " 빼야지..
			rvar1 = rvar1.substr(1, rvar1.length() - 2);
		}

		if(vtype2 == EVT_UNKNOWN)
		{
			nekoVariable *var = GetNekoNovel()->GetCurrentGame()->GetVariable(rvar2.c_str());
			if(var)
			{
				vtype2 = var->GetType();
				rvar2 = var->GetValue();
			}
		}
		else if(vtype2 == EVT_STRING)
		{
			// 왼쪽, 오른쪽 " 빼야지..
			rvar2 = rvar2.substr(1, rvar2.length() - 2);
		}

		if((vtype1 == EVT_STRING && vtype2 != EVT_STRING) ||
			(vtype1 != EVT_STRING && vtype2 == EVT_STRING))
			return false;

		bool result = false;
		bool reverse = false;

		switch(opType)
		{
		case EOT_NOTEQUAL:
			reverse = true;
		case EOT_EQUAL:
			{
				switch(vtype1)
				{
				case EVT_STRING:
					{
						result = (strcmp(rvar1.c_str(), rvar2.c_str()) == 0);
					}
					break;
				case EVT_INTEGER:
				case EVT_FLOAT:
					{
						result = abs(atof(rvar1.c_str()) - atof(rvar2.c_str())) <= 0.0001f;
					}
					break;
				case EVT_UNKNOWN:
					result = (rvar2 == "없음");
					break;
				}
			}
			break;
		case EOT_RIGHTEQUALBIGGER:
			reverse = true;
		case EOT_LEFTBIGGER:
			{
				if(vtype1 == EVT_STRING)
					return false;

				result = atof(rvar1.c_str()) > atof(rvar2.c_str());
			}
			break;
		case EOT_RIGHTBIGGER:
			reverse = true;
		case EOT_LEFTEQUALBIGGER:
			{
				if(vtype1 == EVT_STRING)
					return false;

				result = atof(rvar1.c_str()) >= atof(rvar2.c_str());
			}
			break;
		case EOT_INCLUDE:
		case EOT_INCLUDENOCASE:
			{
				const char8 *s = rvar1.c_str();
				const char8 *d = rvar2.c_str();
				char a, b;
				bool noCase = (opType == EOT_INCLUDENOCASE);

				while(*s != '\0')
				{
					a = *s;
					b = *d;

					if(noCase)
					{
						if(a >= 'A' && a <= 'Z')
							a += 32;
						if(b >= 'A' && b <= 'Z')
							b += 32;
					}

					if(a == b)
					{
						++d;

						if(*d == '\0')
						{
							result = true;
							break;
						}
					}
					else
					{
						d = rvar2.c_str();
					}

					++s;
				}
			}
			break;
		default:
			return false;
		}

		if(reverse)
			return !result;
		return result;
	}

	E_VARIABLE_TYPE nekoVariable::GetTypeFrom(const char8 *text)
	{
		{
			const char8 *p = text;
			int32 digits = 0;
			int32 point = 0;

			while(*p != '\0')
			{
				if(*p == ' ')
				{
					++p;
					continue;
				}

				if(!((*p >= '0' && *p <= '9') || *p == '-' || *p == '+'))
				{
					// 실수인듯..
					if(*p == '.')
					{
						// 아니네 -_-;
						if(point > 1)
							break;

						++point;
						++p;
						continue;
					}
					break;
				}

				++p;
				++digits;
			}

			// 끝났다면..
			if(digits > 0 && *p == '\0')
			{
				if(point > 0)
					return EVT_FLOAT;
				return EVT_INTEGER;
			}
		}

		if(*text == '\"' && *(text + 1) != '\0')
			return EVT_STRING;

		return EVT_UNKNOWN;
	}

	void nekoVariable::Load(int32 version, nekoByteBuffer &buf)
	{
		buf >> mString >> mType;
	}
}