#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")/../.."

# Ferramentas do venv do projeto (cmake, gcovr)
if [ -d ".venv/bin" ]; then
    export PATH="$(pwd)/.venv/bin:${PATH}"
fi

BUILD_DIR="build/linux"
BUILD_TYPE="Debug"
PROJECT_ROOT="$(pwd)"

echo "================================================="
echo "Compilando o projeto e os testes para Linux (Debug + cobertura)"
echo "================================================="

bash ./_scripts/fetch_deps.sh

mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

cmake ../.. -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" -DENABLE_COVERAGE=ON
cmake --build . --parallel

echo "================================================="
echo "Executando os testes..."
echo "================================================="
./c_json_tests

cd "${PROJECT_ROOT}"
mkdir -p coverage/html

echo "================================================="
echo "Gerando relatorio de cobertura (minimo 90%)..."
echo "================================================="

if command -v gcovr >/dev/null 2>&1; then
    GCOVR=gcovr
elif [ -x ".venv/bin/gcovr" ]; then
    GCOVR=".venv/bin/gcovr"
else
    echo "ERRO: gcovr nao encontrado. Instale com: pip install gcovr"
    exit 1
fi

GCOVR_ARGS=(
    --gcov-ignore-parse-errors
    --root .
    --gcov-object-directory "${BUILD_DIR}"
    --filter 'src/c_json.c'
    --filter 'src/c_json_list.c'
    --filter 'src/c_json_mem.c'
    --exclude-unreachable-branches
    --exclude-lines-by-pattern '^\s*[A-Za-z_][A-Za-z0-9_]*:\s*$'
)

"${GCOVR}" "${GCOVR_ARGS[@]}" \
    --xml-pretty \
    --print-summary \
    --fail-under-line 90 \
    -o coverage/cobertura-coverage.xml

"${GCOVR}" "${GCOVR_ARGS[@]}" \
    --html --html-details \
    -o coverage/html/coverage-report.html

echo "================================================="
echo "Cobertura OK (>= 90%). Relatorio em coverage/html/"
echo "================================================="
