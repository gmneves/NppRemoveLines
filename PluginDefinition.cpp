#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include "PluginDefinition.h"
#include "menuCmdID.h"
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cwctype>
#include <filesystem>
#include <fstream>

// Estado global de sensibilidade a maiúsculas/minúsculas
bool g_caseSensitive = false;
TCHAR iniFilePath[MAX_PATH] = {0};

// Dados do Notepad++
FuncItem funcItem[nbFunc];
NppData nppData;

//
// Configuração (INI)
//
void initConfigPath() {
    TCHAR appDataPath[MAX_PATH] = {0};
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appDataPath))) {
        std::wstring dir = std::wstring(appDataPath) + L"\\NppRemoveLines";

        std::error_code ec;
        std::filesystem::create_directories(dir, ec);
        if (ec) {
            OutputDebugStringW((L"Erro ao criar diretorio: " + std::wstring(ec.message().begin(), ec.message().end()) + L"\n").c_str());
        }

        wcscpy_s(iniFilePath, dir.c_str());
        wcscat_s(iniFilePath, L"\\NppRemoveLines.ini");

        std::ifstream iniCheck(iniFilePath);
        if (!iniCheck.good()) {
            iniCheck.close();
            std::ofstream createIni(iniFilePath);
            if (createIni) {
                createIni << "[Settings]\nCaseSensitive=0\n";
                createIni.close();
            }
        } else {
            iniCheck.close();
        }
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
// Inicialização do Plugin
//
void pluginInit(HANDLE /*hModule*/) {
    initConfigPath();
    loadConfig();

    setCommand(FUNC_REMOVE_LINES, TEXT("Remover linhas que CONTÊM a seleção"), removeLinesWithSelection, nullptr, false);
    setCommand(FUNC_REMOVE_LINES_NOT, TEXT("Remover linhas que NÃO CONTÊM a seleção"), removeLinesWithoutSelection, nullptr, false);
    setCommand(FUNC_TOGGLE_CASE_SENSITIVITY, TEXT("Case Sensitive"), toggleCaseSensitive, nullptr, false);
}

void pluginCleanUp() {
}

void commandMenuInit() {
}

void commandMenuCleanUp() {
}

bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool checkOnInit) {
    if (index >= nbFunc || !pFunc)
        return false;

    wcscpy_s(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = checkOnInit;
    funcItem[index]._pShKey = sk;

    return true;
}

void toggleCaseSensitive() {
    g_caseSensitive = !g_caseSensitive;
    ::SendMessage(nppData._nppHandle, NPPM_SETMENUITEMCHECK, funcItem[FUNC_TOGGLE_CASE_SENSITIVITY]._cmdID, g_caseSensitive);
    saveConfig();
}

extern "C" __declspec(dllexport) void beNotified(SCNotification *notifyCode) {
    switch (notifyCode->nmhdr.code) {
        case NPPN_SHUTDOWN: {
            commandMenuCleanUp();
            break;
        }
        case NPPN_READY: {
            ::SendMessage(nppData._nppHandle, NPPM_SETMENUITEMCHECK,
                          funcItem[FUNC_TOGGLE_CASE_SENSITIVITY]._cmdID,
                          g_caseSensitive);
            break;
        }
        default:
            break;
    }
}

//
// Utilitários de String e Unicode
//
static std::wstring toWString(const char* str, size_t len, UINT codePage) {
    if (!str || len == 0)
        return std::wstring();

    int sizeNeeded = MultiByteToWideChar(codePage, 0, str, static_cast<int>(len), nullptr, 0);
    if (sizeNeeded <= 0)
        return std::wstring();

    std::wstring wstr(sizeNeeded, L'\0');
    MultiByteToWideChar(codePage, 0, str, static_cast<int>(len), &wstr[0], sizeNeeded);
    return wstr;
}

static bool containsSubstr(const std::wstring& text, const std::wstring& pattern, bool caseSensitive) {
    if (pattern.empty())
        return true;
    if (pattern.size() > text.size())
        return false;

    if (caseSensitive) {
        return text.find(pattern) != std::wstring::npos;
    }

    auto it = std::search(
        text.begin(), text.end(),
        pattern.begin(), pattern.end(),
        [](wchar_t c1, wchar_t c2) {
            return towlower(c1) == towlower(c2);
        }
    );
    return it != text.end();
}

//
// Lógica Principal de Filtragem de Linhas
//
static void filterLines(bool keepMatching) {
    int which = -1;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, reinterpret_cast<LPARAM>(&which));
    HWND curScintilla = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;
    if (!curScintilla) return;

    // 1. Obter a seleção ativa (sem restrição artificial de tamanho)
    int selStart = static_cast<int>(::SendMessage(curScintilla, SCI_GETSELECTIONSTART, 0, 0));
    int selEnd = static_cast<int>(::SendMessage(curScintilla, SCI_GETSELECTIONEND, 0, 0));
    if (selStart == selEnd) {
        return; // Nenhuma seleção realizada
    }

    int selBytesWithNull = static_cast<int>(::SendMessage(curScintilla, SCI_GETSELTEXT, 0, 0));
    if (selBytesWithNull <= 1) {
        return;
    }

    std::vector<char> selBuffer(selBytesWithNull);
    ::SendMessage(curScintilla, SCI_GETSELTEXT, 0, reinterpret_cast<LPARAM>(selBuffer.data()));
    std::string keyword(selBuffer.data());

    // 2. Identificar a página de código (CodePage) e modo de quebra de linha (EOL) do buffer ativo
    UINT codePage = static_cast<UINT>(::SendMessage(curScintilla, SCI_GETCODEPAGE, 0, 0));
    if (codePage == 0) {
        codePage = CP_ACP; // ANSI fallback se não for UTF-8 (65001)
    }

    int eolMode = static_cast<int>(::SendMessage(curScintilla, SCI_GETEOLMODE, 0, 0));
    std::string eolStr;
    switch (eolMode) {
        case SC_EOL_CR:   eolStr = "\r"; break;
        case SC_EOL_LF:   eolStr = "\n"; break;
        case SC_EOL_CRLF:
        default:          eolStr = "\r\n"; break;
    }

    std::wstring patternW = toWString(keyword.data(), keyword.size(), codePage);

    // 3. Processar linhas de forma otimizada com buffer reutilizável
    int numLines = static_cast<int>(::SendMessage(curScintilla, SCI_GETLINECOUNT, 0, 0));
    if (numLines <= 0) return;

    int docLength = static_cast<int>(::SendMessage(curScintilla, SCI_GETLENGTH, 0, 0));
    std::string result;
    if (docLength > 0) {
        result.reserve(static_cast<size_t>(docLength));
    }

    std::vector<char> lineBuffer;

    for (int i = 0; i < numLines; ++i) {
        int lineLength = static_cast<int>(::SendMessage(curScintilla, SCI_LINELENGTH, i, 0));
        if (lineLength <= 0) continue;

        if (static_cast<int>(lineBuffer.size()) < lineLength + 1) {
            lineBuffer.resize(lineLength + 1);
        }

        ::SendMessage(curScintilla, SCI_GETLINE, i, reinterpret_cast<LPARAM>(lineBuffer.data()));
        lineBuffer[lineLength] = '\0';

        std::string line(lineBuffer.data(), lineLength);

        // Remover terminadores de linha para a comparação
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) {
            line.pop_back();
        }

        std::wstring lineW = toWString(line.data(), line.size(), codePage);
        bool matched = containsSubstr(lineW, patternW, g_caseSensitive);

        bool keep = keepMatching ? matched : !matched;
        if (keep) {
            result.append(line);
            result.append(eolStr);
        }
    }

    // 4. Aplicar o novo texto encapsulado em bloco de Undo e restaurar posição de visualização
    ::SendMessage(curScintilla, SCI_BEGINUNDOACTION, 0, 0);

    int firstVisibleLine = static_cast<int>(::SendMessage(curScintilla, SCI_GETFIRSTVISIBLELINE, 0, 0));
    int currentPos = static_cast<int>(::SendMessage(curScintilla, SCI_GETCURRENTPOS, 0, 0));

    ::SendMessage(curScintilla, SCI_SETTEXT, 0, reinterpret_cast<LPARAM>(result.c_str()));

    int newDocLength = static_cast<int>(::SendMessage(curScintilla, SCI_GETLENGTH, 0, 0));
    int restoredPos = (currentPos < newDocLength) ? currentPos : newDocLength;
    ::SendMessage(curScintilla, SCI_SETCURRENTPOS, restoredPos, 0);
    ::SendMessage(curScintilla, SCI_SETSELECTIONSTART, restoredPos, 0);
    ::SendMessage(curScintilla, SCI_SETSELECTIONEND, restoredPos, 0);
    ::SendMessage(curScintilla, SCI_SETFIRSTVISIBLELINE, firstVisibleLine, 0);

    ::SendMessage(curScintilla, SCI_ENDUNDOACTION, 0, 0);
}

void removeLinesWithSelection() {
    // Remove linhas que contêm a seleção -> mantém as que NÃO casam
    filterLines(false);
}

void removeLinesWithoutSelection() {
    // Remove linhas que NÃO contêm a seleção -> mantém as que casam
    filterLines(true);
}
