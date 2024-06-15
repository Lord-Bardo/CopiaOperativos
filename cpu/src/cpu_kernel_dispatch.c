#include "../include/cpu_kernel_dispatch.h"

void atender_cpu_kernel_dispatch(){
    int continuar = 1;
	while( continuar ){
		t_codigo_operacion *cod_op=NULL;
		t_buffer * buffer = crear_buffer();
		recibir_paquete(fd_memoria,cod_op,buffer);
		switch(*cod_op){
			case INSTRUCCION: // ... aca hay que recibir bb recive_buffer size instruccion
				
				break;
			case CONTEXTO_DE_EJECUCION:
                t_pcb *pcb_recibido;
	            inicializarPCB(&pcb);
                //inicializarPCBAleatorio(&pcb_recibido);
                mostrarPCB(pcb);

				buffer_desempaquetar_pcb(buffer,pcb_recibido); //esto recibe todo el paquete desde kernel

				iniciar_ciclo_instruccion(*pcb_recibido);
			case -1:
				log_error(cpu_logger, "KERNEL se desconecto del servidor CPU DISPATCH!");
				continuar = 0;
				break;
				// return EXIT_FAILURE; // Salida un poco drastica, deberia poder seguir andando la CPU aunque se desconecte el Kernel. Lo cambio por lo de continuar y un break
			default:
				log_warning(cpu_logger, "CPU DISPATCH: Operacion desconocida recibida de KERNEL");
				break;
		}
	}
}

void iniciar_ciclo_instruccion(t_pcb pcb_recibido){
	copiarContexto(pcb_recibido);
	mostrarPCB(pcb);
	t_instruccion* instr = fetch(pcb.pid, pcb.registros.pc);
    t_paquete *paquete_pcb = crear_paquete(FINALIZACION);
    
   
    //aca deberiamos mandarle a memoria un pc y pid para que nos devuelva una instruccion.
	
	if (instr != NULL) {
        mostrarInstruccion(*instr);
        execute(instr);
        liberarInstruccion(instr);
        //devolder a kernel
        
    } else {
        printf("Error al obtener la instrucción de memoria\n");
    }
	agregar_contexto_ejecucion_a_paquete(paquete_pcb,&pcb);
    enviar_paquete(fd_cpu_dispatch,paquete_pcb);
}
void buffer_desempaquetar_registros(t_buffer *buffer, t_registros *registros){
    buffer_desempaquetar(buffer, &(registros->pc));
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

int pcb_get_pid(t_pcb *pcb){
    return pcb->pid;
}

void buffer_desempaquetar_pcb(t_buffer *buffer, t_pcb* pcb){ // REVISAR
    int pid_recibido;
    buffer_desempaquetar(buffer, &pid_recibido);//aca creo que iria pcb->pid
    /*if( pid_recibido != pcb_get_pid(pcb) ){
        log_error(cpu_logger, "El PID recibido no se corresponde con el PID del proceso en ejecucion");
    }
*/
    buffer_desempaquetar_registros(buffer, &(pcb->registros));
}