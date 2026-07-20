#!/bin/bash
set -e

# Navega para o diretório raiz do projeto (um nível acima de _scripts)
cd "$(dirname "$0")/.."

# Configuracao da versao do cJSON
CJSON_VERSION="v1.7.18"

# URLs de download
CJSON_H_URL="https://raw.githubusercontent.com/DaveGamble/cJSON/${CJSON_VERSION}/cJSON.h"
CJSON_C_URL="https://raw.githubusercontent.com/DaveGamble/cJSON/${CJSON_VERSION}/cJSON.c"

# Destinos dos arquivos
CJSON_H_DEST="inc/cJSON.h"
CJSON_C_DEST="src/cJSON.c"

# Hash de cada arquivo
CJSON_H_SHA256="0578cc29132912edbc88f83207a8fc76e5db3db0605497e909a9384ef3cc474b"
CJSON_C_SHA256="75c51de8fa40ac9d7a99319c6330719bd692eb81c0a869265f3d4c682533f9b9"

echo "================================================="
echo "Verificando dependências de terceiros..."
echo "================================================="

# Função para baixar e verificar um arquivo se ele não existir
fetch_if_missing() {
    local url=$1
    local dest=$2
    local expected_sha256=$3

    if [ -f "$dest" ]; then
        echo "✅ O arquivo '${dest}' já existe. Pulando o download."
    else
        echo "⏳ Baixando '${dest}'..."
        if ! curl -o "$dest" --fail -L "$url"; then
            echo "❌ ERRO: Falha ao baixar '${dest}'."
            rm -f "$dest" # Limpa o arquivo parcial em caso de falha
            exit 1
        fi
        echo "✅ Download de '${dest}' concluído."

        echo "⚖️  Verificando integridade (checksum) de '${dest}'..."
        
        local downloaded_sha256
        if command -v sha256sum >/dev/null 2>&1; then
            downloaded_sha256=$(sha256sum "$dest" | awk '{print $1}')
        elif command -v shasum >/dev/null 2>&1; then
            downloaded_sha256=$(shasum -a 256 "$dest" | awk '{print $1}')
        else
            echo "❌ ERRO: Nenhuma ferramenta de checksum (sha256sum, shasum) encontrada."
            exit 1
        fi

        if [ "$downloaded_sha256" != "$expected_sha256" ]; then
            echo "❌ ERRO DE SEGURANÇA: O checksum do arquivo baixado não corresponde ao esperado!"
            echo "   - Esperado:   ${expected_sha256}"
            echo "   - Recebido:   ${downloaded_sha256}"
            rm -f "$dest" # DELETA o arquivo suspeito
            exit 1
        fi
        
        echo "✅ Integridade de '${dest}' confirmada."
    fi
}

fetch_if_missing "$CJSON_H_URL" "$CJSON_H_DEST" "$CJSON_H_SHA256"
fetch_if_missing "$CJSON_C_URL" "$CJSON_C_DEST" "$CJSON_C_SHA256"

echo ""
echo "================================================="
echo "Todas as dependências foram satisfeitas!"
echo "================================================="
