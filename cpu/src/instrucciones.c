#include "../include/instrucciones.h"

void inicializarInstruccion(t_instruccion* instr, t_instr_code code, char* args[5]) {
    
}

void liberar_instruccion(t_instruccion* instruccion) {
    
	if(instruccion ==NULL){
		printf("La instruccion es nula, error");
		return;
	}
	if(instruccion->argumentos ==NULL){
		printf("La lista es nula, error");
		return;
	}
	list_destroy_and_destroy_elements(instruccion->argumentos,free);
	free(instruccion);
}

t_instruccion* crear_instruccion() {
    // Reserva memoria para la estructura t_instruccion
    t_instruccion *nueva_instruccion = malloc(sizeof(t_instruccion));
    if (nueva_instruccion == NULL) {
        return NULL; // Manejo de error si no se puede asignar memoria
    }

    // Inicializa los campos de la estructura
    nueva_instruccion->instr_code = NOCODE;
    nueva_instruccion->argumentos = list_create();

    return nueva_instruccion;
}
void mostrarInstruccion(t_instruccion instr) {
    printf("Op Code mostrar INstruccion: %d\n", instr.instr_code);
    list_iterate(instr.argumentos,print_element);

}
void print_element(void *data) {
    printf("%s\n", (char *)data);
}

void execute(t_instruccion *instruccion){
	switch (instruccion->instr_code){
	case EXIT:
		log_info(cpu_logger,"ENTRE AL EXIT");
		salir_ciclo_instruccion=1;
		motivo_desalojo = SUCCESS;
		enviar_pcb_kernel(motivo_desalojo);
		break;
	case SET:
		ejecutarSet(list_get(instruccion->argumentos,0),list_get(instruccion->argumentos,1));
		break;
	case SUM:
		ejecutarSum(list_get(instruccion->argumentos,0),list_get(instruccion->argumentos,1));
		break;
	case SUB:
		ejecutarSub(list_get(instruccion->argumentos,0),list_get(instruccion->argumentos,1));
		break;
	case JNZ:
		ejecutarJnz(list_get(instruccion->argumentos,0),list_get(instruccion->argumentos,1));
		break;
	case MOV_IN:
		ejecutarMovIn(list_get(instruccion->argumentos,0),list_get(instruccion->argumentos,1));
		break;
	case MOV_OUT:
		ejecutarMovOut(list_get(instruccion->argumentos,0),list_get(instruccion->argumentos,1));
		break;
	case RESIZE:
		ejecutarResize(list_get(instruccion->argumentos,0));
		break;
	case COPY_STRING:
		ejecutarCopyString(list_get(instruccion->argumentos,0));
		break;
	case WAIT:
		ejecutarWait(list_get(instruccion->argumentos,0));
		break;
	case SIGNAL:
		ejecutarSignal(list_get(instruccion->argumentos,0));
		break;
	case IO_GEN_SLEEP:
		ejecutarIoGenSleep(list_get(instruccion->argumentos,0),list_get(instruccion->argumentos,1));
		salir_ciclo_instruccion =1;
		motivo_desalojo = IO;
		break;
	case IO_STDIN_READ:
		ejecutarStdRead(list_get(instruccion->argumentos,0),list_get(instruccion->argumentos,1),list_get(instruccion->argumentos,2));
		break;
	case IO_STDOUT_WRITE:
		ejecutarStdWrite(list_get(instruccion->argumentos,0),list_get(instruccion->argumentos,1),list_get(instruccion->argumentos,2));
		break;		
	case IO_FS_CREATE:
		ejecutarIOFsCreate(list_get(instruccion->argumentos,0),list_get(instruccion->argumentos,1));
		break;
	case IO_FS_DELETE:
		ejecutarIOFsDelete(list_get(instruccion->argumentos,0),list_get(instruccion->argumentos,1));
		break;
	case IO_FS_TRUNCATE:
		ejecutarIOFsTruncate(list_get(instruccion->argumentos,0),list_get(instruccion->argumentos,1),list_get(instruccion->argumentos,2));
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
	t_paquete * paquete = crear_paquete(COP_WAIT);
	agregar_a_paquete(paquete, recurso, sizeof(recurso)+1);
	enviar_paquete(fd_kernel_dispatch,paquete);
	eliminar_paquete(paquete);
}

void ejecutarSignal(char * recurso){
	t_paquete * paquete = crear_paquete(COP_SIGNAL);
	agregar_a_paquete(paquete, recurso, sizeof(recurso)+1);
	enviar_paquete(fd_kernel_dispatch,paquete);
	eliminar_paquete(paquete);
}

void ejecutarIoGenSleep(char * interfaz, char * tiempo_string){ //pasar a int el tiempo
	t_codigo_operacion op= COP_IO_GEN_SLEEP;
	t_paquete *paquete =crear_paquete(IO);
	int tiempo_int = atoi(tiempo_string);
	agregar_a_paquete(paquete,interfaz,sizeof(interfaz)+1);

	agregar_a_paquete(paquete,&op,sizeof(t_codigo_operacion));

	agregar_a_paquete(paquete,&tiempo_int,sizeof(int));
	
	
	enviar_paquete(fd_cpu_dispatch,paquete);
	eliminar_paquete(paquete);
	
	//armar paquete para kernel
}

void ejecutarResize(char *tamanio){
	t_paquete * paquete = crear_paquete(COP_RESIZE); //este nombre deberia ser otro para no pisar el de las instrucicones MEM_RESIZE por ejemplo
	int tamanio_int = atoi(tamanio);
	agregar_a_paquete(paquete,&pcb.pid,sizeof(int));
	agregar_a_paquete(paquete,&tamanio_int,sizeof(int));
	enviar_paquete(fd_memoria,paquete);
	log_info(cpu_logger,"Envie paquete peticion resize a memoria");
	eliminar_paquete(paquete);
	t_codigo_operacion respuesta;
	recibir_codigo_operacion(fd_memoria,&respuesta);
	if(respuesta==OUT_OF_MEMORY){
		motivo_desalojo =OUT_OF_MEMORY;
		salir_ciclo_instruccion =1;
	}
	log_info(cpu_logger,"Confirmacion resize %d",respuesta);
	//aca deberia esperar recibir el out of memory
	//En caso de que la respuesta de la memoria sea Out of Memory, se deberá devolver el contexto de ejecución al Kernel informando de esta situación.
}

void ejecutarMovIn(char* registro_datos, char* registro_direrccion){
	//TODO
	printf("ENTRÉ MOV IN");
}
void ejecutarMovOut(char* registro_direrccion,char* registro_datos){
	//TODO
	printf("ENTRE MOV OUT");
}
void ejecutarCopyString(char *tamanio){
	/*Toma del string apuntado por el registro SI y copia la cantidad de bytes
	 indicadas en el parámetro tamaño a la posición de memoria apuntada por el registro DI.*/
    printf("entre copy string");
}

void ejecutarStdRead(char* interfaz, char *registro_direccion, char * registro_tamanio){
	//TODO
	printf("ENTRE STDREAD");
}
void ejecutarStdWrite(char * interfaz, char *registro_direccion, char * registro_tamanio){ //entiendo que voy a leer un registro y ese valor es el tamanio
	//TODO
	printf("ENTRE STDWRITE");
}
void ejecutarIOFsCreate(char * interfaz, char * archivo){
	t_paquete * paquete = crear_paquete(COP_IO_FS_CREATE);
	agregar_a_paquete(paquete, interfaz, sizeof(interfaz)+1);
	agregar_a_paquete(paquete, archivo, sizeof(archivo)+1);
	enviar_paquete(fd_kernel_dispatch,paquete);
	eliminar_paquete(paquete);
}
void ejecutarIOFsDelete(char * interfaz, char * archivo){
	t_paquete * paquete = crear_paquete(COP_IO_FS_DELETE);
	agregar_a_paquete(paquete, interfaz, sizeof(interfaz)+1);
	agregar_a_paquete(paquete, archivo, sizeof(archivo)+1);
	enviar_paquete(fd_kernel_dispatch,paquete);
	eliminar_paquete(paquete);
}
void ejecutarIOFsTruncate(char * interfaz, char * archivo, char * registro_tamanio){
	u_int32_t *valor_registro = obtener_registro(registro_tamanio);
	t_paquete * paquete = crear_paquete(COP_IO_FS_TRUNCATE);
	agregar_a_paquete(paquete, interfaz, sizeof(interfaz)+1);
	agregar_a_paquete(paquete, archivo, sizeof(archivo)+1);
	agregar_a_paquete(paquete,valor_registro, sizeof(u_int32_t));
	enviar_paquete(fd_kernel_dispatch,paquete);
	eliminar_paquete(paquete);
}

// t_instruccion* pedidoAMemoria(int pid, int pc) {
//     // Simulación de obtención de instrucción de memoria
//     char* args[5] = {"EDX", "69", NULL, "arg4", "arg5"};
//     t_instruccion* instr = (t_instruccion*) malloc(sizeof(t_instruccion));
//     if (instr == NULL) {
//         printf("Error al asignar memoria para la instrucción\n");
//         return NULL;
//     }
//      printf("Op Code PEDIDOmemoria: %d\n", SET);
//     inicializarInstruccion(instr, EXIT, args);
//     return instr;
// }



int obtener_numero_instruccion(char * cadena){
	if(strcmp(cadena,"SET")==0){
		return SET;
	}
	if(strcmp(cadena,"MOV_IN")==0){
		return MOV_IN;
	}
	if(strcmp(cadena,"MOV_OUT")==0){
		return MOV_OUT;
	}
	if(strcmp(cadena,"SUM")==0){
		return SUM;
	}
	if(strcmp(cadena,"SUB")==0){
		return SUB;
	}
	if(strcmp(cadena,"JNZ")==0){
		return JNZ;
	}
	if(strcmp(cadena,"RESIZE")==0){
		return RESIZE;
	}
	if(strcmp(cadena,"COPY_STRING")==0){
		return COPY_STRING;
	}
	if(strcmp(cadena,"WAIT")==0){
		return WAIT;
	}
	if(strcmp(cadena,"SIGNAL")==0){
		return SIGNAL;
	}
	if(strcmp(cadena,"IO_GEN_SLEEP")==0){
		return IO_GEN_SLEEP;
	}
	if(strcmp(cadena,"IO_STDIN_READ")==0){
		return IO_STDIN_READ;
	}
	if(strcmp(cadena,"IO_STDOUT_WRITE")==0){
		return IO_STDOUT_WRITE;
	}
	if(strcmp(cadena,"IO_FS_CREATE")==0){
		return IO_FS_CREATE;
	}
	if(strcmp(cadena,"IO_FS_DELETE")==0){
		return IO_FS_DELETE;
	}
	if(strcmp(cadena,"IO_FS_TRUNCATE")==0){
		return IO_FS_TRUNCATE;
	}
	if(strcmp(cadena,"IO_FS_WRITE")==0){
		return IO_FS_WRITE;
	}
	if(strcmp(cadena,"IO_FS_READ")==0){
		return IO_FS_READ;
	}
	if(strcmp(cadena,"EXIT")==0){
		return EXIT;
	}
	return -1;
}


