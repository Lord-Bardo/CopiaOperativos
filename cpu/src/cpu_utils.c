
#include "../include/cpu_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "cpu.c" //lo agrego para poder usar el pcb en ejecutar instruccion

/* t_pcb* deserializar_pcb (t_paquete* paquete) {
	t_pcb* pcb = s_malloc(sizeof(t_pcb));
	uint64_t offset = 0;
	while (package_decode_isset(package, offset)) { //isset indica si hay algun valor valido para extraer de la estructura
		t_paquete* paquete_anidado = decodificar_paquete(paquete->buffer, &offset);
		switch (paquete_anidado->type) {
			/*case EC_INSTRUCTIONS: //ver que hacer con esto
				pcb->instrucciones = queue_create();
				deserialize_instructions(nested_package, pcb->instructions);
				break;
			case PC:
				decodificar_paquete_buffer(paquete_anidado->buffer, &(pcb->PC), NULL); //crear definicion del program_counter
				break;
			/*case LAST_BURST_TIME:
				package_decode_buffer(nested_package->buffer, &(ec->last_burst_time), NULL);
				break;
			case PROCESS_PID:
				decodificar_paquete_buffer(paquete_anidado->buffer, &(pcb->PID), NULL);
				break;
			case registros:
				pcb->registros= deserializar_cpu_registros(paquete_anidado->buffer);
				break;
			/*case SEGMENTS_TABLE:
				ec->segments_table = deserializar_segment_table(nested_package);
				break;
			default:
				printf("Error: Tipo de paquete desconocido.\n");
				return NULL;
		}
		destruir_paquete(paquete_anidado);
	}
	return pcb;
} */

// Función para deserializar una estructura t_pcb desde un paquete
t_pcb* deserializar_pcb(uint8_t* buffer, size_t buffer_size) {
    if (buffer_size < sizeof(int) * 2 + sizeof(estado)) {
        printf("Error: Tamaño de buffer insuficiente para deserializar t_pcb\n");
        return NULL;
    }
    
    t_pcb* pcb = (t_pcb*)malloc(sizeof(t_pcb));
    if (pcb == NULL) {
        printf("Error: No se pudo asignar memoria para la estructura t_pcb\n");
        return NULL;
    }
    
    uint64_t offset = 0;
    
    // Copiar PID
    memcpy(&(pcb->PID), buffer + offset, sizeof(int));
    offset += sizeof(int);
    
    // Copiar quantum
    memcpy(&(pcb->quantum), buffer + offset, sizeof(int));
    offset += sizeof(int);
    
    // Copiar estado
    memcpy(&(pcb->estado), buffer + offset, sizeof(estado));
    offset += sizeof(estado);
    
    // Copiar registros
    memcpy(&(pcb->registros.PC), buffer + offset, sizeof(__uint32_t));
    offset += sizeof(__uint32_t);
    
    pcb->registros.AX = *(buffer + offset++);
    pcb->registros.BX = *(buffer + offset++);
    pcb->registros.CX = *(buffer + offset++);
    pcb->registros.DX = *(buffer + offset++);
    
    memcpy(&(pcb->registros.EAX), buffer + offset, sizeof(__uint32_t));
    offset += sizeof(__uint32_t);
    
    memcpy(&(pcb->registros.EBX), buffer + offset, sizeof(__uint32_t));
    offset += sizeof(__uint32_t);
    
    memcpy(&(pcb->registros.ECX), buffer + offset, sizeof(__uint32_t));
    offset += sizeof(__uint32_t);
    
    memcpy(&(pcb->registros.EDX), buffer + offset, sizeof(__uint32_t));
    offset += sizeof(__uint32_t);
    
    memcpy(&(pcb->registros.SI), buffer + offset, sizeof(__uint32_t));
    offset += sizeof(__uint32_t);
    
    memcpy(&(pcb->registros.DI), buffer + offset, sizeof(__uint32_t));
    offset += sizeof(__uint32_t);
    
    return pcb;
}


char* recibir_instruccion(socket_cliente)
{
	t_list recibir_instruccion = recibir_paquete(socket_cliente)
	
	char* instruccion_completa;
	strcpy (instruccion_completa, recibir_instruccion->head->dato);
	char **instruccion_parciada = string_split(instruccion_completa, " ");
	interpretar_instruccion()
}

void ejecutar_instruccion(char **instruccion){
	t_instruccion instruc_auxliar;
	if(strcmp(instruccion[0], "SET")){
		instruc_auxiliar->op_cod=0;
		instruc_auxiliar->argumentos->head->data = pasar_registro_a_int(instruccion[1]);
		instruc_auxiliar->argumentos->head->netx->data = atoi(instruccion[2]);
		if(son_registro_y_num(instruc_auxiliar->argumentos)) // validamos que los parametros sean correctos y, si lo son, ejecutamos la instruccion.
			ejecutar_SET(instruc_auxiliar); //podemos usar diccionario para reconocer registros
		 else 
			printf("ERROR, parametros incorrecto");
	}
	if(strcmp(instruccion[0], "SUM")){
		instruc_auxiliar->op_cod=1;
		instruc_auxiliar->argumentos->head->data = pasar_registro_a_int(instruccion[1]);
		instruc_auxiliar->argumentos->head->netx->data = pasar_registro_a_int(instruccion[2]);
		if(son_dos_registros(instruc_auxiliar->argumentos)) // validamos que los parametros sean correctos y, si lo son, ejecutamos la instruccion.
			ejecutar_SUM(instruc_auxiliar); //podemos usar diccionario para reconocer registros
		 else 
			printf("ERROR, parametros incorrecto");
	}
	if(strcmp(instruccion[0], "SUB")){
		instruc_auxiliar->op_cod=2;
		instruc_auxiliar->argumentos->head->data = pasar_registro_a_int(instruccion[1]);
		instruc_auxiliar->argumentos->head->netx->data = pasar_registro_a_int(instruccion[2]);
		if(son_dos_registros(instruc_auxiliar->argumentos)) // validamos que los parametros sean correctos y, si lo son, ejecutamos la instruccion.
			ejecutar_SUB(instruc_auxiliar); //podemos usar diccionario para reconocer registros
		 else 
			printf("ERROR, parametros incorrecto");
	}
	if(strcmp(instruccion[0], "JNZ")){
		instruc_auxiliar->op_cod=3;
		instruc_auxiliar->argumentos->head->data = pasar_registro_a_int(instruccion[1]);
		instruc_auxiliar->argumentos->head->netx->data = atoi(instruccion[2]);
		if(son_registro_y_num(instruc_auxiliar->argumentos)) // validamos que los parametros sean correctos y, si lo son, ejecutamos la instruccion.
			ejecutar_JNZ(instruc_auxiliar); //podemos usar diccionario para reconocer registros
		else 
			printf("ERROR, parametros incorrecto");
	}
	if(strcmp(instruccion, "IO_GEN_SLEEP")){
		// TODO 
	}
	if(strcmp(instruccion, "IO_GEN_READ")){
		// TODO 
	}
	if(strcmp(instruccion, "MOV_IN")){
		instruc_auxiliar->op_cod=6;
		instruc_auxiliar->argumentos->head->data = pasar_registro_a_int(instruccion[1]);
		instruc_auxiliar->argumentos->head->netx->data = pasar_registro_a_int(instruccion[2]);
		if(son_dos_registros(instruc_auxiliar->argumentos)) // validamos que los parametros sean correctos y, si lo son, ejecutamos la instruccion.
			ejecutar_MOV_IN(instruc_auxiliar); //podemos usar diccionario para reconocer registros
		 else 
			printf("ERROR, parametros incorrecto"); 
	}
	if(strcmp(instruccion, "MOV_OUT")){
		// TODO 
	}
	if(strcmp(instruccion, "RESIZE")){
		// TODO 
	}
	if(strcmp(instruccion, "COPY_STRING")){
		// TODO 
	}
	if(strcmp(instruccion, "WAIT")){
		// TODO 
	}
	if(strcmp(instruccion, "SIGNAL")){
		// TODO 
	}
	if(strcmp(instruccion, "IO_STDOUT_WRITE")){
		// TODO 
	}
	if(strcmp(instruccion, "IO_FS_CREATE")){
		// TODO 
	}
	if(strcmp(instruccion, "IO_FS_DELETE")){
		// TODO 
	}
	if(strcmp(instruccion, "IO_FS_TRUNCATE")){
		// TODO 
	}
	if(strcmp(instruccion, "IO_FS_WRITE")){
		// TODO 
	}
	if(strcmp(instruccion, "IO_FS_READ")){
		// TODO 
	}
	if(strcmp(instruccion, "EXIT")){
		// TODO 
	}
	if(strcmp(instruccion, "IO_STDOUT_WRITE")){
		// TODO 
	}
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
void ejecutar_SUM(t_instruccion* instruccion){ //supongo que si en los argumentos hay un registro habrá un numero que representará al valor del enum que se deba acceder
	switch(*instruccion->argumentos->head->data){
	    case 0: //AX
		 pcb_auxiliar.registros.AX += obtener_registro(*instruccion->argumentos->head->next->data); // retorna el registro que debo sumar (el origen)
		 break;
		case 1: //BX
		 pcb_auxiliar.registros.BX += obtener_registro(*instruccion->argumentos->head->next->data);
		 break;
		case 2: //CX
		 pcb_auxiliar.registros.CX += obtener_registro(*instruccion->argumentos->head->next->data);
		 break;
		case 3: //DX
	 	 pcb_auxiliar.registros.CX += obtener_registro(*instruccion->argumentos->head->next->data);
		 break;
		case 4: //EAX
		 pcb_auxiliar.registros.EAX += obtener_registro(*instruccion->argumentos->head->next->data);
		 break;
		case 5: //EBX
		 pcb_auxiliar.registros.EBX += obtener_registro(*instruccion->argumentos->head->next->data);
		 break;
		case 6: //ECX
		 pcb_auxiliar.registros.ECX += obtener_registro(*instruccion->argumentos->head->next->data);
		 break;
		case 7: //EDX
		 pcb_auxiliar.registros.EDX += obtener_registro(*instruccion->argumentos->head->next->data);
		 break;   
		default: 
		 break;
	}
}

void ejecutar_SET(t_instruccion* instruccion){
	switch(*instruccion->argumentos->head->data){
	    case 0: //AX
		 pcb_auxiliar.registros.AX = *instruccion->argumentos->head->next->data;
		 break;
		case 1: //BX
		 pcb_auxiliar.registros.BX = *instruccion->argumentos->head->next->data;
		 break;
		case 2: //CX
		 pcb_auxiliar.registros.CX = *instruccion->argumentos->head->next->data;
		 break;
		case 3: //DX
	 	 pcb_auxiliar.registros.CX = *instruccion->argumentos->head->next->data;
		 break;
		case 4: //EAX
		 pcb_auxiliar.registros.EAX = *instruccion->argumentos->head->next->data;
		 break;
		case 5: //EBX
		 pcb_auxiliar.registros.EBX = *instruccion->argumentos->head->next->data;
		 break;
		case 6: //ECX
		 pcb_auxiliar.registros.ECX = *instruccion->argumentos->head->next->data;
		 break;
		case 7: //EDX
		 pcb_auxiliar.registros.EDX = *instruccion->argumentos->head->next->data;
		 break;   
		default: 
		 break;
	}
}

void ejecutar_SUB(t_instruccion* instruccion){ //supongo que si en los argumentos hay un registro habrá un numero que representará al valor del enum que se deba acceder
	switch(*instruccion->argumentos->head->data){
	    case 0: //AX
		 pcb_auxiliar.registros.AX -= obtener_registro(*instruccion->argumentos->head->next->data);
		 break;
		case 1: //BX
		 pcb_auxiliar.registros.BX -= obtener_registro(*instruccion->argumentos->head->next->data);
		 break;
		case 2: //CX
		 pcb_auxiliar.registros.CX -= obtener_registro(*instruccion->argumentos->head->next->data);
		 break;
		case 3: //DX
		 pcb_auxiliar.registros.CX -= obtener_registro(*instruccion->argumentos->head->next->data);
		 break;
		case 4: //EAX
		 pcb_auxiliar.registros.EAX -= obtener_registro(*instruccion->argumentos->head->next->data);
		 break;
		case 5: //EBX
		 pcb_auxiliar.registros.EBX -= obtener_registro(*instruccion->argumentos->head->next->data);
		 break;
		case 6: //ECX
		 pcb_auxiliar.registros.ECX -= obtener_registro(*instruccion->argumentos->head->next->data);
		 break;
		case 7: //EDX
		 pcb_auxiliar.registros.EDX -= obtener_registro(*instruccion->argumentos->head->next->data);
		 break;   
		default: 
		 break;
	}
}

void ejecutar_JNZ(t_instruccion* instruccion){ //supongo que si en los argumentos hay un registro habrá un numero que representará al valor del enum que se deba acceder
	switch(*instruccion->argumentos->head->data){
	    case 0: //AX
		 if(pcb_auxiliar.registros.AX!=0){
			pcb_auxiliar.registros.PC = *instruccion->argumentos->head->next->data;
		 }
		 break;
		case 1: //BX
		 if(pcb_auxiliar.registros.BX!=0){
			pcb_auxiliar.registros.PC = *instruccion->argumentos->head->next->data;
		 }
		 break;
		case 2: //CX
		 if(pcb_auxiliar.registros.CX!=0){
			pcb_auxiliar.registros.PC = *instruccion->argumentos->head->next->data;
		 }
		 break;
		case 3: //DX
		 if(pcb_auxiliar.registros.DX!=0){
			pcb_auxiliar.registros.PC = *instruccion->argumentos->head->next->data;
		 }
		 break;
		case 4: //EAX
		 if(pcb_auxiliar.registros.EAX!=0){
			pcb_auxiliar.registros.PC = *instruccion->argumentos->head->next->data;
		 }
		 break;
		case 5: //EBX
		 if(pcb_auxiliar.registros.EBX!=0){
			pcb_auxiliar.registros.PC = *instruccion->argumentos->head->next->data;
		 }
		case 6: //ECX
		 if(pcb_auxiliar.registros.ECX!=0){
			pcb_auxiliar.registros.PC = *instruccion->argumentos->head->next->data;
		 }
		 break;
		case 7: //EDX
		 if(pcb_auxiliar.registros.EDX!=0){
			pcb_auxiliar.registros.PC = *instruccion->argumentos->head->next->data;
		 }
		 break;   
		default: 
		 break;
	}
}
void ejecutar_MOV_IN(t_instruccion* instruccion){
	t_paquete *paquete_direccion_instruccion = crear_paquete(FETCH); //creamos el paquete para mandarle la dire de instruccion que queremos a memoria
	agregar_a_paquete(paquete_direccion_instruccion, instruccion->argumentos->head->next->data, sizeof(instruccion->argumentos->head->next->data));
	serializar_paquete_instruccion(paquete_direccion_instruccion, (sizeof(int) + (paquete_direccion_instruccion->buffer->size)));
	enviar_paquete(paquete_direccion_instruccion, fd_memoria); // TODO ESTO DEBE SER CORREGIDO CON LOS DATOS DE LA INSTRUCCION
}

bool son_dos_registros(t_list *argumentos){
	if(argumentos->elements_count == 2){
		if(between(*argumentos->head->data, 0, 7) && isdigit(*argumentos->head->next->data))
			return true;
	}
	else
		return false;
}
bool son_registro_y_num(t_list *argumentos){
    if(argumentos->elements_count == 2){
		if(between(*argumentos->head->data, 0, 7) && between(*argumentos->head->next->data, 0, 7))
			return true;
	}
	else
		return false;

}
bool between(float valor, float min, float max){
	if(valor>=min && valor<=max)
		return true;
}

int obtener_valor_registro(int reg){
	switch(reg){
	    case 0: //AX
		 return (int)pcb->registros.AX;
		 break;
		case 1: //BX
		 return (int)pcb->registros.BX;
		 break;
		case 2: //CX
		 return (int)pcb->registros.CX;
		 break;
		case 3: //DX
		 return (int)pcb->registros.DX;
		 break;
		case 4: //EAX
		 return (int)pcb->registros.EAX;
		 break;
		case 5: //EBX
		 return (int)pcb->registros.EBX;
		 break;
		case 6: //ECX
		 return (int)pcb->registros.ECX;
		 break;
		case 7: //EDX
		 return (int)pcb->registros.EDX;
		 break;   
		default: 
		 break;
	}
}


/* 
arrancar.
paquete auxiliar
crear paquete aux: que esté serializado, luego deserializarlo, despu ya está maomeno
necesiro ese paquete auxiliar, necesito si ese auxiliar porque cuando devuelvo AX tiene que dar 20 jaja en los papeles no? 
porque ahí serializa todo y eso lo comentó. 
Tengo que comentar enviar paquete... recibir paquete hay que moquearlo */

