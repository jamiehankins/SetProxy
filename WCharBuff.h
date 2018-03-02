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

    operator std::string()
    {
        return GetString(m_data.data());
        //return std::string(m_data.begin(), m_data.end() - 1);
    }

    LPWSTR GetWCharPtr();
    static std::string GetString(LPWSTR wstr);

private:
    std::vector<WCHAR> m_data;
};