#include "includes.h"

/* Localiza determinada(s) linha(s) dentro da lista de arquivos atrav�s de seu indice.
par�metros:
	entrada = cabeca do arquivo (endereco de mem�ria inicial da alocac�o).
	indice	= indice da linha a ser procurada.
retorno:
	Se a rotina for executada com sucesso retorna um ponteiro para a linha encontrada,
	caso contr�rio retorna 0x00 (NULL).
*/
Arquivo *acessar_linha_por_indice( Arquivo *entrada, int indice )
{
	return ( Arquivo * ) ( ( unsigned long long ) entrada + ( indice * sizeof( Arquivo ) ) );
}

/* Localiza determinada(s) express�o(�es) dentro da lista de arquivos atrav�s de seu indice.
par�metros:
	entrada = cabeca da lista de express�es (endereco de mem�ria inicial da alocac�o).
	indice	= indice da linha a ser procurada.
retorno:
	Se a rotina for executada com sucesso retorna um ponteiro para a linha encontrada,
	caso contr�rio retorna 0x00 (NULL).
*/
Expr *acessar_expressao_por_indice( Expr *entrada, int indice )
{
	return ( Expr * ) ( ( unsigned long long ) entrada + ( indice * sizeof( Expr ) ) );
}

bool procurar_tag( Arquivo *cabeca, char *tag, int tipo_de_procura )
{
	bool retorno = false;

	if( cabeca == NULL )
		return false;

	//iterando entre as linhas do arquivo.
	for( int i = 0; i < cabeca->qnt_total_linhas; i++ )
	{
		Arquivo *arqv_auxiliar = acessar_linha_por_indice( cabeca, i );

		//verificando validade do ponteiro.
		if( arqv_auxiliar == NULL )
			continue;

		//verificando validade da tag.
		if( arqv_auxiliar->tag == NULL )
			continue;

		if( tipo_de_procura == EXATO   && !strcmp( arqv_auxiliar->tag, tag ) ||
			tipo_de_procura == CONTEM  &&  strstr( arqv_auxiliar->tag, tag ) ||
			tipo_de_procura == COMECA  &&  comeca_com( arqv_auxiliar->tag, tag ) ||
			tipo_de_procura == TERMINA &&  termina_com( arqv_auxiliar->tag, tag ) )
		{
			//tipo de formatacao diferente para o arquivo de express�o.
			if( !strcmp( cabeca->nome, NOME_ARQUIVO_EXPR ) )
			{
				printf( "%s\n", arqv_auxiliar->tag );
				printf( "%s\n", arqv_auxiliar->texto );
				printf( "%s\n", arqv_auxiliar->tag );
			}
			else
				printf( "%s\n", arqv_auxiliar->texto );

			retorno = true;
		}
	}

	return retorno;
}

/* Efetua a busca de uma express�o e em seguida efetua a busca das tags de c�lculo nos outros arquivos.
   Tamb�m efetua a valida��o de uma express�o de c�lculo.
par�metros:
	arquivo		= arquivo para ser analisado.
	tag			= tag de express�o a ser encontrada.
retorno:
	true:		se pelo menos uma tag de c�lculo foi encontrada.
	false:		se nenhuma tag de c�lculo foi encontrada.
*/
bool procurar_tags_calculo( Arquivo **arquivo, char *tag, bool validar )
{
	bool e_valido	= false;
	bool parar		= false;

	if( *arquivo == NULL )
		return false;

	Arquivo *expcal = localizar_arquivo( arquivo, NOME_ARQUIVO_EXPR );

	if( expcal == NULL )
		return false;

	//iterando as linhas do arquivo de express�es 'expcal'.
	for( int i = 0; i < expcal->qnt_total_linhas; i++ )
	{
		Arquivo *expcal_aux = acessar_linha_por_indice( expcal, i );

		//verificacao de validade do ponteiro.
		if( !expcal_aux )
			continue;

		//verificando validade da tag.
		if( expcal_aux->tag == NULL )
			continue;

		//filtrando somente a expressao requisitada atrav�s da tag concatenada.
		if( strcmp( tag, expcal_aux->tag ) )
			continue;

		//iterando entre as tags de c�lculo da expressao filtrada.
		for( int j = 0; j < expcal_aux->qnt_tags_calculo; j++ )
		{
			Expr *expr_aux = acessar_expressao_por_indice( expcal_aux->expr, j );

			//verificac�o de validade do ponteiro.
			if( !expr_aux )
				continue;

			if( !validar )
			{
				//iterando entre os 10 arquivos para localizar a(s) tag(s) de c�lculo.
				for( int k = 0; k < QNT_MAX_ARQUIVOS; k++ )
					if( procurar_tag( arquivo[ k ], expr_aux->texto, EXATO ) )
					{
						expr_aux->num_ocorrencias++;
						e_valido = true;
					}
			}
			else
			{
				if( !parar )
					 e_valido = true;

				//verificando validade da express�o atrav�s do n�mero de ocorr�ncias das tags de c�lculo.
				if( !expr_aux->num_ocorrencias )
				{
					printf( "TAG DE CALCULO '%s' NAO ENCONTRADO!\n", expr_aux->texto );
					e_valido = false;
					parar	 = true;
				}
			}
		}
	}

	return e_valido;
}

/* Efetua a leitura de um arquivo texto e realiza as opera��es adequadas de tratamento dos dados obtidos.
par�metros:
	diretorio	= diret�rio onde o arquivo texto est� localizado.
	nome		= nome do arquivo texto.
retorno:
	Se a rotina for executada com sucesso retorna um ponteiro para a lista de dados do arquivo lido,
	caso contr�rio retorna 0x00 (NULL).
*/
Arquivo *ler_arquivo_texto( char *diretorio, char *nome )
{
	char path_buffer[ TAMANHO_MAX_CAMINHO ] = { 0 };
	char linha[ TAMANHO_MAX_LINHA ]			= { 0 };
	int qnt_total_linhas					= 0;

	sprintf( path_buffer, "%s//%s", diretorio, nome );
	FILE *arquivo = fopen( path_buffer, "rt" );

	if( arquivo == NULL )
	{
		printf( "Problemas na abertura do arquivo %s.\n", nome );
		printf( "Verifique se o diretorio e o nome do arquivo estao corretos!\n" );
		return NULL;
	}

	//contando total de linhas para efetuar alocacao de mem�ria posteriormente.
	for( ; !feof( arquivo ); fgets( linha, TAMANHO_MAX_LINHA, arquivo ) )
	{
		//verificando se � um arquivo de expressao (expcal).
		if( !strcmp( nome, NOME_ARQUIVO_EXPR ) )
		{
			//desconsiderando linhas vazias e contando a quantidade de expressoes nas linhas que n�o possu�m '#'.
			if( *linha != '\r' && !strstr( linha, "#" ) )
			{
                //printf( "%i - %i\n", qnt_total_linhas, linha, &linha[0] );
				qnt_total_linhas++;
            }
		}
		else
		{
			//contando quantidade de linhas (desconsiderando linhas vazias).
			if( *linha != '\r' )
				qnt_total_linhas++;
		}
	}

	//reposicionando o ponteiro para o in�cio do arquivo.
	rewind( arquivo );

	//alocando mem�ria para todas as linhas e zerando os valores alocados.
	Arquivo *linhas = calloc( qnt_total_linhas, sizeof( Arquivo ) );

	//verificando alocacao de mem�ria.
	if( linhas == NULL )
		return NULL;

	for( int i = 0, n_expr = 0; !feof( arquivo ); i++ )
	{
		//obtendo linha do arquivo texto.
		char *result = fgets( linha, TAMANHO_MAX_LINHA, arquivo );

		if( result != NULL )
		{
			//verificando se � um arquivo de expressao (expcal).
			if( !strcmp( nome, NOME_ARQUIVO_EXPR ) )
			{
				//verificacao de linha vazia.
				if( *linha == '\r' )
				{
					/*
					[n_expr = 0]
					# expressao_1
					(c�lculo)
					# expressao_1
					--------------> '\r' (n_expr++)
					[n_expr = 1]
					# expressao_2
					(c�lculo)
					# expressao_2 */

					n_expr++;
					continue;
				}

				if( n_expr >= qnt_total_linhas - 1 )
				{
					printf( "Quantidade de expressoes superior a quantidade total de linhas!\n" );
					printf( "Verifique se o arquivo %s possui mais de uma quebra de linha entre as expressoes.\n", nome );
					return NULL;
				}

				Arquivo *arqv_auxiliar = acessar_linha_por_indice( linhas, n_expr );

				if( arqv_auxiliar == NULL )
					return NULL;

				strcpy( arqv_auxiliar->nome, nome );
				strcpy( arqv_auxiliar->diretorio, diretorio );
				arqv_auxiliar->qnt_total_linhas = qnt_total_linhas - 1;

				//se a linha cont�m '#' ela � uma tag de expressao.
				if( strstr( linha, "#" ) )
				{
					//alocando mem�ria para a tag.
					if( !arqv_auxiliar->tag )
						arqv_auxiliar->tag = calloc( strlen( linha ) + 1, sizeof( char ) );

					//copiando tag para o objeto desconsiderando a quebra de linha ('\r') no final da linha.
					memcpy( arqv_auxiliar->tag, linha, strlen( linha ) - 2 );
				}
				else
				{
					//alocando mem�ria para o texto.
					if( !arqv_auxiliar->texto )
						arqv_auxiliar->texto = calloc( strlen( linha ) + 1, sizeof( char ) );

					//copiando express�o para o objeto desconsiderando a quebra de linha ('\r') no final da linha.
					memcpy( arqv_auxiliar->texto, linha, strlen( linha ) - 2 );

					//contando quantidade de tags de c�lculo na linha.
					for( size_t x = 0; x < strlen( linha ); x++ )
						if( linha[ x ] == '@' )
							arqv_auxiliar->qnt_tags_calculo++;

					//alocando mem�ria para pilha de tags de c�lculo e zerando os valores alocados.
					arqv_auxiliar->expr = calloc( arqv_auxiliar->qnt_tags_calculo, sizeof( Expr ) );

					if( arqv_auxiliar->expr == NULL )
						return NULL;

					for( size_t y = 0, tag_expr = 0; y < strlen( linha ); y++ )
					{
						Expr *expr_auxiliar = acessar_expressao_por_indice( arqv_auxiliar->expr, tag_expr );

						if( expr_auxiliar == NULL )
							return NULL;

						if( linha[ y ] != '@' )
							continue;

						for( int z = 0; isalnum( linha[ y + 1 + z ] ) || linha[ y + 1 + z ] == '_'; z++ )
							expr_auxiliar->texto[ z ] = linha[ y + 1 + z ];

						tag_expr++;
					}
				}
			}
			else
			{
				Arquivo *arqv_auxiliar = acessar_linha_por_indice( linhas, i );

				if( arqv_auxiliar == NULL )
					return NULL;

				//ignorando linhas vazias.
				if( *linha != '\r' )
				{
					//alocando mem�ria para a tag.
					if( !arqv_auxiliar->tag )
						 arqv_auxiliar->tag = calloc( strlen( linha ) + 1, sizeof( char ) );

					//alocando mem�ria para o texto.
					if( !arqv_auxiliar->texto )
						 arqv_auxiliar->texto = calloc( strlen( linha ) + 1, sizeof( char ) );

					//copiando dados do arquivo para a estrutura.
					strcpy( arqv_auxiliar->texto, linha );
					strcpy( arqv_auxiliar->nome, nome );
					strcpy( arqv_auxiliar->diretorio, diretorio );

					arqv_auxiliar->tamanho_linha = strlen( linha );
					arqv_auxiliar->qnt_total_linhas = qnt_total_linhas;

					//copiando a tag separada pelo token ';' para estrutura.
					extrair_string( arqv_auxiliar->texto, ";", arqv_auxiliar->tag );
				}
			}
		}
	}

	//fechando arquivo.
	fclose( arquivo );

	return linhas;
}

/* Localiza determinado(s) arquivo(s) atrav�s de seu nome.
par�metros:
	entrada = array contendo a lista de arquivos.
	nome	= nome do arquivo a ser procurado.
retorno:
	Se a rotina for executada com sucesso retorna um ponteiro para a lista de dados do arquivo encontrado,
	caso contr�rio retorna 0x00 (NULL).
*/
Arquivo *localizar_arquivo( Arquivo **entrada, char *nome )
{
	for( int i = 0; i < QNT_MAX_ARQUIVOS; i++ )
		if( entrada[ i ] != NULL )
			if( !strcmp( entrada[ i ]->nome, nome ) )
				return entrada[ i ];
	return NULL;
}

/* Libera a mem�ria alocada durante a leitura do(s) arquivo(s).
par�metros:
	cabeca = ponteiro para o espa�o de mem�ria do arquivo.
*/
void destruir_arquivo( Arquivo *cabeca )
{
	if( cabeca == NULL ) 
		return;

	//iterando entre as linhas do arquivo de expressao.
	for( int i = 0; i < cabeca->qnt_total_linhas; i++ )
	{
		Arquivo *arqv_auxiliar = acessar_linha_por_indice( cabeca, i );

		//verificando se o ponteiro � valido.
		if( arqv_auxiliar == NULL )
			continue;

		//limpando mem�ria alocada anteriormente.
		if( arqv_auxiliar->tag != NULL );
			free( arqv_auxiliar->tag );

		if( arqv_auxiliar->texto != NULL );
			free( arqv_auxiliar->texto );

		if( arqv_auxiliar->expr != NULL );
			free( arqv_auxiliar->expr );
	}

	free( cabeca );
}

/* Verifica o sufixo de uma string.
par�metros:
	s		= string de entrada a ser comparada.
	t		= string com o valor a ser comparado.
retorno:
	true	= 's' termina com 't'.
	false	= 's' n�o termina com 't'.
*/
bool termina_com( const char *s, const char *t )
{
	size_t slen = strlen( s );
	size_t tlen = strlen( t );
	if( tlen > slen )
		return false;
	return !strcmp( s + slen - tlen, t );
}

/* Verifica o prefixo de uma string.
par�metros:
	s		= string de entrada a ser comparada.
	t		= string com o valor a ser comparado.
retorno:
	true	= 's' come�a com 't'.
	false	= 's' n�o come�a com 't'.
*/
bool comeca_com( const char *s, const char *t )
{
	return strncmp( t, s, strlen( t ) ) == 0;
}

/* Extrai de uma string uma palavara separada por um token delimitante.
par�metros:
	entrada		= string de entrada a ser comparada.
	token		= string com o valor a ser comparado.
	saida		= buffer que ir� conter a palavra resultante.
*/
void extrair_string( char *entrada, char *token, char *saida )
{
	char *str_buffer = ( char * ) calloc( strlen( entrada ) + 1, sizeof( char ) );
	strcpy( str_buffer, entrada );

	if( strtok( str_buffer, token ) != NULL )
		strcpy( saida, strtok( str_buffer, token ) );

	free( str_buffer );
}

/* Compara se em um vetor de string h� algum valor duplicado.
par�metros:
	vet_string			= vetor de strings de entrada a ser comparado.
	tamanho				= quantidade de elementos do vetor.
*/
bool existe_duplicado_str( char **vet_string, int tamanho )
{
	for( int i = 0; i <= tamanho; i++ )
		for( int j = i + 1; j < tamanho; j++ )
			if( !strcmp( vet_string[ i ], vet_string[ j ] ) )
				return true;
	return false;
}
