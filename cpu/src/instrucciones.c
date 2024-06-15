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
	int *registro = obtener_registro(registro_string);
	
	if (registro != NULL) {
    	int valor = atoi(valor_string);
		*registro = valor;
	} 
	else {
    	printf("Error: Registro no válido\n");
    // Manejar el error de alguna manera apropiada
	}
	
}
void ejecutarSum(char * registro_destino, char * registro_origen){
	int *registroD = obtener_registro(registro_destino); //donde vamos a guardar el resultado

	if (registroD != NULL) {
    	int valor = *obtener_registro(registro_origen);
		*registroD += valor;
	} 
	else {
    printf("Error: Registro Destino no válido\n");
    	// Manejar el error de alguna manera apropiada
	}
	
}
void ejecutarSub(char * registro_destino, char * registro_origen){
	int *registroD = obtener_registro(registro_destino); //donde vamos a guardar el resultado

	if (registroD != NULL) {
    	int valor = *obtener_registro(registro_origen);
		*registroD -= valor;
	} 
	else {
    printf("Error: Registro Destino no válido\n");
    	// Manejar el error de alguna manera apropiada
	}
	
}

void ejecutarJnz(char * registro_string, char * nro_instruccion_string){

	int valor_registro = *obtener_registro(registro_string);
	int nro_instruccion =atoi(nro_instruccion_string);
	if(valor_registro==0){
		pcb.registros.pc = nro_instruccion;
	}

}



t_instruccion* pedidoAMemoria(int pid, int pc) {
    // Simulación de obtención de instrucción de memoria
    char* args[5] = {"DX", "1200", NULL, "arg4", "arg5"};
    t_instruccion* instr = (t_instruccion*) malloc(sizeof(t_instruccion));
    if (instr == NULL) {
        printf("Error al asignar memoria para la instrucción\n");
        return NULL;
    }
     printf("Op Code PEDIDOmemoria: %d\n", SUM);
    inicializarInstruccion(instr, SET, args);
    return instr;
}