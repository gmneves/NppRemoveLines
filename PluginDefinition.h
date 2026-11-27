#ifndef PLUGINDEFINITION_H
#define PLUGINDEFINITION_H

#include "PluginInterface.h"

//
// Nome do plugin
//
const TCHAR NPP_PLUGIN_NAME[] = TEXT("NppRemoveLines");

//
// Identificadores de comandos
//
enum {
    FUNC_REMOVE_LINES = 0,
    FUNC_REMOVE_LINES_NOT,
    FUNC_TOGGLE_CASE_SENSITIVITY,
    nbFunc
};

//
// Ciclo de vida do plugin
//
void pluginInit(HANDLE hModule);
void pluginCleanUp();
void commandMenuInit();
void commandMenuCleanUp();
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk = NULL, bool checkOnInit = false);

//
// Funções associadas aos comandos do menu
//
void removeLinesWithSelection();
void removeLinesWithoutSelection();
void toggleCaseSensitive();

#endif // PLUGINDEFINITION_H
