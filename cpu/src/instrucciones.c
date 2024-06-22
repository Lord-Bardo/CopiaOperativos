#include "../include/instrucciones.h"

void inicializarInstruccion(t_instruccion* instr, t_instr_code code, char* args[5]) {
    if (instr == NULL) {
        printf("Error: puntero a instrucción es NULL\n");
        return;
    }

    // Inicializar op_code
    instr->instr_code = code;
    printf("Op Code dentro de inicializar instruccion: %d\n", instr->instr_code);
    // Asignar memoria y copiar los argumentos
    for (int i = 0; i < 5; i++) {
        if (args[i] != NULL) {
            instr->argumentos[i] = (char*) malloc(strlen(args[i]) + 1);
            if (instr->argumentos[i] == NULL) {
                printf("Error al asignar memoria para el argumento %d\n", i);
                // Liberar memoria previamente asignada antes de retornar
                for (int j = 0; j < i; j++) {
                    free(instr->argumentos[j]);
                }
                return;
            }
            strcpy(instr->argumentos[i], args[i]);
        } else {
            instr->argumentos[i] = NULL;
        }
    }
}

void liberarInstruccion(t_instruccion* instr) {
    if (instr != NULL) {
        // Liberar memoria de los argumentos
        for (int i = 0; i < 5; i++) {
            if (instr->argumentos[i] != NULL) {
                free(instr->argumentos[i]);
            }
        }
    }
}


void mostrarInstruccion(t_instruccion instr) {
    printf("Op Code mostrar INstruccion: %d\n", instr.instr_code);
    for (int i = 0; i < 5; i++) {
        if (instr.argumentos[i] != NULL) {
            printf("Arg %d: %s\n", i, instr.argumentos[i]);
        }
    } 

}
t_instruccion* fetch(int pid, int pc) {
    t_instruccion* instr = pedidoAMemoria(pid, pc);
    return instr;
}

void execute(t_instruccion *instruccion){
	switch (instruccion->instr_code)
	{
	case EXIT:
		salir =1;
		break;
	case SET:
		ejecutarSet(instruccion->argumentos[0],instruccion->argumentos[1]);
		break;
	case SUM:
		ejecutarSum(instruccion->argumentos[0],instruccion->argumentos[1]);
		break;
	case SUB:
		ejecutarSub(instruccion->argumentos[0],instruccion->argumentos[1]);
		break;
	case JNZ:
		ejecutarJnz(instruccion->argumentos[0],instruccion->argumentos[1]);
		break;
	case IO_GEN_SLEEP:
		ejecutarSum(instruccion->argumentos[0],instruccion->argumentos[1]);
		break;
	case MOV_IN:
		ejecutarSum(instruccion->argumentos[0],instruccion->argumentos[1]);
		break;
	case MOV_OUT:
		ejecutarSum(instruccion->argumentos[0],instruccion->argumentos[1]);
		break;
	case RESIZE:
		ejecutarSum(instruccion->argumentos[0],instruccion->argumentos[1]);
		break;
	case COPY_STRING:
		ejecutarSub(instruccion->argumentos[0],instruccion->argumentos[1]);
		break;
	case IO_STDIN_READ:
		ejecutarSub(instruccion->argumentos[0],instruccion->argumentos[1]);
		break;
	case IO_STDOUT_WRITE:
		ejecutarSub(instruccion->argumentos[0],instruccion->argumentos[1]);
		break;		
	default:
		break;
	}
}

void ejecutarSet(char * registro_string, char * valor_string){
	u_int32_t *registro = obtener_registro(registro_string);
	
	if (registro != NULL) {
    	u_int32_t valor = (u_int32_t)atoi(valor_string);
		*registro = valor;
	} 
	else {
    	printf("Error: Registro no válido\n");
    // Manejar el error de alguna manera apropiada
	}
	
}



void ejecutarSum(char * registro_destino, char * registro_origen){ //x lo que vi en las pruebas siempre suman mismo tipo registros

	if((strcmp(registro_destino,"AX")==0 )||(strcmp("BX",registro_destino)== 0)||(strcmp("CX",registro_destino)== 0)||(strcmp("DX",registro_destino)== 0)){
		u_int8_t *registroD = obtener_registro(registro_destino);
		u_int8_t *registroO = obtener_registro(registro_origen);
		*registroD += * registroO;
	}
	else{
		u_int32_t *registroD = obtener_registro(registro_destino);
		u_int32_t *registroO = obtener_registro(registro_origen);
		*registroD += * registroO;
	}
}
void ejecutarSub(char * registro_destino, char * registro_origen){ //x lo que vi en las pruebas siempre suman mismo tipo registros

	if((strcmp(registro_destino,"AX")==0 )||(strcmp("BX",registro_destino)== 0)||(strcmp("CX",registro_destino)== 0)||(strcmp("DX",registro_destino)== 0)){
		u_int8_t *registroD = obtener_registro(registro_destino);
		u_int8_t *registroO = obtener_registro(registro_origen);
		*registroD -= * registroO;
	}
	else{
		u_int32_t *registroD = obtener_registro(registro_destino);
		u_int32_t *registroO = obtener_registro(registro_origen);
		*registroD -= * registroO;
	}
}

void ejecutarJnz(char * registro_string, char * nro_instruccion_string){

	u_int32_t *valor_registro = obtener_registro(registro_string);
	u_int32_t nro_instruccion = (u_int32_t)atoi(nro_instruccion_string);
	if(*valor_registro!=0){
		pcb.pc = nro_instruccion;
	}

}

void ejecutarIoGenSleep(char * interfaz, char * tiempo_string){
	int tiempo = atoi(tiempo_string);
	//armar paquete para kernel

}

t_instruccion* pedidoAMemoria(int pid, int pc) {
    // Simulación de obtención de instrucción de memoria
    char* args[5] = {"EDX", "1200", NULL, "arg4", "arg5"};
    t_instruccion* instr = (t_instruccion*) malloc(sizeof(t_instruccion));
    if (instr == NULL) {
        printf("Error al asignar memoria para la instrucción\n");
        return NULL;
    }
     printf("Op Code PEDIDOmemoria: %d\n", SET);
    inicializarInstruccion(instr, SET, args);
    return instr;
}