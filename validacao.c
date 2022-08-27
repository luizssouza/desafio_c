#include "includes.h"

bool validar_calculo( Arquivo **arquivos, char *tag )
{
	char tag_buffer[ TAMANHO_MAX_LINHA ] = { 0 };
	int num_ocorrencias[ 2 ]			 = { 0 };

	//concatenando tag com # (# tag).
	sprintf( tag_buffer, "# %s", tag );

	printf( "i.\tDADOS DO CALCULO:\n" );
	for( int i = 0; i < QNT_MAX_ARQUIVOS; i++ )
		if( procurar_tag( arquivos[ i ], tag, EXATO ) )
			num_ocorrencias[ 0 ]++;

	if( !num_ocorrencias[ 0 ] )
		return false;

	printf( "ii.\tEXPRESSAO:\n" );
	//procurando expressao através da tag dentro do arquivo de expressoes.
	for( int i = 0; i < QNT_MAX_ARQUIVOS; i++ )
		if( procurar_tag( arquivos[ i ], tag_buffer, EXATO ) )
			num_ocorrencias[ 1 ]++;

	if( !num_ocorrencias[ 1 ] )
		return false;

	printf( "\niii.\tTAGS DA EXPRESSAO:\n" );
	//procurando tags de cálculo dentro dos arquivos.
	if( procurar_tags_calculo( arquivos, tag_buffer, false ) )
	{
		printf( "iv.\tVALIDACAO:\n" );
		//procurando tags de cálculo dentro dos arquivos.
		if( procurar_tags_calculo( arquivos, tag_buffer, true ) )
			printf( "EXPRESSAO VALIDA!\n" );
		else
			printf( "EXPRESSAO INVALIDA!\n" );
	}

	return true;
}

bool tratar_cmdline( int argc, char *argv[ ], Cmd *cmd )
{
	bool existe_repeticoes = false;

	char *diretorio		= argv[ 1 ];
	char *operacao		= argv[ 2 ];
	char *arquivos		= argv[ 3 ];
	char *tag_procura	= argv[ 4 ];

	//Exibe ajuda caso o número de parâmetros seja inferior ou maior que 5.
	if( argc != 5 )
	{
		exibir_ajuda( );

		return false;
	}

	//Não permite que a string do diretório seja superior a 4096 caracteres.
	//Restrição do próprio SO Linux.
	if( strlen( diretorio ) <= TAMANHO_MAX_CAMINHO )
		strcpy( cmd->diretorio, diretorio );
	else
	{
		printf( "O tamanho do diretorio e superior a %i caracteres\n", TAMANHO_MAX_CAMINHO );
		return false;
	}

	//Validações do tipo de operações
	if( !strcmp( operacao, "pesq" ) )
		cmd->tipo_de_operacao = PESQUISA;
	else if( !strcmp( operacao, "valid" ) )
		cmd->tipo_de_operacao = VALIDACAO;
	else
	{
		printf( "Operacao %s invalida!\n", operacao );
		return false;
	}

	//tratanto os dados dos arquivos de entrada separado por ','
	char *texto_buffer = ( char * ) calloc( strlen( arquivos ) + 1, sizeof( char ) );
	strcpy( texto_buffer, arquivos );
	char *texto_limpo = strtok( texto_buffer, "," );

	for( ; cmd->qnt_arquivos_lidos < QNT_MAX_ARQUIVOS && texto_limpo != NULL; cmd->qnt_arquivos_lidos++, texto_limpo = strtok( NULL, "," ) )
		if( strlen( texto_limpo ) <= TAMANHO_MAX_NOME_ARQUIVO )
			strcpy( cmd->arquivos[ cmd->qnt_arquivos_lidos ], texto_limpo );
		else
		{
			printf( "O tamanho do nome do arquivo %s e superior a %i caracteres\n", texto_limpo, TAMANHO_MAX_NOME_ARQUIVO );
			return false;
		}

	free( texto_buffer );

	//verificando se foi inserido algum arquivo duplicado.
	for( int i = 0; i <= cmd->qnt_arquivos_lidos; i++ )
		for( int j = i + 1; j < cmd->qnt_arquivos_lidos; j++ )
			if( !strcmp( cmd->arquivos[ i ], cmd->arquivos[ j ] ) )
				existe_repeticoes = true;

	if( existe_repeticoes )
	{
		printf( "Voce entrou com algum arquivo mais de uma vez!\n" );
		printf( "Reabra o programa e tente novamente!\n" );
		return false;
	}

	if( cmd->qnt_arquivos_lidos < ( QNT_MAX_ARQUIVOS - 1 ) )
	{
		printf( "\n\n<ATENCAO>\n\tVoce nao abriu todos os %i arquivos\n", QNT_MAX_ARQUIVOS );
		printf( "\tpode ser que a busca nao aconteca de forma adequada\n\n\n" );
	}

	//tratando as tags separadas por ',' e as operações de entrada, possibilitando fazer mais de uma pesquisa ao mesmo tempo.
	texto_buffer = ( char * ) calloc( strlen( tag_procura ) + 1, sizeof( char ) );
	strcpy( texto_buffer, tag_procura );
	texto_limpo = strtok( texto_buffer, "," );

	for( ; cmd->qnt_tags_entrada < QNT_MAX_ARQUIVOS && texto_limpo != NULL; cmd->qnt_tags_entrada++, texto_limpo = strtok( NULL, "," ) )
	{
		if( strlen( texto_limpo ) <= TAMANHO_MAX_NOME_ARQUIVO )
		{
			if( !comeca_com( texto_limpo, "*" ) && termina_com( texto_limpo, "*" ) )
				cmd->tipo_de_busca[ cmd->qnt_tags_entrada ] = COMECA;
			else if( comeca_com( texto_limpo, "*" ) && !termina_com( texto_limpo, "*" ) )
				cmd->tipo_de_busca[ cmd->qnt_tags_entrada ] = TERMINA;
			else if( comeca_com( texto_limpo, "*" ) && termina_com( texto_limpo, "*" ) )
				cmd->tipo_de_busca[ cmd->qnt_tags_entrada ] = CONTEM;
			else
				cmd->tipo_de_busca[ cmd->qnt_tags_entrada ] = EXATO;

			strcpy( cmd->tag[ cmd->qnt_tags_entrada ], texto_limpo );
		}
		else
		{
			printf( "O tamanho do nome do arquivo %s e superior a %i caracteres\n", texto_limpo, TAMANHO_MAX_NOME_ARQUIVO );
			return false;
		}
	}

	free( texto_buffer );

	//removendo algum asterisco da string.
	for( int i = 0; i < cmd->qnt_tags_entrada; i++ )
		extrair_string( cmd->tag[ i ], "*", cmd->tag[ i ] );

	return true;
}

void exibir_ajuda( void )
{
	printf( "Quantidade de parametros invalidos!\n\n" );
	printf( "========== AJUDA ==========\n" );
	printf( "** Para realizar uma pesquisa de calculo:\n" );
	printf( "\t[caminho] [operacao] [arquivos] [tags]\n" );
	printf( "** Exemplo:\n" );
	printf( "\t/home/linux/Arquivos pesq 'calana.txt,caldig.txt,caldsc.txt,entana.txt,entdig.txt,entdsc.txt,expcal.txt,varana.txt,vardig.txt,vardsc.txt' '*DVLD_INTER_CD*,*_CD'\n" );
	printf( "\n\n" );
	printf( "** Para realizar uma validacao de calculo:\n" );
	printf( "\t[caminho] [operacao] [arquivos] [tags]\n" );
	printf( "** Exemplo:\n" );
	printf( "\t/home/linux/Arquivos valid 'calana.txt,caldig.txt,caldsc.txt,entana.txt,entdig.txt,entdsc.txt,expcal.txt,varana.txt,vardig.txt,vardsc.txt' 'DVLD_INTER_CD'\n" );
}
