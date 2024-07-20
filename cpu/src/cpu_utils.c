
#include "../include/cpu_utils.h"

/*

typedef struct {
	t_instr_code instr_code;
	char* argumentos[5];
}t_instruccion;*/

void buffer_desempaquetar_registros(t_buffer *buffer, t_registros *registros){
    buffer_desempaquetar(buffer, &(registros->ax));
    buffer_desempaquetar(buffer, &(registros->bx));
    buffer_desempaquetar(buffer, &(registros->cx));
    buffer_desempaquetar(buffer, &(registros->dx));
    buffer_desempaquetar(buffer, &(registros->eax));
    buffer_desempaquetar(buffer, &(registros->ebx));
    buffer_desempaquetar(buffer, &(registros->ecx));
    buffer_desempaquetar(buffer, &(registros->edx));
    buffer_desempaquetar(buffer, &(registros->si));
    buffer_desempaquetar(buffer, &(registros->di));
}


void buffer_desempaquetar_pcb(t_buffer *buffer, t_pcb* pcb){ // REVISAR

    buffer_desempaquetar(buffer, &(pcb->pid));
    buffer_desempaquetar(buffer, &(pcb->pc));
    buffer_desempaquetar_registros(buffer, &(pcb->registros));
}

void enviar_pcb_kernel(t_codigo_operacion motivo_desalojo){
    t_paquete *paquete = crear_paquete(motivo_desalojo);
    agregar_contexto_ejecucion_a_paquete(paquete,&pcb);
    enviar_paquete(fd_kernel_dispatch,paquete);
    eliminar_paquete(paquete);
}
void agregar_contexto_ejecucion_a_paquete(t_paquete *paquete, t_pcb* pcb){
    agregar_pid_a_paquete(paquete, pcb_get_pid(pcb));
    agregar_uint32_a_paquete(paquete, pcb->pc);
    agregar_registros_a_paquete(paquete, pcb_get_registros(pcb));
}
void agregar_registros_a_paquete(t_paquete *paquete, t_registros registros){
    agregar_uint8_a_paquete(paquete, registros.ax);
    agregar_uint8_a_paquete(paquete, registros.bx);
    agregar_uint8_a_paquete(paquete, registros.cx);
    agregar_uint8_a_paquete(paquete, registros.dx);
    agregar_uint32_a_paquete(paquete, registros.eax);
    agregar_uint32_a_paquete(paquete, registros.ebx);
    agregar_uint32_a_paquete(paquete, registros.ecx);
    agregar_uint32_a_paquete(paquete, registros.edx);
    agregar_uint32_a_paquete(paquete, registros.si);
    agregar_uint32_a_paquete(paquete, registros.di);
}
void agregar_pid_a_paquete(t_paquete *paquete, int pid){
    agregar_a_paquete(paquete, &pid, sizeof(pid));
}

int pcb_get_pid(t_pcb *pcb){
    return pcb->pid;
}
t_registros pcb_get_registros(t_pcb *pcb){
    return pcb->registros;
}
void recibir_instruccion(t_instruccion * instruccion){
	t_codigo_operacion op_code;
	t_buffer * buffer = crear_buffer();
	
	recibir_paquete(fd_memoria,&op_code,buffer);

	if(op_code!= INSTRUCCION){
		log_info(cpu_logger,"ERROR AL RECIBIR LA INSTRUCCION DE MEMORIA, el codigo de operacion no es INSTRUCCION");
		return;
	}
	
	buffer_desempaquetar_instruccion(buffer,instruccion);
	eliminar_buffer(buffer);

}
t_instruccion convertir_string_a_instruccion(char* cadena) {
    t_instruccion instruccion;
    char *instr_code = strtok(cadena, " ");
    instruccion.instr_code =obtener_numero_instruccion(instr_code);

    int i = 0;
    char *token = strtok(NULL, " ");
    while (token != NULL && i < 5) {
        instruccion.argumentos[i++] = token;
        token = strtok(NULL, " ");
    }
    // Asegurarse de que los argumentos restantes sean NULL
    while (i < 5) {
        instruccion.argumentos[i++] = NULL;
    }
    return instruccion;
}

void buffer_desempaquetar_instruccion(t_buffer *buffer, t_instruccion * instruccion){
	char * instruccion_string =buffer_desempaquetar_string(buffer);
    log_info(cpu_logger,"LA CADENA ENTERA DE LA INSTRUCCION ES: %s",instruccion_string);
   
    *instruccion = convertir_string_a_instruccion(instruccion_string);
    mostrarInstruccion(*instruccion);
}

void enviar_fetch_memoria(int pid, u_int32_t pc){
	t_paquete * paquete = crear_paquete(FETCH);
	agregar_a_paquete(paquete, &pid,sizeof(int));
    agregar_uint32_a_paquete(paquete, pc);
	enviar_paquete(fd_memoria,paquete);
	eliminar_paquete(paquete);
}
void pedir_frame_memoria(int pagina){
    t_paquete *paquete = crear_paquete(FRAME);
    agregar_a_paquete(paquete,&pagina,sizeof(int));
    enviar_paquete(fd_memoria,paquete);
}
void recibir_frame(int *frame){
    t_codigo_operacion op_code;
    t_buffer * buffer = crear_buffer();

    recibir_paquete(fd_memoria,&op_code,buffer);

    if(op_code != FRAME){
        log_info(cpu_logger,"CODIGO DE OPERACION RECIBIDO NO ES FRAME, REICBI %d",op_code);
        return;
    }
    buffer_desempaquetar(buffer,frame);
    log_info(cpu_logger,"FRAME RECIBIDO: %d", *frame);

}

void insertar_ordenado_lista_interrupciones(t_interrupt interrupcion_nueva) {
    nodo_t* actual = lista_interrupciones;
    nodo_t* anterior = NULL;

    while (actual != NULL && actual->interrupcion.pid != interrupcion_nueva.pid) {
        anterior = actual;
        actual = actual->siguiente;
    }

    if (actual != NULL) {
        // Si el PID ya existe, solo actualizamos si la nueva interrupción tiene mayor prioridad
        if (interrupcion_nueva.motivo_desalojo == INTERRUPT_USER) {
            actual->interrupcion.motivo_desalojo = interrupcion_nueva.motivo_desalojo;
        }
    } else {
        // Si el PID no existe, insertamos una nueva interrupción
        nodo_t* nuevo_nodo = (nodo_t*)malloc(sizeof(nodo_t));
        nuevo_nodo->interrupcion = interrupcion_nueva;
        nuevo_nodo->siguiente = NULL;

        if (anterior == NULL) {
            // Insertar al inicio de la lista
            lista_interrupciones = nuevo_nodo;
        } else {
            // Insertar en el medio o al final de la lista
            anterior->siguiente = nuevo_nodo;
        }
    }
}

// Función para quitar una interrupción de la lista por PID
void quitar_lista_interrupciones(int pid_sacar) {
    nodo_t* actual = lista_interrupciones;
    nodo_t* anterior = NULL;

    while (actual != NULL && actual->interrupcion.pid != pid_sacar) {
        anterior = actual;
        actual = actual->siguiente;
    }

    if (actual != NULL) {
        if (anterior == NULL) {
            lista_interrupciones = actual->siguiente;
        } else {
            anterior->siguiente = actual->siguiente;
        }
        free(actual);
    }
}

// Función para buscar una interrupción en la lista por PID
int buscar_lista_interrupciones(int pid_buscar) {
    nodo_t* actual = lista_interrupciones;

    while (actual != NULL) {
        if (actual->interrupcion.pid == pid_buscar) {
            return 1; // Encontrado
        }
        actual = actual->siguiente;
    }

    return 0; // No encontrado
}

void check_interrupt(){
    if (salir_ciclo_instruccion){
        //limpiar
        quitar_lista_interrupciones(pcb.pid);
    }
    else{
        if(buscar_lista_interrupciones(pcb.pid)){ //deberia hacer esto si la instruccion quiere continuar ejecutando normalmente verdad?
            salir_ciclo_instruccion=1;
            motivo_desalojo = obtener_motivo_lista(pcb.pid);
            enviar_pcb_kernel(motivo_desalojo);
        }
    }
    
}


int obtener_motivo_lista(int pid_buscar){
    nodo_t* actual = lista_interrupciones;

    while (actual != NULL) {
        if (actual->interrupcion.pid == pid_buscar) {
            return actual->interrupcion.motivo_desalojo; // Encontrado
        }
        actual = actual->siguiente;
    }
    return -1;
}
// Función de prueba para imprimir la lista
void imprimir_lista() {
    nodo_t* actual = lista_interrupciones;
    while (actual != NULL) {
        printf("PID: %d, Motivo: %d\n", actual->interrupcion.pid, actual->interrupcion.motivo_desalojo);
        actual = actual->siguiente;
    }
}

/*void recibir_pcb(int socket_cliente){
	t_list *pcb_como_lista = recibir_paquete(socket_cliente);
	t_pcb pcb;
	
}*/
// void agregar_instruccion_paquete(t_paquete *paquete, char* instruccion)
// {
// 	agregar_a_paquete(paquete, instruccion, string_length(instruccion));
// }

// void agregar_pcb_paquete(t_paquete *paquete, t_pcb *pcb) {
// 	agregar_a_paquete(paquete, &(pcb->pid), sizeof(int));
// 	agregar_a_paquete(paquete, &(pcb->registros.pc), 8);
// 	agregar_a_paquete(paquete, &(pcb->registros.ax), 2);
// 	agregar_a_paquete(paquete, &(pcb->registros.bx), 2);
// 	agregar_a_paquete(paquete, &(pcb->registros.cx), 2);
// 	agregar_a_paquete(paquete, &(pcb->registros.dx), 2);
// 	agregar_a_paquete(paquete, &(pcb->registros.eax), 8);
// 	agregar_a_paquete(paquete, &(pcb->registros.ebx), 8);
// 	agregar_a_paquete(paquete, &(pcb->registros.ecx), 8);
// 	agregar_a_paquete(paquete, &(pcb->registros.edx), 8);
// 	agregar_a_paquete(paquete, &(pcb->registros.si), 8);
// 	agregar_a_paquete(paquete, &(pcb->registros.di), 8);
// }

/*char* recibir_instruccion(int socket_cliente) //no se quien hizo esto pero tiene que retornar un char* y no retorna nada
{
	t_list *recibir_instruccion = recibir_paquete(socket_cliente);
	char* instruccion_completa;
	strcpy (instruccion_completa, recibir_instruccion->head->data);
	char **instruccion_parciada = string_split(instruccion_completa, " ");
	ejecutar_instruccion(instruccion_parciada); //no se llama asi me parece
}*/

