#include "stdafx.h"
#include "WCharBuff.h"

using namespace std;

WCharBuff::WCharBuff()
{
}

WCharBuff::WCharBuff(string sStr)
{
    wstring wide(sStr.begin(), sStr.end());
    m_data = vector<WCHAR>(wide.begin(), wide.end());
    m_data.push_back(L'\0');
}

WCharBuff::WCharBuff(wstring sStr)
{
    m_data = vector<WCHAR>(sStr.begin(), sStr.end());
    m_data.push_back(L'\0');
}

LPWSTR WCharBuff::GetWCharPtr()
{
    return m_data.size() > 0 ?
        (LPWSTR)m_data.data() :
        nullptr;
}