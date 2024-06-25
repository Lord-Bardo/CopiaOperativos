#include "../include/cpu_kernel_dispatch.h"


int salir_ciclo_instruccion=0;

void iniciar_ciclo_instruccion(t_pcb pcb_recibido){
	inicializarPCB(&pcb);
    while(1){
        
        t_codigo_operacion cod_op;
		t_buffer * buffer = crear_buffer();
        t_pcb *pcb_recibido;


	    recibir_paquete(fd_kernel_dispatch,&cod_op,buffer);
        printf("RECIBI PAQUETE");
        mostrarPCB(pcb);
		buffer_desempaquetar_pcb(buffer,pcb_recibido);
        salir_ciclo_instruccion=0;
        copiarContexto(*pcb_recibido);
	    mostrarPCB(pcb);

        while(!salir_ciclo_instruccion){
	        t_instruccion* instr = fetch(pcb.pid, pcb.pc);
            log_info(cpu_logger,"PID: %d FETCH - Program Counter: %d",pcb.pid,pcb.pc);
	        //aca deberiamos mandarle a memoria un pc y pid para que nos devuelva una instruccion.

	        if (instr != NULL) {
                mostrarInstruccion(*instr);
                log_info(cpu_logger,"PID: %d - Ejecutando: %d - <PARAMETROS>",pcb.pid,instr->instr_code); //PID: <PID> - Ejecutando: <INSTRUCCION> - <PARAMETROS>
                execute(instr);
                pcb.pc++;
                liberarInstruccion(instr);
            }
            else {
                printf("Error al obtener la instrucción de memoria\n");
            }
            //check interrupt, como hago para fijarme si mellego una interrupcion de kernel?
            //alguna estructura para guardar interrupciones
            
        }
        //mandar contexto ejecucion
        eliminar_buffer(buffer);
    }
}


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
