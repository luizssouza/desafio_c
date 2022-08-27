#ifndef PESQUISA_H
#define PESQUISA_H

#pragma once

#include "includes.h"

bool procurar_tag( Arquivo *cabeca, char *tag, int tipo_de_procura );
bool procurar_tags_calculo( Arquivo **arquivo, char *tag, bool validar );
Arquivo *ler_arquivo_texto( char *diretorio, char *nome );
Arquivo *acessar_linha_por_indice( Arquivo *entrada, int indice );
Expr *acessar_expressao_por_indice( Expr *entrada, int indice );
void destruir_arquivo( Arquivo *cabeca );
void extrair_string( char *entrada, char *token, char *saida );
bool comeca_com( const char *str, const char *pre );
bool termina_com( const char *s, const char *t );
bool existe_duplicado_str( char **vet_string, int tamanho );

#endif