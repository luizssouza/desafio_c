#ifndef VALIDACAO_H
#define VALIDACAO_H

#pragma once

#include "includes.h"

Arquivo *localizar_arquivo( Arquivo **entrada, char *nome );
bool validar_calculo( Arquivo **arquivo, char *tag );
bool tratar_cmdline( int argc, char *argv[ ], Cmd *cmd );
void exibir_ajuda( void );

#endif