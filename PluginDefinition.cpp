//this file is part of notepad++
//Copyright (C)2022 Don HO <don.h@free.fr>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include "PluginDefinition.h"
#include "menuCmdID.h"
#include "wchar.h"
#include <windows.h>
#include <shlwapi.h> // Para PathRemoveFileSpec
#include <string>
#include <shlobj.h>  // Para SHGetFolderPath
#include <filesystem> // C++17 para criar pasta
#include <fstream>  // para std::ofstream


bool g_caseSensitive = true;
void removeLinesWithSelection();
void removeLinesWithoutSelection();
void toggleCaseSensitive();

TCHAR iniFilePath[MAX_PATH] = {0};

void initConfigPath() {
    TCHAR appDataPath[MAX_PATH] = {0};
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appDataPath))) {
        std::wstring dir = std::wstring(appDataPath) + L"\\NppRemoveLines";

        std::error_code ec;
        std::filesystem::create_directories(dir, ec);
        if (ec) {
            OutputDebugStringW((L"Erro ao criar diretório: " + std::wstring(ec.message().begin(), ec.message().end()) + L"\n").c_str());
        }

        wcscpy_s(iniFilePath, dir.c_str());
        wcscat_s(iniFilePath, L"\\NppRemoveLines.ini");

        OutputDebugStringW((L"INI path: " + std::wstring(iniFilePath) + L"\n").c_str());

        std::ifstream iniCheck(iniFilePath);
        if (!iniCheck.good()) {
            iniCheck.close();

            // Cria arquivo e escreve conteúdo default
            std::ofstream createIni(iniFilePath);
            if (createIni) {
                createIni << "[Settings]\nCaseSensitive=0\n";
                createIni.close();
                OutputDebugStringW(L"Arquivo INI criado com conteúdo padrão.\n");
            }
            else {
                OutputDebugStringW(L"Falha ao criar arquivo INI.\n");
            }
        } else {
            iniCheck.close();
        }
    }
    else {
        OutputDebugStringW(L"Falha ao obter o caminho %APPDATA%\n");
    }
}

void saveConfig() {
    WritePrivateProfileString(L"Settings", L"CaseSensitive", g_caseSensitive ? L"1" : L"0", iniFilePath);
}

void loadConfig() {
    wchar_t buf[8] = {0};
    GetPrivateProfileString(L"Settings", L"CaseSensitive", L"0", buf, 8, iniFilePath);
    g_caseSensitive = (wcscmp(buf, L"1") == 0);
}

//
// The plugin data that Notepad++ needs
//
FuncItem funcItem[nbFunc];

//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;

//
// Initialize your plugin data here
// It will be called while plugin loading   
void pluginInit(HANDLE /*hModule*/) {
	initConfigPath();
    loadConfig();
    setCommand(FUNC_REMOVE_LINES, TEXT("Remover linhas que CONTÊM a seleção"), removeLinesWithSelection, nullptr, false);
	setCommand(FUNC_REMOVE_LINES_NOT, TEXT("Remover linhas que NÃO CONTÊM a seleção"), removeLinesWithoutSelection, nullptr, false);
	setCommand(FUNC_TOGGLE_CASE_SENSITIVITY, TEXT("Case Sensitive"), toggleCaseSensitive, nullptr, false);
	//::SendMessage(nppData._nppHandle, NPPM_SETMENUITEMCHECK, funcItem[FUNC_TOGGLE_CASE_SENSITIVITY]._cmdID, g_caseSensitive);
}

void toggleCaseSensitive() {
    // Atualiza o estado do menu com check
    g_caseSensitive = !g_caseSensitive;
	::SendMessage(nppData._nppHandle, NPPM_SETMENUITEMCHECK, funcItem[FUNC_TOGGLE_CASE_SENSITIVITY]._cmdID, g_caseSensitive);
    saveConfig(); // ← salva no .ini sempre que mudar
}


extern "C" __declspec(dllexport) void beNotified(SCNotification *notifyCode)
{
	switch (notifyCode->nmhdr.code) 
	{
		case NPPN_SHUTDOWN:
		{
			commandMenuCleanUp();
		}
		break;

		case NPPN_READY:
		{
			// Aqui é seguro alterar o estado visual do menu
			::SendMessage(nppData._nppHandle, NPPM_SETMENUITEMCHECK,
			              funcItem[FUNC_TOGGLE_CASE_SENSITIVITY]._cmdID,
			              g_caseSensitive);
		}
		break;

		default:
			return;
	}
}



//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
}

//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{

    //--------------------------------------------//
    //-- STEP 3. CUSTOMIZE YOUR PLUGIN COMMANDS --//
    //--------------------------------------------//
    // with function :
    // setCommand(int index,                      // zero based number to indicate the order of command
    //            TCHAR *commandName,             // the command name that you want to see in plugin menu
    //            PFUNCPLUGINCMD functionPointer, // the symbol of function (function pointer) associated with this command. The body should be defined below. See Step 4.
    //            ShortcutKey *shortcut,          // optional. Define a shortcut to trigger this command
    //            bool check0nInit                // optional. Make this menu item be checked visually
    //            );
//	wcscpy(funcItem[0]._itemName, L"Remover linhas com selecao");
//	funcItem[0]._pFunc = removeLinesWithSelection;
//	wcscpy(funcItem[1]._itemName, L"Remover linhas sem selecao");
//	funcItem[1]._pFunc = removeLinesWithoutSelection;
}

//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{
	// Don't forget to deallocate your shortcut here
}


//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool checkOnInit) 
{
    if (index >= nbFunc)
        return false;

    if (!pFunc)
        return false;

    wcscpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = checkOnInit;
    funcItem[index]._pShKey = sk;

    return true;
}

//----------------------------------------------//
//-- STEP 4. DEFINE YOUR ASSOCIATED FUNCTIONS --//
//----------------------------------------------//

#include <sstream>
#include <string>
#include <algorithm>

void removeLinesWithSelection() {
    HWND curScintilla;
    int which = -1;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
    curScintilla = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;

    int selStart = ::SendMessage(curScintilla, SCI_GETSELECTIONSTART, 0, 0);
    int selEnd = ::SendMessage(curScintilla, SCI_GETSELECTIONEND, 0, 0);
    int selLen = selEnd - selStart;

    if (selLen <= 0 || selLen >= 256) return;

    char selected[256] = {0};
    ::SendMessage(curScintilla, SCI_GETSELTEXT, 0, (LPARAM)selected);
    std::string keyword = selected;

    int numLines = ::SendMessage(curScintilla, SCI_GETLINECOUNT, 0, 0);
    std::string result;

    for (int i = 0; i < numLines; ++i) {
        int lineLength = ::SendMessage(curScintilla, SCI_LINELENGTH, i, 0);
        if (lineLength <= 0) continue;

        char* buffer = new char[lineLength + 1];
        memset(buffer, 0, lineLength + 1);

        ::SendMessage(curScintilla, SCI_GETLINE, i, (LPARAM)buffer);
        std::string line(buffer);
        delete[] buffer;

        // Remove \r and \n
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());

		if (g_caseSensitive) {
			if (line.find(keyword) == std::string::npos)
				result += line + "\r\n";
		} else {
			std::string lowerLine = line;
			std::string lowerKeyword = keyword;

			std::transform(lowerLine.begin(), lowerLine.end(), lowerLine.begin(), ::tolower);
			std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);

			if (lowerLine.find(lowerKeyword) == std::string::npos)
				result += line + "\r\n";
		}
    }
    ::SendMessage(curScintilla, SCI_SETTEXT, 0, (LPARAM)result.c_str());
}

void removeLinesWithoutSelection() {
    HWND curScintilla;
    int which = -1;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
    curScintilla = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;

    int selStart = ::SendMessage(curScintilla, SCI_GETSELECTIONSTART, 0, 0);
    int selEnd = ::SendMessage(curScintilla, SCI_GETSELECTIONEND, 0, 0);
    int selLen = selEnd - selStart;

    if (selLen <= 0 || selLen >= 256) return;

    char selected[256] = {0};
    ::SendMessage(curScintilla, SCI_GETSELTEXT, 0, (LPARAM)selected);
    std::string keyword = selected;

    int numLines = ::SendMessage(curScintilla, SCI_GETLINECOUNT, 0, 0);
    std::string result;

    for (int i = 0; i < numLines; ++i) {
        int lineLength = ::SendMessage(curScintilla, SCI_LINELENGTH, i, 0);
        if (lineLength <= 0) continue;

        char* buffer = new char[lineLength + 1];
        memset(buffer, 0, lineLength + 1);
        ::SendMessage(curScintilla, SCI_GETLINE, i, (LPARAM)buffer);

        std::string line(buffer);
        delete[] buffer;

        // Remove \r and \n
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());

        bool contains = false;
        if (g_caseSensitive) {
            contains = (line.find(keyword) != std::string::npos);
        } else {
            std::string lowerLine = line;
            std::string lowerKeyword = keyword;
            std::transform(lowerLine.begin(), lowerLine.end(), lowerLine.begin(), ::tolower);
            std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);
            contains = (lowerLine.find(lowerKeyword) != std::string::npos);
        }

        if (contains)
            result += line + "\r\n";
    }

    ::SendMessage(curScintilla, SCI_SETTEXT, 0, (LPARAM)result.c_str());
}
