#!/bin/bash
# Este script remove o diretório de build para uma limpeza completa.

# Para a execução se um comando falhar
set -e

# Encontra o diretório raiz do projeto
PROJECT_ROOT=$(git rev-parse --show-toplevel)
BUILD_DIR="${PROJECT_ROOT}/build"

if [ -d "${BUILD_DIR}" ]; then
    echo "Limpando diretório de build: ${BUILD_DIR}"
    rm -rf "${BUILD_DIR}"
    echo "Limpeza concluída."
else
    echo "Diretório de build não encontrado. Nada a fazer."
fi
