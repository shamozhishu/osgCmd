#ifndef __ZOO_UTILS_H__
#define __ZOO_UTILS_H__

#include <zoo/Support.h>

namespace zoo {

/* ���ֽ��ַ����ָ�� */
template<typename Container>
inline void stringtok(Container& container, const std::string& in,
	const char* const delimiters = " \t\n", const int deep = 0)
{
	const std::string::size_type len = in.length();
	std::string::size_type i = 0;
	int count = 0;
	while (i < len)
	{
		i = in.find_first_not_of(delimiters, i);
		if (i == std::string::npos)
			return; // nothing left
		// find the end of the token
		std::string::size_type j = in.find_first_of(delimiters, i);
		count++;
		// push token
		if (j == std::string::npos || (deep > 0 && count > deep))
		{
			container.push_back(in.substr(i));
			return;
		}
		else
			container.push_back(in.substr(i, j - i));
		// set up for next loop
		i = j + 1;
	}
}

/* ���ַ��ַ����ָ�� */
template<typename Container>
inline void wstringtok(Container& container, const std::wstring& in,
	const wchar_t* const delimiters = L" \t\n", const int deep = 0)
{
	const std::wstring::size_type len = in.length();
	std::wstring::size_type i = 0;
	int count = 0;
	while (i < len)
	{
		i = in.find_first_not_of(delimiters, i);
		if (i == std::wstring::npos)
			return; // nothing left
		// find the end of the token
		std::string::size_type j = in.find_first_of(delimiters, i);
		count++;
		// push token
		if (j == std::wstring::npos || (deep > 0 && count > deep))
		{
			container.push_back(in.substr(i));
			return;
		}
		else
			container.push_back(in.substr(i, j - i));
		// set up for next loop
		i = j + 1;
	}
}

/* ����vector��������ظ�Ԫ�� */
template<typename T>
void clearDupElements(vector<T>& elems)
{
	typename vector<T>::iterator it, dupit;
	for (it = ++elems.begin(); it != elems.end();)
	{
		dupit = find(elems.begin(), it, *it);
		if (dupit != it)
			it = elems.erase(it);
		else
			it++;
	}
}

/* ����list��������ظ�Ԫ�� */
template<typename T>
void clearDupElements(list<T>& elems)
{
	typename list<T>::iterator it, dupit;
	for (it = ++elems.begin(); it != elems.end();)
	{
		dupit = find(elems.begin(), it, *it);
		if (dupit != it)
			it = elems.erase(it);
		else
			it++;
	}
}

/* �޼������ֽ��ַ�����ǰ��Ŀհ׷� */
static inline string& trim(string& str)
{
	if (!str.empty())
	{
		str.erase(0, str.find_first_not_of(" \n\r\t"));
		str.erase(str.find_last_not_of(" \n\r\t") + 1);
	}
	return str;
}

/* �޼������ַ��ַ�����ǰ��Ŀհ׷� */
static inline wstring& trim(wstring& str)
{
	if (!str.empty())
	{
		str.erase(0, str.find_first_not_of(L" \n\r\t"));
		str.erase(str.find_last_not_of(L" \n\r\t") + 1);
	}
	return str;
}

/* ���ݸ�Ŀ¼ */
_zooExport extern string DATA_ROOT_DIR_ANSI;
_zooExport extern string DATA_ROOT_DIR_UTF8;

/* ���ַ�����ֽ��ַ���֮���ת�� */
_zooExport string  w2a_(const wstring& wideStr);
_zooExport wstring a2w_(const string& ansiStr);
_zooExport string  unicodeToUtf8(const wstring& wstr);
_zooExport wstring utf8ToUnicode(const string& str);
_zooExport string  ansiToUtf8(const string& str);
_zooExport string  utf8ToAnsi(const string& str);

/* �ַ���ת��ΪСд */
_zooExport string  strToLower(string str);
_zooExport wstring strToLower(wstring str);

/* �ַ���ת��Ϊ��д */
_zooExport string  strToUpper(string str);
_zooExport wstring strToUpper(wstring str);

/* �ַ����Ƚ� */
_zooExport bool    compareNoCase(string strA, const string strB);
_zooExport bool    compareNoCase(wstring strA, const wstring strB);

/* ���ܽ��� */
_zooExport string  encrypt(string str, unsigned short key);
_zooExport string  decrypt(string str, unsigned short key);

/* ����Ŀ¼ */
_zooExport string getWorkDir();
_zooExport bool   setWorkDir(const string& path);

/* �õ���ִ���ļ�Ŀ¼ */
_zooExport string getExeDir();

/* ��ָ���ļ�����Ѱ������ָ����ʽ���ļ�·���� */
_zooExport bool findFileDir(vector<string>& arrFilePath, const string& strCurPath, const string& fileFormat);
_zooExport bool findFileDir(vector<wstring>& arrFilePath, const wstring& strCurPath, const wstring& fileFormat);

/* �ַ����滻���� */
_zooExport string&  strReplaceAll(string& str, const string& old_value, const string& new_value);
_zooExport wstring& wstrReplaceAll(wstring& str, const wstring& old_value, const wstring& new_value);
_zooExport string&  strReplaceAllDistinct(string& str, const string& old_value, const string& new_value);
_zooExport wstring& wstrReplaceAllDistinct(wstring& str, const wstring& old_value, const wstring& new_value);

}

#endif // __ZOO_UTILS_H__
