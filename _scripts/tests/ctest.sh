#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")/../.."

# Ferramentas do venv do projeto (cmake, gcovr)
if [ -d ".venv/bin" ]; then
    export PATH="$(pwd)/.venv/bin:${PATH}"
fi

BUILD_DIR="build/linux"
BUILD_TYPE="Debug"

echo "================================================="
echo "Compilando o projeto e os testes para Linux (${BUILD_TYPE})"
echo "================================================="

bash ./_scripts/fetch_deps.sh
bash ./_scripts/builds/linux.sh "${BUILD_TYPE}"

echo "================================================="
echo "Executando os testes com CTest..."
echo "================================================="
cd "${BUILD_DIR}"
ctest --output-on-failure --verbose

echo "================================================="
echo "Execucao dos testes concluida."
echo "================================================="
