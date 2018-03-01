// SetProxy.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "WCharBuff.h"
#include "SetProxy.h"

using namespace std;

void SetProxy(SET_PROXY_OPTIONS& options)
{
    vector<INTERNET_PER_CONN_OPTION> optList;

    if (options.fClear)
    {
        optList.emplace_back();
        optList.back().dwOption = INTERNET_PER_CONN_FLAGS_UI;
        optList.back().Value.dwValue = PROXY_TYPE_DIRECT;
    }
    else
    {
        // Build flags.
        DWORD dwFlags = 0;
        if (options.fDetectSettings)
        {
            dwFlags |= PROXY_TYPE_AUTO_DETECT;
        }
        if (options.fUseConfigScript)
        {
            dwFlags |= PROXY_TYPE_AUTO_PROXY_URL;
        }
        if (options.fUseProxyServer)
        {
            dwFlags |= PROXY_TYPE_PROXY;
        }
        else
        {
            dwFlags |= PROXY_TYPE_DIRECT;
        }
        optList.emplace_back();
        optList.back().dwOption = INTERNET_PER_CONN_FLAGS_UI;
        optList.back().Value.dwValue = dwFlags;

        // Set config script.
        if (nullptr != options.sConfigScriptUrl)
        {
            optList.emplace_back();
            optList.back().dwOption = INTERNET_PER_CONN_AUTOCONFIG_URL;
            optList.back().Value.pszValue = options.sConfigScriptUrl;
        }

        // Set proxy server URL.
        if (nullptr != options.sProxyServerUrl)
        {
            optList.emplace_back();
            optList.back().dwOption = INTERNET_PER_CONN_PROXY_SERVER;
            optList.back().Value.pszValue = options.sProxyServerUrl;
        }

        // Do we skip the proxy for local addresses?
        if (options.fBypassLocal)
        {
            optList.emplace_back();
            optList.back().dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
            optList.back().Value.pszValue = (LPWSTR)L"<local>";
        }
    }

    INTERNET_PER_CONN_OPTION_LIST list;
    ZeroMemory(&list, sizeof(list));
    list.dwSize = sizeof(list);
    list.pszConnection = options.sConnection;
    list.pOptions = optList.data();
    list.dwOptionCount = optList.size();

    if (!InternetSetOption(
        NULL,
        INTERNET_OPTION_PER_CONNECTION_OPTION,
        &list,
        sizeof(list)))
    {
        cout << "Error " << GetLastError() << " setting options." << endl;
    }
    else if (!InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0))
    {
        cout << "Error " << GetLastError() << " sending change notification." << endl;
    }
    else if (!InternetSetOption(NULL, INTERNET_OPTION_REFRESH, NULL, 0))
    {
        cout << "Error " << GetLastError() << " refreshing internet options." << endl;
    }
}

void Usage()
{
    cout << "Usage: SetProxy [-cx connName] [-ds] [-uc] [-su scriptUrl] [-up] [-pu proxyUrl] [-bl] [-clear]" << endl;
    cout << "  -cx Connection Name - If you omit this, then the settings are applied" << endl
        << "      as system default." << endl;
    cout << "  -ds Detect Settings - This is specified without a parameter." << endl
        << "      This determines whether to try to get a DHCP setting for proxy." << endl;
    cout << "  -uc Use Config Script - This is specified without a parameter." << endl
        << "      This determines whether a config script is used." << endl;
    cout << "  -su Config Script - This specifies the config script to use." << endl;
    cout << "  -up Use Proxy - This is specified without a parameter." << endl
        << "      This determines whether to use a manually configured proxy." << endl;
    cout << "  -pu Proxy URL - This is the URL for the manually configured proxy." << endl;
    cout << "  -bl Bypass Local - This is specified without a parameter." << endl
        << "      This determines whether local addresses bypass the proxy setting." << endl;
    cout << "-clear Clear proxy settings. The only other parameter this can be" << endl
        << "       specified with is connection name." << endl;
}

bool ParseCmdLine(vector<string> args, SET_PROXY_OPTIONS& options)
{
    bool success = false;
    if (args.size() > 0)
    {
        success = true;
        for (auto arg = args.begin(); arg != args.end(); ++arg)
        {
            if ('/' != (*arg)[0] && '-' != (*arg)[0])
            {
                cout << "Usage Error." << endl;
                success = false;
                break;
            }
            string op = (*arg).substr(1);
            // Get connection name.
            if (op == "cx")
            {
                if (args.end() == arg + 1 ||
                    (*(arg + 1))[0] == '/' ||
                    (*(arg + 1))[0] == '-')
                {
                    cout << "Usage Error, /cx flag without data." << endl;
                    success = false;
                    break;
                }
                options.sConnection = *(++arg);
            }
            // Get detect settings.
            else if (op == "ds")
            {
                options.fDetectSettings = true;
            }
            // Get use config script.
            else if (op == "uc")
            {
                options.fUseConfigScript = true;
            }
            // Get config script URL.
            else if (op == "su")
            {
                if (args.end() == arg + 1 ||
                    (*(arg + 1))[0] == '/' ||
                    (*(arg + 1))[0] == '-')
                {
                    cout << "Usage Error, /su flag without data." << endl;
                    success = false;
                    break;
                }
                options.sConfigScriptUrl = *(++arg);
            }
            // Get use proxy.
            else if (op == "up")
            {
                options.fUseProxyServer = true;
            }
            // Get proxy URL.
            else if (op == "pu")
            {
                if (args.end() == arg + 1 ||
                    (*(arg + 1))[0] == '/' ||
                    (*(arg + 1))[0] == '-')
                {
                    cout << "Usage Error, /pu flag without data." << endl;
                    success = false;
                    break;
                }
                options.sProxyServerUrl = *(++arg);
            }
            // Get bypass local.
            else if (op == "bl")
            {
                options.fBypassLocal = true;
            }
            else if (op == "clear")
            {
                options.fClear = true;
            }
        }
        // Do some sanity checks.
        if (options.fUseConfigScript && nullptr == options.sConfigScriptUrl)
        {
            cout << "Usage Error, /uc flag without /cs flag." << endl;
            success = false;
        }
        if (options.fUseProxyServer && nullptr == options.sProxyServerUrl)
        {
            cout << "Usage Error, /up flag without /pu flag." << endl;
            success = false;
        }
        if (options.fClear)
        {
            if (options.fDetectSettings ||
                options.fUseConfigScript ||
                options.fUseProxyServer ||
                options.fBypassLocal ||
                nullptr != options.sConfigScriptUrl ||
                nullptr != options.sProxyServerUrl)
            {
                cout << "Usage Error: Only connection name can be specified when you specify -clear." << endl;
                success = false;
            }
        }
    }
    return success;
}

int main(int argc, char* argv[])
{
    vector<string> args;
    for (int i = 1; i < argc; ++i)
    {
        args.emplace_back(argv[i]);
    }
    SET_PROXY_OPTIONS opts;
    if (!ParseCmdLine(args, opts))
    {
        Usage();
    }
    else
    {
        SetProxy(opts);
    }
    return 0;
}

