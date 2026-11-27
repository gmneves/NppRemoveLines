# NppRemoveLines

[![Build and Release](https://github.com/gmneves/NppRemoveLines/actions/workflows/build.yml/badge.svg)](https://github.com/gmneves/NppRemoveLines/actions/workflows/build.yml)
[![GitHub Release](https://img.shields.io/github/v/release/gmneves/NppRemoveLines?color=blue)](https://github.com/gmneves/NppRemoveLines/releases/latest)
[![License: GPL v2](https://img.shields.io/badge/License-GPLv2-green.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html)
[![Notepad++](https://img.shields.io/badge/Notepad%2B%2B-Plugin-success)](https://notepad-plus-plus.org/)

Um plugin rápido e leve para **Notepad++** que permite filtrar e remover linhas do documento ativo com base no texto selecionado.

---

## 🚀 Funcionalidades

- **Remover linhas que CONTÊM a seleção**: Remove todas as linhas do documento que possuem o termo selecionado.
- **Remover linhas que NÃO CONTÊM a seleção**: Mantém apenas as linhas que contêm o termo selecionado (excluindo todas as demais).
- **Sensibilidade a Maiúsculas/Minúsculas (*Case Sensitive*)**: Opção alternável no menu (com preferência persistida em `%APPDATA%\NppRemoveLines\NppRemoveLines.ini`).
- **Suporte Seguro a Unicode / UTF-8**: Comparação sem distorcer caracteres acentuados ou multibyte em buffers UTF-8 ou ANSI.
- **Histórico de Undo Íntegro**: Ação de remoção agrupada em uma única operação de desfecho (`Ctrl + Z`), com restauração suave do cursor e da visualização.
- **Posicionamento Inteligente do Cursor**:
  - Ao *manter* linhas filtradas, o cursor permanece na mesma linha e coluna original.
  - Ao *remover* a linha onde o cursor estava, ele é posicionado automaticamente na linha adjacente mais próxima (abaixo ou acima).
- **Respeito ao padrão de quebras de linha**: Detecta e preserva o padrão de término de linha (`CRLF`, `LF` ou `CR`) definido no arquivo.

---

## ⌨️ Atalhos de Teclado

O plugin define os seguintes atalhos padrão:

| Comando | Atalho Padrão | Descrição |
| :--- | :--- | :--- |
| **Remover linhas que CONTÊM a seleção** | `Ctrl + Alt + R` | Remove as linhas que possuem o termo selecionado |
| **Remover linhas que NÃO CONTÊM a seleção** | `Ctrl + Alt + Shift + R` | Mantém apenas as linhas que contêm o termo |

> [!WARNING]
> **Atenção sobre o atalho `Ctrl + Alt + R` no Notepad++:**
>
> Por padrão, o Notepad++ utiliza a combinação `Ctrl + Alt + R` para o comando nativo **"Direção do texto da direita para a esquerda" (*Right-to-Left text direction*)**.
>
> Se você pressionar `Ctrl + Alt + R` e o texto for alinhado à direita em vez de executar o plugin, é necessário liberar esse atalho no Notepad++:
>
> 1. No menu superior, vá em **Configurações (*Settings*) > Mapeador de atalhos... (*Shortcut Mapper...*)**.
> 2. Na aba **Comandos principais (*Main menu*)**, localize a linha do comando **Direção do texto da direita para a esquerda (*Right-to-Left*)**.
> 3. Clique duas vezes nele e altere o atalho para **Nenhum** (ou atribua outra tecla).
> 4. Na aba **Comandos de plug-in (*Plugin commands*)**, verifique se o comando do **NppRemoveLines** está com `Ctrl + Alt + R` atribuído.

---

## 🖱️ Menu de Contexto (Botão Direito do Mouse)

Para acessar as opções do plugin diretamente pelo botão direito do mouse sobre o texto (sem precisar navegar pelo menu superior), você pode adicioná-las ao arquivo de configuração de menu do Notepad++:

1. Feche o **Notepad++**.
2. Abra o arquivo `%APPDATA%\Notepad++\contextMenu.xml` (ou `Win + R` e digite `%APPDATA%\Notepad++\contextMenu.xml`).
3. Adicione o seguinte trecho dentro da tag `<ScintillaContextMenu>` (por exemplo, logo após a linha do comando `Select all` ou `Begin/End Select`):

```xml
        <!-- NppRemoveLines plugin commands -->
        <Item id="0"/>
        <Item PluginEntryName="NppRemoveLines" PluginCommandItemName="Remover linhas que CONTÊM a seleção" ItemNameAs="Remover linhas com a seleção" />
        <Item PluginEntryName="NppRemoveLines" PluginCommandItemName="Remover linhas que NÃO CONTÊM a seleção" ItemNameAs="Remover linhas sem a seleção" />
        <Item PluginEntryName="NppRemoveLines" PluginCommandItemName="Case Sensitive" ItemNameAs="Case Sensitive" />
        <Item id="0"/>
```

4. Salve o arquivo e abra o Notepad++. As opções aparecerão diretamente ao clicar com o botão direito sobre qualquer texto selecionado.

---

## 📥 Download e Instalação

1. Baixe a DLL correspondente à arquitetura do seu Notepad++ na página de [**Releases**](https://github.com/gmneves/NppRemoveLines/releases/latest):
   - **64-bit:** `NppRemoveLines-x64.dll`
   - **32-bit:** `NppRemoveLines-x86.dll`
2. No diretório de plugins do seu Notepad++ (normalmente `C:\Program Files\Notepad++\plugins\` para 64-bit ou `C:\Program Files (x86)\Notepad++\plugins\` para 32-bit), crie uma subpasta com o nome exato `NppRemoveLines`.
3. Renomeie o arquivo baixado para `NppRemoveLines.dll` e coloque-o dentro dessa pasta:
   ```text
   Notepad++\plugins\NppRemoveLines\NppRemoveLines.dll
   ```
4. Reinicie o Notepad++. O plugin aparecerá no menu **Plugins > NppRemoveLines**.

---

## 🛠️ Compilação

### Via GitHub Actions (Recomendado)
O repositório conta com CI/CD automatizado via GitHub Actions. A cada commit na branch `main` ou criação de tag (`v*`), as versões **x64** e **x86** são compiladas e disponibilizadas automaticamente nos Releases e Artifacts.

### Compilação Local (MinGW-w64)
Necessário ter o compilador `g++` (com suporte a C++17) e a ferramenta `make` disponíveis no PATH:

```bash
# Compilar a DLL
make

# Limpar arquivos intermediários
make clean
```

---

## 📄 Licença

Este projeto é software livre, distribuído sob os termos da licença **GNU General Public License v2** (ou posterior). Consulte os arquivos do código para maiores detalhes.
