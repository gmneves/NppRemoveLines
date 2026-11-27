# NppRemoveLines

Um plugin para **Notepad++** que permite filtrar e remover rapidamente linhas do documento ativo com base no texto selecionado.

## 🚀 Funcionalidades

- **Remover linhas que CONTÊM a seleção** (`Ctrl + Alt + R`): Remove todas as linhas do documento que possuem o termo selecionado.
- **Remover linhas que NÃO CONTÊM a seleção** (`Ctrl + Alt + Shift + R`): Mantém apenas as linhas que contêm o termo selecionado (excluindo as demais).
- **Atalhos customizáveis**: Todos os atalhos podem ser alterados em **Configurações > Mapeador de atalhos... > Comandos de plug-in**.
- **Menu de contexto (botão direito)**: Suporte a inclusão no menu de contexto do editor via `contextMenu.xml`.
- **Case Sensitive**: Opção no menu para ativar ou desativar diferenciação entre maiúsculas e minúsculas (preferência persistida em `%APPDATA%\NppRemoveLines\NppRemoveLines.ini`).
- **Suporte Seguro a Unicode / UTF-8**: Comparação sem distorcer caracteres acentuados ou multibyte.
- **Histórico de Undo Íntegro**: Ação de remoção agrupada em uma única operação de desfecho (`Ctrl+Z`), preservando o cursor e a visualização do documento.
- **Respeito ao padrão de quebras de linha**: Detecta e preserva o padrão de término de linha (`CRLF`, `LF` ou `CR`) definido no arquivo.

## 📥 Instalação

1. Baixe o `NppRemoveLines.dll` (versão 32-bit ou 64-bit de acordo com a sua versão do Notepad++).
2. No diretório de plugins do Notepad++ (geralmente `C:\Program Files\Notepad++\plugins\` ou `%APPDATA%\Notepad++\plugins\`), crie uma pasta chamada `NppRemoveLines`.
3. Copie o arquivo `NppRemoveLines.dll` para dentro dessa pasta:
   ```text
   Notepad++\plugins\NppRemoveLines\NppRemoveLines.dll
   ```
4. Reinicie o Notepad++. O plugin aparecerá no menu **Plugins > NppRemoveLines**.

## 🛠️ Compilação

### Via GitHub Actions
Cada push ou pull request na branch `main` compila automaticamente as versões x64 e x86 no GitHub Actions, disponibilizando os binários na aba **Actions** (Artifacts).

### Compilação Local (MinGW-w64)
Necessário ter o compilador `g++` e a ferramenta `make` no PATH:
```bash
make
```

Para limpar os arquivos intermediários:
```bash
make clean
```

## 📄 Licença
Distribuído sob a licença GNU General Public License v2 (ou superior).
