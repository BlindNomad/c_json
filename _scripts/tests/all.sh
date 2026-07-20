#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")/../.."

# Ferramentas do venv do projeto (cmake, gcovr)
if [ -d ".venv/bin" ]; then
    export PATH="$(pwd)/.venv/bin:${PATH}"
fi

FUZZ_TIME="${FUZZ_TIME:-60}"

# Clang portatil (ex.: extraido em ~/.local/clang-18) para ambientes sem pacote de sistema
if [ -x "${HOME}/.local/clang-18/usr/bin/clang" ]; then
    export PATH="${HOME}/.local/clang-18/usr/bin:${PATH}"
    export LD_LIBRARY_PATH="${HOME}/.local/clang-18/usr/lib/llvm-18/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"
fi

echo "================================================="
echo "Bateria local: coverage + Valgrind + fuzz"
echo "================================================="

if ! command -v valgrind >/dev/null 2>&1; then
    echo "ERRO: valgrind nao encontrado. Instale com: sudo apt install valgrind"
    exit 1
fi

if ! command -v clang >/dev/null 2>&1; then
    echo "ERRO: clang nao encontrado. Instale com: sudo apt install clang"
    exit 1
fi

echo ""
echo ">>> [1/3] Cobertura (GTest + gcovr, minimo 90%)"
bash ./_scripts/tests/coverage.sh

echo ""
echo ">>> [2/3] Valgrind (leak-check)"
bash ./_scripts/tests/valgrind.sh

echo ""
echo ">>> [3/3] Fuzzing (libFuzzer + ASan/UBSan, ${FUZZ_TIME}s)"
FUZZ_TIME="${FUZZ_TIME}" bash ./_scripts/tests/fuzz.sh

echo ""
echo "================================================="
echo "Bateria local concluida com sucesso."
echo "Relatorio HTML: coverage/html/coverage-report.html"
echo "================================================="
