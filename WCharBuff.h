#pragma once

class WCharBuff
{
public:
    WCharBuff();
    WCharBuff(std::string sStr);
    WCharBuff(std::wstring sStr);

    operator LPWSTR()
    {
        return GetWCharPtr();
    }

    LPWSTR GetWCharPtr();

private:
    std::vector<WCHAR> m_data;
};