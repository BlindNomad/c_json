#!/bin/bash

set -e

# --- CONFIGURAÇÃO MANUAL DO NDK ---
# ATENÇÃO: Modifique esta linha com o caminho absoluto para o seu Android NDK.
NDK_PATH="/home/gustavo/ndk/android-ndk-r28c"

# --- Validação do Caminho do NDK ---
if [ ! -d "$NDK_PATH" ]; then
    echo "Erro: O diretório do Android NDK não foi encontrado em '${NDK_PATH}'."
    echo "Por favor, edite o script e defina o caminho correto na variável NDK_PATH."
    exit 1
fi

# Navega para o diretório raiz do projeto (dois níveis acima de _scripts/builds).
cd "$(dirname "$0")/../.."

# --- Configuração das Variáveis de Build ---
# O tipo de build pode ser passado como primeiro argumento (ex: ./android.sh Debug)
BUILD_TYPE=${1:-Release}
# Lista de todas as ABIs a serem compiladas.
ABIS=("armeabi-v7a" "arm64-v8a" "x86" "x86_64")
# Nível mínimo da API do Android.
ANDROID_PLATFORM="21"
# Caminho para o arquivo de toolchain do NDK.
TOOLCHAIN_FILE="${NDK_PATH}/build/cmake/android.toolchain.cmake"

echo "================================================="
echo "Iniciando build para todas as ABIs do Android"
echo "Tipo de Build: ${BUILD_TYPE}"
echo "NDK Path: ${NDK_PATH}"
echo "================================================="

# --- Loop de Build para cada ABI ---
for ABI in "${ABIS[@]}"
do
    BUILD_DIR="build/android/${ABI}"

    echo ""
    echo "-------------------------------------------------"
    echo "Construindo para ABI: ${ABI}"
    echo "Diretório de Build: ${BUILD_DIR}"
    echo "-------------------------------------------------"

    # 1. Executa o CMake para configurar o projeto para a ABI atual.
    cmake -B "${BUILD_DIR}" -S . \
        -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN_FILE}" \
        -DANDROID_ABI="${ABI}" \
        -DANDROID_PLATFORM="${ANDROID_PLATFORM}" \
        -DCMAKE_BUILD_TYPE=${BUILD_TYPE}

    # 2. Executa o build (compilação) usando todos os cores disponíveis.
    cmake --build "${BUILD_DIR}" --parallel

    echo "Build para ${ABI} concluído com sucesso!"
done

echo ""
echo "================================================="
echo "Build para Android concluído para todas as ABIs!"
echo "Você pode encontrar os artefatos em: build/android/"
echo "================================================="
