
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
    buffer_desempaquetar_registros(buffer, &(pcb->registros));
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
void agregar_uint32_a_paquete(t_paquete *paquete, uint32_t n){
    agregar_a_paquete(paquete, &n, sizeof(uint32_t));
}

void agregar_uint8_a_paquete(t_paquete *paquete, uint8_t n){
    agregar_a_paquete(paquete, &n, sizeof(uint8_t));
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

	if(op_code!= FETCH){
		log_info(cpu_logger,"ERROR AL RECIBIR LA INSTRUCCION DE MEMORIA, el codigo de operacion no es FETCH");
		return;
	}
	
	buffer_desempaquetar_instruccion(buffer,instruccion);
	eliminar_buffer(buffer);

}
void buffer_desempaquetar_instruccion(t_buffer *buffer, t_instruccion * instruccion){
	//tengo que desempaquetar 
	buffer_desempaquetar(buffer,&(instruccion->instr_code));
	buffer_desempaquetar_argumentos(buffer,instruccion);
}


void buffer_desempaquetar_argumentos(t_buffer *buffer, t_instruccion * instruccion){
	char * cadena = buffer_desempaquetar_string(buffer);

	char *token = strtok(cadena, " ");
    int i = 0;
    
    while (token != NULL && i < 5) {
        // Asignar memoria y copiar el token al array de argumentos
        instruccion->argumentos[i] = malloc(strlen(token) + 1);
        if (instruccion->argumentos[i] == NULL) {
            fprintf(stderr, "Error: No se pudo asignar memoria para el argumento.\n");
            exit(EXIT_FAILURE);
        }
        strcpy(instruccion->argumentos[i], token);
        i++;
        token = strtok(NULL, " ");
    }
    
    // Asegurar que los restantes elementos en argumentos[] sean NULL
    while (i < 5) {
        instruccion->argumentos[i] = NULL;
        i++;
    }
    
    // Liberar la cadena desempaquetada
    free(cadena);

}

void liberar_instruccion(t_instruccion *instruccion) {
    // Liberar instr_code si es necesario
    // (asumimos que no es dinámico en este ejemplo, pero podrías ajustarlo si es necesario)
    
    // Liberar los argumentos
    for (int i = 0; i < 5; i++) {
        if (instruccion->argumentos[i] != NULL) {
            free(instruccion->argumentos[i]);
            instruccion->argumentos[i] = NULL;
        }
    }
}

void enviar_fetch_memoria(int pid, u_int32_t pc){
	t_paquete * paquete = crear_paquete(FETCH);
	agregar_a_paquete(paquete, &pid,sizeof(int));
    agregar_uint32_a_paquete(paquete, pc);
	enviar_paquete(fd_memoria,paquete);
	eliminar_paquete(paquete);
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

