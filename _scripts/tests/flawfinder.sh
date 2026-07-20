#!/bin/bash

set -e

echo "🚀 Executando Flawfinder para análise de segurança..."
echo "🗒️  Logs da aplicação:"

find src inc -type f \( -name "*.c" -o -name "*.h" \) \
  ! -name "cJSON.c" ! -name "cJSON.h" \
  | xargs flawfinder --context --columns --quiet

echo "✅ Análise do Flawfinder concluída."
