#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#pragma once

#include "includes.h"

enum E_TIPO_DE_OPERACAO
{
	DESCONHECIDO	= -1,
	PESQUISA		= 0,
	VALIDACAO		= 1
};

enum E_TIPO_DE_PROCURA
{
	EXATO		= 0,
	COMECA		= 1,
	TERMINA		= 2,
	CONTEM		= 3
};

typedef struct s_expr
{
	char texto[ TAMANHO_MAX_LINHA ];
	int num_ocorrencias;
} Expr;

typedef struct s_arquivo
{													//(x86)
	char	*texto;									//0x0000
	char	*tag;									//0x0004
	char	nome[ TAMANHO_MAX_NOME_ARQUIVO ];		//0x0008 ~ 0x106
	char	diretorio[ TAMANHO_MAX_CAMINHO ];		//0x0107 ~ 0x20A
	int		tamanho_linha;							//0x020B
	int		qnt_total_linhas;						//0x020C
	Expr	*expr;									//0x020F
	int		qnt_tags_calculo;						//0x0213
} Arquivo;

typedef struct s_cmdline
{
	signed int tipo_de_operacao;
	int qnt_arquivos_lidos;
	int qnt_tags_entrada;
	int tipo_de_busca[ QNT_MAX_TAGS ];
	char tag[ QNT_MAX_TAGS ][ TAMANHO_MAX_LINHA ];
	char diretorio[ TAMANHO_MAX_CAMINHO ];
	char arquivos[ QNT_MAX_ARQUIVOS ][ TAMANHO_MAX_NOME_ARQUIVO ];
} Cmd;

#endif