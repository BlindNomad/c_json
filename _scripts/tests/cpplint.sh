#!/bin/bash

set -e

echo -e "🚀 Executando Cpplint:"
echo -e "      - Ignora whitespace/tab"
echo -e "      - Ignora casting"
echo -e "      - Ignora comentarios de multiplas linhas"
echo -e "      - Ignora a necessidade de por subdir em includes"
echo -e "🗒️  Logs da aplicação:"

cpplint \
  --filter=-whitespace/tab,-readability/casting,-readability/multiline_comment,-build/include_subdir \
  --linelength=160 \
  --exclude=src/cJSON.c \
  --exclude=inc/cJSON.h \
  inc/* src/*

# Corrigido o nome da ferramenta na mensagem de conclusão
echo -e "✅ Análise do Cpplint concluída."
