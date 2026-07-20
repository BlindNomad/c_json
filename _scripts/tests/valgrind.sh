#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")/../.."

# Ferramentas do venv do projeto (cmake, gcovr)
if [ -d ".venv/bin" ]; then
    export PATH="$(pwd)/.venv/bin:${PATH}"
fi

BUILD_DIR="build/linux"
BUILD_TYPE="Debug"
TEST_BIN="${BUILD_DIR}/c_json_tests"

if [ ! -x "${TEST_BIN}" ]; then
    echo "================================================="
    echo "Binario de testes nao encontrado. Compilando..."
    echo "================================================="
    bash ./_scripts/fetch_deps.sh
    mkdir -p "${BUILD_DIR}"
    cd "${BUILD_DIR}"
    cmake ../.. -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
    cmake --build . --parallel
    cd ../..
fi

if ! command -v valgrind >/dev/null 2>&1; then
    echo "ERRO: valgrind nao encontrado. Instale com: sudo apt install valgrind"
    exit 1
fi

echo "================================================="
echo "Executando testes com Valgrind..."
echo "================================================="

valgrind --leak-check=full \
    --show-leak-kinds=all \
    --errors-for-leak-kinds=definite,indirect \
    --error-exitcode=1 \
    "${TEST_BIN}"

echo "================================================="
echo "Valgrind concluido sem vazamentos de memoria."
echo "================================================="
