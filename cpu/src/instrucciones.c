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

void liberar_instruccion(t_instruccion* instruccion) {
    if (instruccion == NULL) {
        return; // Si instruccion es NULL, no hay nada que liberar
    }

    // Liberar la memoria de cada argumento en el arreglo de argumentos
    for (int i = 0; i < 5; i++) {
        if (instruccion->argumentos[i] != NULL) {
            free(instruccion->argumentos[i]);
            instruccion->argumentos[i] = NULL; // Importante: marcar como NULL después de liberar
        }
    }

    // Liberar la memoria de la estructura t_instruccion
    free(instruccion);
}
t_instruccion* crear_instruccion() {
    // Reserva memoria para la estructura t_instruccion
    t_instruccion* nueva_instruccion = (t_instruccion*)malloc(sizeof(t_instruccion));
    if (nueva_instruccion == NULL) {
        perror("Error al reservar memoria para t_instruccion");
        exit(EXIT_FAILURE);
    }

    // Reserva memoria para cada argumento en el arreglo de argumentos
    for (int i = 0; i < 5; i++) {
        nueva_instruccion->argumentos[i] = (char*)malloc(15 * sizeof(char));
        if (nueva_instruccion->argumentos[i] == NULL) {
            perror("Error al reservar memoria para un argumento");

            // Libera memoria previamente asignada en caso de error
            for (int j = 0; j < i; j++) {
                free(nueva_instruccion->argumentos[j]);
            }
            free(nueva_instruccion);
            exit(EXIT_FAILURE);
        }
    }

    return nueva_instruccion;
}

void mostrarInstruccion(t_instruccion instr) {
    printf("Op Code mostrar INstruccion: %d\n", instr.instr_code);
    for (int i = 0; i < 5; i++) {
        if (instr.argumentos[i] != NULL) {
            printf("Arg %d: %s\n", i, instr.argumentos[i]);
        }
    } 

}


void execute(t_instruccion *instruccion){
	switch (instruccion->instr_code)
	{
	case EXIT:
		log_info(cpu_logger,"ENTRE AL EXIT");
		salir_ciclo_instruccion=1;
		motivo_desalojo = SUCCESS;
		enviar_pcb_kernel(motivo_desalojo);
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
	case MOV_IN:
		ejecutarMovIn(instruccion->argumentos[0],instruccion->argumentos[1]);
		break;
	case MOV_OUT:
		ejecutarMovOut(instruccion->argumentos[0],instruccion->argumentos[1]);
		break;
	case RESIZE:
		ejecutarResize(instruccion->argumentos[0]);
		break;
	case COPY_STRING:
		ejecutarCopyString(instruccion->argumentos[0]);
		break;
	case WAIT:
		ejecutarWait(instruccion->argumentos[0]);
		break;
	case SIGNAL:
		ejecutarSignal(instruccion->argumentos[0]);
		break;
	case IO_GEN_SLEEP:
		ejecutarIoGenSleep(instruccion->argumentos[0],instruccion->argumentos[1]);
		salir_ciclo_instruccion =1;
		motivo_desalojo = IO;
		break;
	case IO_STDIN_READ:
		ejecutarStdRead(instruccion->argumentos[0],instruccion->argumentos[1],instruccion->argumentos[2]);
		break;
	case IO_STDOUT_WRITE:
		ejecutarStdWrite(instruccion->argumentos[0],instruccion->argumentos[1],instruccion->argumentos[2]);
		break;		
	case IO_FS_CREATE:
		ejecutarIOFsCreate(instruccion->argumentos[0],instruccion->argumentos[1]);
		break;
	case IO_FS_DELETE:
		ejecutarIOFsDelete(instruccion->argumentos[0],instruccion->argumentos[1]);
		break;
	case IO_FS_TRUNCATE:
		ejecutarIOFsTruncate(instruccion->argumentos[0],instruccion->argumentos[1],instruccion->argumentos[2]);
		break;
	case IO_FS_WRITE:
		break;
	case IO_FS_READ:
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
void ejecutarWait(char * recurso){
	t_paquete * paquete = crear_paquete(WAIT);
	agregar_a_paquete(paquete, recurso, sizeof(recurso)+1);
	enviar_paquete(fd_kernel_dispatch,paquete);
	eliminar_paquete(paquete);
}

void ejecutarSignal(char * recurso){
	t_paquete * paquete = crear_paquete(SIGNAL);
	agregar_a_paquete(paquete, recurso, sizeof(recurso)+1);
	enviar_paquete(fd_kernel_dispatch,paquete);
	eliminar_paquete(paquete);
}

void ejecutarIoGenSleep(char * interfaz, char * tiempo_string){ //pasar a int el tiempo
	t_codigo_operacion op= IO_GEN_SLEEP;
	t_paquete *paquete =crear_paquete(IO);

	agregar_a_paquete(paquete,interfaz,sizeof(interfaz)+1);

	agregar_a_paquete(paquete,&op,sizeof(t_codigo_operacion));

	agregar_a_paquete(paquete,tiempo_string,sizeof(tiempo_string)+1);
	
	
	enviar_paquete(fd_cpu_dispatch,paquete);
	eliminar_paquete(paquete);
	
	//armar paquete para kernel
}

void ejecutarResize(char *tamanio){
	t_paquete * paquete = crear_paquete(RESIZE); //este nombre deberia ser otro para no pisar el de las instrucicones MEM_RESIZE por ejemplo
	int tamanio_int = atoi(tamanio);
	
	agregar_a_paquete(paquete,&tamanio_int,sizeof(int));
	enviar_paquete(fd_memoria,paquete);
	eliminar_paquete(paquete);

	//En caso de que la respuesta de la memoria sea Out of Memory, se deberá devolver el contexto de ejecución al Kernel informando de esta situación.
}

void ejecutarMovIn(char* registro_datos, char* registro_direrccion){
	//TODO
}
void ejecutarMovOut(char* registro_direrccion,char* registro_datos){
	//TODO
}
void ejecutarCopyString(char *tamanio){
	/*Toma del string apuntado por el registro SI y copia la cantidad de bytes
	 indicadas en el parámetro tamaño a la posición de memoria apuntada por el registro DI.*/

}

void ejecutarStdRead(char* interfaz, char *registro_direccion, char * registro_tamanio){
	//TODO
}
void ejecutarStdWrite(char * interfaz, char *registro_direccion, char * registro_tamanio){ //entiendo que voy a leer un registro y ese valor es el tamanio
	//TODO
}
void ejecutarIOFsCreate(char * interfaz, char * archivo){
	t_paquete * paquete = crear_paquete(IO_FS_CREATE);
	agregar_a_paquete(paquete, interfaz, sizeof(interfaz)+1);
	agregar_a_paquete(paquete, archivo, sizeof(archivo)+1);
	enviar_paquete(fd_kernel_dispatch,paquete);
	eliminar_paquete(paquete);
}
void ejecutarIOFsDelete(char * interfaz, char * archivo){
	t_paquete * paquete = crear_paquete(IO_FS_DELETE);
	agregar_a_paquete(paquete, interfaz, sizeof(interfaz)+1);
	agregar_a_paquete(paquete, archivo, sizeof(archivo)+1);
	enviar_paquete(fd_kernel_dispatch,paquete);
	eliminar_paquete(paquete);
}
void ejecutarIOFsTruncate(char * interfaz, char * archivo, char * registro_tamanio){
	u_int32_t *valor_registro = obtener_registro(registro_tamanio);
	t_paquete * paquete = crear_paquete(IO_FS_TRUNCATE);
	agregar_a_paquete(paquete, interfaz, sizeof(interfaz)+1);
	agregar_a_paquete(paquete, archivo, sizeof(archivo)+1);
	agregar_a_paquete(paquete,valor_registro, sizeof(u_int32_t));
	enviar_paquete(fd_kernel_dispatch,paquete);
	eliminar_paquete(paquete);
}

t_instruccion* pedidoAMemoria(int pid, int pc) {
    // Simulación de obtención de instrucción de memoria
    char* args[5] = {"EDX", "69", NULL, "arg4", "arg5"};
    t_instruccion* instr = (t_instruccion*) malloc(sizeof(t_instruccion));
    if (instr == NULL) {
        printf("Error al asignar memoria para la instrucción\n");
        return NULL;
    }
     printf("Op Code PEDIDOmemoria: %d\n", SET);
    inicializarInstruccion(instr, EXIT, args);
    return instr;
}



int obtener_numero_instruccion(char * cadena){
	if(strcmp(cadena,"SET")==0){
		return 0;
	}
	if(strcmp(cadena,"SUM")==0){
		return 1;
	}
	if(strcmp(cadena,"SUB")==0){
		return 2;
	}
	if(strcmp(cadena,"JNZ")==0){
		return 3;
	}
	if(strcmp(cadena,"IO_GEN_SLEEP")==0){
		return 4;
	}
	if(strcmp(cadena,"MOV_IN")){
		return 5;
	}
	if(strcmp(cadena,"MOV_OUT")){
		return 6;
	}
	if(strcmp(cadena,"RESIZE")){
		return 7;
	}
	if(strcmp(cadena,"COPY_STRING")){
		return 8;
	}
	if(strcmp(cadena,"INS_IO_STDIN_READ")){
		return 9;
	}
	if(strcmp(cadena,"INS_IO_STDOUT_WRITE")){
		return 10;
	}
	return -1;
}


