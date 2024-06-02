
#include "cpu_utils.h"



char* recibir_instruccion(socket_cliente)
{
	t_list recibir_instruccion = recibir_paquete(socket_cliente)
	
	char* instruccion_completa;
	strcpy (instruccion_completa, recibir_instruccion->head->dato);
	char **instruccion_parciada = string_split(instruccion_completa, " ");
	interpretar_instruccion()
}

void ejecutar_instruccion(char **instruccion){
t_instruccion auxliar;
	if(strcmp(instruccion[0], "SET")){
		auxiliar->op_cod=0;
		auxiliar->argumentos->head->data = pasar_registro_a_int(instruccion[1]);
		auxiliar->argumentos->head->netx->data = string_to_int(instruccion[2]);
		
	}
	if(strcmp(instruccion, "SUM"))
		return 1;
	if(strcmp(instruccion, "SUB"))
		return 2;
	if(strcmp(instruccion, "JNZ"))
		return 3;
	if(strcmpp(instruccion, "IO_GEN_SLEEP"))
		return 4;
}

int pasar_registro_a_int(char* registro){
	if(strcmp(registro, "AX"))
		return 0;
	if(strcmp(registro, "BX"))
		return 1;
	if(strcmp(registro, "CX"))
		return 2;
	if(strcmp(registro, "DX"))
		return 3;
	if(strcmp(registro, "EAX"))
		return 4;
	if(strcmp(registro, "EBX")){
		return 5;
	}
	if(strcmp(registro, "ECX")){
		return 6;
	}
	if(strcmp)
}