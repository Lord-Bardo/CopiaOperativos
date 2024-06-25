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
	        t_instruccion* instr ;

            enviar_fetch_memoria(pcb.pid, pcb.pc);

            recibir_instruccion(instr);

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
            liberarInstruccion(instr);
            
        }
        //mandar contexto ejecucion
        eliminar_buffer(buffer);
    }
}


