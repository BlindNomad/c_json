#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")/../.."

BUILD_DIR="build/linux"
BUILD_TYPE=${1:-Release}

echo "================================================="
echo "Iniciando build para Linux"
echo "Tipo de Build: ${BUILD_TYPE}"
echo "Diretorio de Build: ${BUILD_DIR}"
echo "================================================="

bash ./_scripts/fetch_deps.sh

mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

cmake ../.. -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
cmake --build . --parallel

echo "================================================="
echo "Build para Linux concluido com sucesso!"
echo "Artefatos em: $(pwd)"
echo "================================================="
