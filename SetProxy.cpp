// SetProxy.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "WCharBuff.h"
#include "SetProxy.h"

using namespace std;

bool SetProxy(SET_PROXY_OPTIONS& options)
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
            optList.emplace_back();
            optList.back().dwOption = INTERNET_PER_CONN_PROXY_BYPASS;
            optList.back().Value.pszValue =
                options.fBypassLocal ? (LPWSTR)L"<local>" : (LPWSTR)L"";
    }

    INTERNET_PER_CONN_OPTION_LIST list;
    ZeroMemory(&list, sizeof(list));
    list.dwSize = sizeof(list);
    list.pszConnection = options.sConnection;
    list.pOptions = optList.data();
    list.dwOptionCount = static_cast<DWORD>(optList.size());

    bool isSet = true;
    if (!InternetSetOption(
        NULL,
        INTERNET_OPTION_PER_CONNECTION_OPTION,
        &list,
        sizeof(list)))
    {
        cout << "Error " << GetLastError() << " setting options." << endl;
        isSet = false;
    }
    else if (!InternetSetOption(NULL, INTERNET_OPTION_SETTINGS_CHANGED, NULL, 0))
    {
        cout << "Error " << GetLastError() << " sending change notification." << endl;
    }
    else if (!InternetSetOption(NULL, INTERNET_OPTION_REFRESH, NULL, 0))
    {
        cout << "Error " << GetLastError() << " refreshing internet options." << endl;
    }
    return isSet;
}

void Usage()
{
    cout << "Usage: SetProxy [-cx connName] [-cr regex] [-ds] [-uc] [-su scriptUrl] [-up] [-pu proxyUrl] [-bl] [-clear]" << endl;
    cout << "  -cx Connection Name - If you omit this, then the settings are applied" << endl
        << "      as system default." << endl;
    cout << "  -cr Connection RegEx - This is a regular expression to select the" << endl
        << "      connection to apply the change to. If you specify this, you can't" << endl
        << "      specify the -cx parameter. Also, if more than one connection" << endl
        << "      matches, then it will be applied to all that match." << endl;
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
    cout << "  -clear Clear proxy settings. The only other parameter this can be" << endl
        << "      specified with is connection name." << endl;
    cout << "  -enum Enumerate all connections, must be specified alone." << endl;
    cout << endl;
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
            // Get connection regex.
            if (op == "cr")
            {
                if (args.end() == arg + 1 ||
                    (*(arg + 1))[0] == '/' ||
                    (*(arg + 1))[0] == '-')
                {
                    cout << "Usage Error, /cr flag without data." << endl;
                    success = false;
                    break;
                }
                options.connRegEx = *(++arg);
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
            else if (op == "enum")
            {
                options.fEnum = true;
            }
            else if (op == "?" || op == "help" || op == "h")
            {
                success = false;
            }
            else
            {
                success = false;
                cout << "Usage Error: " << *arg << " is not a valid parameter." << endl;
            }
        }
        // Do some sanity checks.
        if (nullptr != options.sConnection && !options.connRegEx.empty())
        {
            cout << "Usage Error, you can't specify both a connection name and a connection regex." << endl;
        }
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
                options.fEnum ||
                nullptr != options.sConfigScriptUrl ||
                nullptr != options.sProxyServerUrl)
            {
                cout << "Usage Error: Only connection name can be specified when you specify -clear." << endl;
                success = false;
            }
        }
        if (options.fEnum)
        {
            if (options.fDetectSettings ||
                options.fUseConfigScript ||
                options.fUseProxyServer ||
                options.fBypassLocal ||
                options.fClear ||
                nullptr != options.sConfigScriptUrl ||
                nullptr != options.sProxyServerUrl ||
                nullptr != options.sConnection)
            {
                cout << "Usage Error: When you specify -enum, no other parameters are permitted." << endl;
                success = false;
            }
        }
    }
    return success;
}

string StringBool(bool f)
{
    return f ? "True" : "False";
}

void PrintSettings(SET_PROXY_OPTIONS& opts)
{
    string scope = nullptr == opts.sConnection ?
        "LAN Settings" : string("Connection ") + (string)opts.sConnection;
    cout << "Setting applied for " << scope << ":" << endl;
    cout << "  Detect Settings: " << StringBool(opts.fUseConfigScript) << endl;
    cout << "  Use Config Script: " << StringBool(opts.fUseConfigScript) << endl;
    cout << "  Config Script URL: " << (nullptr == opts.sConfigScriptUrl ?
        string("(none)") : (string)opts.sConfigScriptUrl) << endl;
    cout << "  Use Fixed Proxy Server: " << StringBool(opts.fUseProxyServer) << endl;
    cout << "  Proxy Server URL: " << (nullptr == opts.sProxyServerUrl ?
        string("(none)") : (string)opts.sProxyServerUrl) << endl;
    cout << "  Bypass Proxy for Local Addresses: " << StringBool(opts.fBypassLocal) << endl << endl;
}

vector<string> GetConnections()
{
    DWORD size = 0;
    DWORD entries = 0;
    DWORD dwStatus = RasEnumEntries(NULL, NULL, NULL, &size, &entries);
    vector<string> names;
    if (entries > 0)
    {
        vector<RASENTRYNAME> namestructs;
        namestructs.resize(entries);
        namestructs[0].dwSize = sizeof(RASENTRYNAME);
        dwStatus = RasEnumEntries(NULL, NULL, namestructs.data(), &size, &entries);
        if (dwStatus == NO_ERROR)
        {
            for (auto name : namestructs)
            {
                names.push_back(WCharBuff::GetString(name.szEntryName));
            }
        }
    }
    return names;
}

bool Apply(SET_PROXY_OPTIONS& opts)
{
    bool status = true;
    if (SetProxy(opts))
    {
        PrintSettings(opts);
    }
    else
    {
        string scope = nullptr == opts.sConnection ?
            "LAN Settings" : string("Connection ") + (string)opts.sConnection;
        cout << "Error applying settings to " << scope << "." << endl;
        status = false;
    }
    return status;
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
        if (opts.fEnum)
        {
            auto conns = GetConnections();
            if (conns.size() > 0)
            {
                cout << "Connections:" << endl;
                for (auto name : conns)
                {
                    cout << "  " << name << endl;
                }
            }
            else
            {
                cout << "No connections found." << endl;
            }
            cout << endl;
        }
        else
        {
            list<string> applyTo;
            bool lan = opts.connRegEx.empty() && nullptr == opts.sConnection;
            if (opts.connRegEx.empty() && nullptr != opts.sConnection)
            {
                applyTo.push_back(opts.sConnection);
            }
            else
            {
                auto connections = GetConnections();
                try
                {
                    regex re(opts.connRegEx, regex_constants::icase);
                    for (auto connection : connections)
                    {
                        if (regex_match(connection, re))
                        {
                            applyTo.push_back(connection);
                        }
                    }
                }
                catch (const regex_error ex)
                {
                    cout << "Error in regular expression: " << ex.what() << endl;
                }
            }
            if (lan)
            {
                Apply(opts);
            }
            else
            {
                for (auto apply : applyTo)
                {
                    opts.sConnection = apply;
                    if (!Apply(opts))
                    {
                        break;
                    }
                }
            }
        }
    }
    return 0;
}

