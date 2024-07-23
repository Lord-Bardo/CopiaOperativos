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
		enviar_pcb_kernel(motivo_desalojo); // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
	if(es_reg_chico(registro_string)){
		set_reg_chico(registro_string, (uint8_t)atoi(valor_string));
	}
	else{
		set_reg_grande(registro_string,atoi(valor_string));
	}
}



void ejecutarSum(char * registro_destino, char * registro_origen){ //x lo que vi en las pruebas siempre suman mismo tipo registros

	if(es_reg_chico(registro_destino)){
		uint8_t regd = get_reg_chico(registro_destino);
		if(es_reg_chico(registro_origen)){
			uint8_t rego = get_reg_chico(registro_origen);
			set_reg_chico(registro_destino, regd + rego);
		}
		else{
			uint32_t rego = get_reg_grande(registro_origen);
			set_reg_chico(registro_destino,(uint8_t)(regd+rego));
		}
	}
	else{
		uint32_t regd = get_reg_grande(registro_destino);
		if(es_reg_chico(registro_origen)){
			uint8_t rego =get_reg_chico(registro_origen);
			set_reg_grande(registro_destino, regd + rego);
		}
		else{
			uint32_t rego = get_reg_grande(registro_origen);
			set_reg_grande(registro_destino, regd + rego);
		}	
	}
}
void ejecutarSub(char * registro_destino, char * registro_origen){ //x lo que vi en las pruebas siempre suman mismo tipo registros

	if(es_reg_chico(registro_destino)){
		uint8_t regd = get_reg_chico(registro_destino);
		if(es_reg_chico(registro_origen)){
			uint8_t rego = get_reg_chico(registro_origen);
			set_reg_chico(registro_destino, regd - rego);
		}
		else{
			uint32_t rego = get_reg_grande(registro_origen);
			set_reg_chico(registro_destino,(uint8_t)(regd - rego));
		}
	}
	else{
		uint32_t regd = get_reg_grande(registro_destino);
		if(es_reg_chico(registro_origen)){
			uint8_t rego =get_reg_chico(registro_origen);
			set_reg_grande(registro_destino, regd - rego);
		}
		else{
			uint32_t rego = get_reg_grande(registro_origen);
			set_reg_grande(registro_destino, regd - rego);
		}	
	}
}

void ejecutarJnz(char * registro_string, char * nro_instruccion_string){
	u_int32_t nro_instruccion = (u_int32_t)atoi(nro_instruccion_string);
	
	if(es_reg_chico(registro_string)){
		uint8_t valor = get_reg_chico(registro_string);
		if(valor!=0){
			set_pc(nro_instruccion);
		}
	}
	else{
		uint32_t valor = get_reg_grande(registro_string);
		if(valor!=0){
			set_pc(nro_instruccion);
		}
	}
}

void ejecutarWait(char * recurso){
	t_paquete * paquete = crear_paquete(COP_WAIT);
	pcb.pc ++;
	agregar_contexto_ejecucion_a_paquete(paquete, &pcb);
	agregar_string_a_paquete(paquete, recurso);
	enviar_paquete(fd_kernel_dispatch,paquete);
	eliminar_paquete(paquete);
	
	salir_ciclo_instruccion = 1;
	motivo_desalojo = COP_WAIT;
}

void ejecutarSignal(char * recurso){
	t_paquete * paquete = crear_paquete(COP_SIGNAL);
	pcb.pc ++;
	agregar_contexto_ejecucion_a_paquete(paquete, &pcb);
	agregar_string_a_paquete(paquete, recurso);
	enviar_paquete(fd_kernel_dispatch,paquete);
	eliminar_paquete(paquete);

	salir_ciclo_instruccion = 1;
	motivo_desalojo = COP_SIGNAL;
}

void ejecutarIoGenSleep(char * interfaz, char * tiempo_string){ //pasar a int el tiempo
	t_codigo_operacion op= COP_IO_GEN_SLEEP;
	t_paquete *paquete =crear_paquete(IO);
	pcb.pc ++;
	agregar_contexto_ejecucion_a_paquete(paquete, &pcb);
	int tiempo_int = atoi(tiempo_string);
	agregar_string_a_paquete(paquete,interfaz);

	agregar_a_paquete(paquete,&op,sizeof(t_codigo_operacion));

	agregar_int_a_paquete(paquete,tiempo_int);
	
	
	enviar_paquete(fd_kernel_dispatch,paquete);
	eliminar_paquete(paquete);
	
	salir_ciclo_instruccion =1;
	motivo_desalojo = IO;
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

		t_paquete *paquete = crear_paquete(OUT_OF_MEMORY);
		agregar_contexto_ejecucion_a_paquete(paquete, &pcb);
		
		enviar_paquete(fd_kernel_dispatch,paquete);
		eliminar_paquete(paquete);
	}
	log_info(cpu_logger,"Confirmacion resize %d",respuesta);
	//aca deberia esperar recibir el out of memory
	//En caso de que la respuesta de la memoria sea Out of Memory, se deberá devolver el contexto de ejecución al Kernel informando de esta situación.
}

void ejecutarMovIn(char* registro_datos, char* registro_direrccion){
	/*
	(Registro Datos, Registro Dirección): Lee el valor de memoria correspondiente a la Dirección Lógica que 
	se encuentra en el Registro Dirección y lo almacena en el Registro Datos.*/
	printf("ENTRÉ MOV IN");
	if(es_reg_chico(registro_direrccion)){
		uint8_t dl = get_reg_chico(registro_direrccion);
		if(es_reg_chico(registro_datos)){
			uint8_t valor;
			mmu_leer(dl,1,&valor);
			set_reg_chico(registro_datos,valor);
		}
		else{
			uint32_t valor;
			mmu_leer(dl,4,&valor);
			set_reg_chico(registro_datos,valor);
		}
	}
	else{
		uint32_t dl = get_reg_grande(registro_direrccion);
		if(es_reg_chico(registro_datos)){
			uint8_t valor;
			mmu_leer(dl,1,&valor);
			set_reg_grande(registro_datos,valor);
		}
		else{
			uint32_t valor;
			mmu_leer(dl,4,&valor);
			set_reg_grande(registro_datos,valor);
		}
	}
	
}
void ejecutarMovOut(char* registro_direrccion,char* registro_datos){
	/*(Registro Dirección, Registro Datos): Lee el valor del Registro Datos y lo escribe en la dirección física de memoria obtenida
	 a partir de la Dirección Lógica almacenada en el Registro Dirección.
	*/
	printf("ENTRE MOV OUT");
	if(es_reg_chico(registro_datos)){
		uint8_t regd = get_reg_chico(registro_datos);
		if(es_reg_chico(registro_direrccion)){
			uint8_t dl = get_reg_chico(registro_direrccion);
			mmu_escribir(dl,1,&regd);
		}
		else{
			uint32_t dl = get_reg_grande(registro_direrccion);
			mmu_escribir(dl,1,&regd);
		}
	}
	else{
		uint32_t regd = get_reg_grande(registro_datos);
		if(es_reg_chico(registro_direrccion)){
			uint8_t dl = get_reg_chico(registro_direrccion);
			mmu_escribir(dl,4,&regd);
		}
		else{
			uint32_t dl = get_reg_grande(registro_direrccion);
			mmu_escribir(dl,4,&regd);
		}
		
	}
	
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
	
	if(es_reg_chico(registro_tamanio)){

		uint8_t valor_registro = get_reg_chico(registro_tamanio);
		t_paquete * paquete = crear_paquete(COP_IO_FS_TRUNCATE);
		agregar_a_paquete(paquete, interfaz, sizeof(interfaz)+1);
		agregar_a_paquete(paquete, archivo, sizeof(archivo)+1);
		agregar_a_paquete(paquete,valor_registro, sizeof(u_int32_t));
		enviar_paquete(fd_kernel_dispatch,paquete);
		eliminar_paquete(paquete);

	}

	else{

		uint32_t valor_registro = get_reg_grande(registro_tamanio);
		t_paquete * paquete = crear_paquete(COP_IO_FS_TRUNCATE);
		agregar_a_paquete(paquete, interfaz, sizeof(interfaz)+1);
		agregar_a_paquete(paquete, archivo, sizeof(archivo)+1);
		agregar_a_paquete(paquete,valor_registro, sizeof(u_int32_t));
		enviar_paquete(fd_kernel_dispatch,paquete);
		eliminar_paquete(paquete);
		
	}	
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


