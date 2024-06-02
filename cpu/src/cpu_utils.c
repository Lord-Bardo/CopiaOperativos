
#include "../include/cpu_utils.h"
#include "cpu_mmu.c"

t_pcb pcb_auxiliar= {
        .PID = 0,
        .quantum = 0,
        .estado = RUNNING,
        .registros = {
            .PC = 0,
            .AX = 0,
            .BX = 0,
            .CX = 0,
            .DX = 0,
            .EAX = 0,
            .EBX = 0,
            .ECX = 0,
            .EDX = 0,
            .SI = 0,
            .DI = 0,
        }
};

/*void recibir_pcb(int socket_cliente){
	t_list *pcb_como_lista = recibir_paquete(socket_cliente);
	t_pcb pcb;
	
}*/
void agregar_instruccion_paquete(t_paquete *paquete, char* instruccion)
{
	agregar_a_paquete(paquete, instruccion, string_length(instruccion));
}

void agregar_pcb_paquete(t_paquete *paquete, t_pcb *pcb) {
	agregar_a_paquete(paquete, &(pcb->PID), sizeof(int));
	agregar_a_paquete(paquete, &(pcb->quantum), sizeof(int));
	agregar_a_paquete(paquete, &(pcb->estado), sizeof(t_estado));
	agregar_a_paquete(paquete, &(pcb->registros.PC), 8);
	agregar_a_paquete(paquete, &(pcb->registros.AX), 2);
	agregar_a_paquete(paquete, &(pcb->registros.BX), 2);
	agregar_a_paquete(paquete, &(pcb->registros.CX), 2);
	agregar_a_paquete(paquete, &(pcb->registros.DX), 2);
	agregar_a_paquete(paquete, &(pcb->registros.EAX), 8);
	agregar_a_paquete(paquete, &(pcb->registros.EBX), 8);
	agregar_a_paquete(paquete, &(pcb->registros.ECX), 8);
	agregar_a_paquete(paquete, &(pcb->registros.EDX), 8);
	agregar_a_paquete(paquete, &(pcb->registros.SI), 8);
	agregar_a_paquete(paquete, &(pcb->registros.DI), 8);
}

char* recibir_instruccion(int socket_cliente) //no se quien hizo esto pero tiene que retornar un char* y no retorna nada
{
	t_list *recibir_instruccion = recibir_paquete(socket_cliente);
	char* instruccion_completa;
	strcpy (instruccion_completa, recibir_instruccion->head->data);
	char **instruccion_parciada = string_split(instruccion_completa, " ");
	ejecutar_instruccion(instruccion_parciada); //no se llama asi me parece
}

/*char* recibir_dato(socket_cliente)  ESTO VAMOS A USARLO PARA LAS FUNCIONES MOVE IN MOVE OUT
{
	t_list* recibir_dato = recibir_paquete(socket_cliente);
	
	return instruccion_completa;
}*/ 

void ejecutar_instruccion(char **instruccion){

	t_instruccion *instruc_auxiliar = malloc(sizeof(t_instruccion));

	if(strcmp(instruccion[0], "SET")){
		instruc_auxiliar->ins_code=0;
		instruc_auxiliar->argumentos->head->data = (void*)(intptr_t) pasar_registro_a_int(instruccion[1]); //conversion complicada de int a void, charlar si hacemos una funcion aux o dejamos así
		instruc_auxiliar->argumentos->head->next->data = (void*)(intptr_t) atoi(instruccion[2]);
		if(son_registro_y_num(instruc_auxiliar->argumentos)) // validamos que los parametros sean correctos y, si lo son, ejecutamos la instruccion.
			ejecutar_SET(instruc_auxiliar); //podemos usar diccionario para reconocer registros
		 else 
			printf("ERROR, parametros incorrecto");
	}
	if(strcmp(instruccion[0], "SUM")){
		instruc_auxiliar->ins_code=1;
		instruc_auxiliar->argumentos->head->data = (void*)(intptr_t) pasar_registro_a_int(instruccion[1]); //
		instruc_auxiliar->argumentos->head->next->data = (void*)(intptr_t) pasar_registro_a_int(instruccion[2]);
		if(son_dos_registros(instruc_auxiliar->argumentos)) // validamos que los parametros sean correctos y, si lo son, ejecutamos la instruccion.
			ejecutar_SUM(instruc_auxiliar); //podemos usar diccionario para reconocer registros
		 else 
			printf("ERROR, parametros incorrecto");
	}
	if(strcmp(instruccion[0], "SUB")){
		instruc_auxiliar->ins_code=2;
		instruc_auxiliar->argumentos->head->data = (void*)(intptr_t) pasar_registro_a_int(instruccion[1]);
		instruc_auxiliar->argumentos->head->next->data = (void*)(intptr_t) pasar_registro_a_int(instruccion[2]);
		if(son_dos_registros(instruc_auxiliar->argumentos)) // validamos que los parametros sean correctos y, si lo son, ejecutamos la instruccion.
			ejecutar_SUB(instruc_auxiliar); //podemos usar diccionario para reconocer registros
		 else 
			printf("ERROR, parametros incorrecto");
	}
	if(strcmp(instruccion[0], "JNZ")){
		instruc_auxiliar->ins_code=3;
		instruc_auxiliar->argumentos->head->data = (void*)(intptr_t) pasar_registro_a_int(instruccion[1]);
		instruc_auxiliar->argumentos->head->next->data = (void*)(intptr_t) atoi(instruccion[2]);
		if(son_registro_y_num(instruc_auxiliar->argumentos)) // validamos que los parametros sean correctos y, si lo son, ejecutamos la instruccion.
			ejecutar_JNZ(instruc_auxiliar); //podemos usar diccionario para reconocer registros
		else 
			printf("ERROR, parametros incorrecto");
	}
	if(strcmp(instruccion[0], "IO_GEN_SLEEP")){
		// TODO 
	}
	if(strcmp(instruccion[0], "IO_GEN_READ")){
		// TODO 
	}
	if(strcmp(instruccion[0], "MOV_IN")){
		instruc_auxiliar->ins_code=6;
		instruc_auxiliar->argumentos->head->data = (void*)(intptr_t) pasar_registro_a_int(instruccion[1]);
		instruc_auxiliar->argumentos->head->next->data = (void*)(intptr_t) pasar_registro_a_int(instruccion[2]);
		if(son_dos_registros(instruc_auxiliar->argumentos)) // validamos que los parametros sean correctos y, si lo son, ejecutamos la instruccion.
			ejecutar_MOV_IN(instruc_auxiliar); //podemos usar diccionario para reconocer registros
		 else 
			printf("ERROR, parametros incorrecto"); 
	}
	if(strcmp(instruccion[0], "MOV_OUT")){
		// TODO 
	}
	if(strcmp(instruccion[0], "RESIZE")){
		// TODO 
	}
	if(strcmp(instruccion[0], "COPY_STRING")){
		// TODO 
	}
	if(strcmp(instruccion[0], "WAIT")){
		// TODO 
	}
	if(strcmp(instruccion[0], "SIGNAL")){
		// TODO 
	}
	if(strcmp(instruccion[0], "IO_STDOUT_WRITE")){
		// TODO 
	}
	if(strcmp(instruccion[0], "IO_FS_CREATE")){
		// TODO 
	}
	if(strcmp(instruccion[0], "IO_FS_DELETE")){
		// TODO 
	}
	if(strcmp(instruccion[0], "IO_FS_TRUNCATE")){
		// TODO 
	}
	if(strcmp(instruccion[0], "IO_FS_WRITE")){
		// TODO 
	}
	if(strcmp(instruccion[0], "IO_FS_READ")){
		// TODO 
	}
	if(strcmp(instruccion[0], "EXIT")){
		// TODO 
	}
	if(strcmp(instruccion[0], "IO_STDOUT_WRITE")){
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
	if(strcmp(registro, "EBX"))
		return 5;
	if(strcmp(registro, "ECX"))
		return 6;
	
}

void ejecutar_SUM(t_instruccion* instruccion){ //supongo que si en los argumentos hay un registro habrá un numero que representará al valor del enum que se deba acceder
	switch(*(int*)instruccion->argumentos->head->data){ //el (*(int*)) cancela el puntero y se queda con int
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
void ejecutar_MOV_OUT(t_instruccion* instruccion){
	// Hay que enviar el dato almacenado en el segundo registro de esta instrucción (parametro 2)
	// a memoria y que se almacene en la dirección física de memoria indicada en el primer
	// registro de esta instrucción (parametro 1) con una dirección lógica. Supongo hay que enviar el 
	// dato y la dirección lógica traducida a física a memoria... NO SÉ CÓMO HACERLO :(
}

void ejecutar_MOV_IN(t_instruccion* instruccion){
	// Hay que enviar la dirección lógica almacenada en el primer registro de esta instrucción (parametro 1)
	// a memoria y, luego, recibir lo almacenado en esa dirección de memoria y guardarlo en el segundo 
	// registro de esta instrucción (parametro2)... NO SÉ CÓMO HACERLO :(
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

