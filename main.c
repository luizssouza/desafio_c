#include "includes.h"

int main( int argc, char *argv[ ] )
{
	Cmd *cmd = calloc( 1, sizeof( Cmd ) );

	if( !tratar_cmdline( argc, argv, cmd ) )
		return 0;

	//correcao para o indice quando o valor for 0.
	//correcao necessária para o laço de repetição abaixo (i = 0; i < 0; i++).
	if( !cmd->qnt_arquivos_lidos )
		 cmd->qnt_arquivos_lidos++;

	if( !cmd->qnt_tags_entrada )
		 cmd->qnt_tags_entrada++;

	Arquivo *lista_de_arquivos[ QNT_MAX_ARQUIVOS ] = { 0 };

	for( int i = 0; i < cmd->qnt_arquivos_lidos; i++ )
		lista_de_arquivos[ i ] = ler_arquivo_texto( cmd->diretorio, cmd->arquivos[ i ] );

	if( cmd->tipo_de_operacao == PESQUISA )
	{
		for( int i = 0; i < cmd->qnt_arquivos_lidos; i++ )
			for( int j = 0; j < cmd->qnt_tags_entrada; j++ )
				procurar_tag( lista_de_arquivos[ i ], cmd->tag[ j ], cmd->tipo_de_busca[ j ] );
	}
	else
		for( int i = 0; i < cmd->qnt_tags_entrada; i++ )
			validar_calculo( lista_de_arquivos, cmd->tag[ i ] );

	//Limpando memória alocada
	for( int i = 0; i < cmd->qnt_arquivos_lidos; i++ )
		destruir_arquivo( lista_de_arquivos[ i ] );
	
	free( cmd );

	return 0;
}

