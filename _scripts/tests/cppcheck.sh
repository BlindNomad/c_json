#!/bin/bash

set -e

echo -e "🚀 Executando Cppcheck:"
echo -e "      - Ignorando recomendação de Const em parametros de ponteiros"
echo -e "      - Ignorando recomendação de Const em variaveis de ponteiros"
echo -e "      - Ignorando recomendação de inserção de includes \n"
echo -e "🗒️  Logs da aplicação:"

cppcheck \
    --enable=all \
    --error-exitcode=1 \
    --suppress=missingIncludeSystem \
    --suppress=unusedFunction \
    --suppress=constParameterPointer \
    --suppress=constVariablePointer \
    --suppress=unmatchedSuppression \
    -I inc \
    -i src/cJSON.c \
    src/

echo -e "✅ Análise do Cppcheck concluída."
