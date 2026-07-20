#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")/../.."

# Ferramentas do venv do projeto (cmake, gcovr)
if [ -d ".venv/bin" ]; then
    export PATH="$(pwd)/.venv/bin:${PATH}"
fi

BUILD_DIR="build/fuzz"
BUILD_TYPE="Debug"
FUZZ_BIN="${BUILD_DIR}/fuzz_parser"
CORPUS_DIR="fuzz/corpus"
FUZZ_TIME="${FUZZ_TIME:-60}"

if [ -x "${HOME}/.local/clang-18/usr/bin/clang" ]; then
    export PATH="${HOME}/.local/clang-18/usr/bin:${PATH}"
    export LD_LIBRARY_PATH="${HOME}/.local/clang-18/usr/lib/llvm-18/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"
fi

if ! command -v clang >/dev/null 2>&1; then
    echo "ERRO: clang nao encontrado. Instale com: sudo apt install clang"
    exit 1
fi

if [ ! -x "${FUZZ_BIN}" ]; then
    echo "================================================="
    echo "Binario de fuzzing nao encontrado. Compilando..."
    echo "================================================="
    bash ./_scripts/fetch_deps.sh
    CC=clang CXX=clang++ cmake -B "${BUILD_DIR}" -DENABLE_FUZZING=ON -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" -DBUILD_TESTS=OFF
    cmake --build "${BUILD_DIR}" --parallel
fi

echo "================================================="
echo "Executando fuzzing (libFuzzer + ASan/UBSan)..."
echo "Corpus: ${CORPUS_DIR}"
echo "Tempo maximo: ${FUZZ_TIME}s"
echo "================================================="

"${FUZZ_BIN}" "${CORPUS_DIR}/" -max_total_time="${FUZZ_TIME}"

echo "================================================="
echo "Fuzzing concluido sem erros."
echo "================================================="
