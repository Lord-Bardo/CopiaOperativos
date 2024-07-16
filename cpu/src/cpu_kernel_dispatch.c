#include "../include/cpu_kernel_dispatch.h"


int salir_ciclo_instruccion=0;
t_codigo_operacion motivo_desalojo = SUCCESS;

void iniciar_ciclo_instruccion(t_pcb pcb_recibido){
	
    while(1){
        inicializarPCB(&pcb);
        t_codigo_operacion cod_op;
		t_buffer * buffer = crear_buffer();
        t_pcb *pcb_recibido = malloc(sizeof(t_pcb));


	    recibir_paquete(fd_kernel_dispatch,&cod_op,buffer);
        printf("RECIBI PAQUETE");
        mostrarPCB(pcb);
		buffer_desempaquetar_pcb(buffer,pcb_recibido);
        salir_ciclo_instruccion=0;
        copiarContexto(*pcb_recibido);
	    mostrarPCB(pcb);

        while(!salir_ciclo_instruccion){
	        t_instruccion* instr = crear_instruccion();

            enviar_fetch_memoria(pcb.pid, pcb.pc);

            recibir_instruccion(instr);

            log_info(cpu_logger,"PID: %d FETCH - Program Counter: %d",pcb.pid,pcb.pc);
	        //aca deberiamos mandarle a memoria un pc y pid para que nos devuelva una instruccion.

	        if (instr != NULL) {
                mostrarInstruccion(*instr);
                log_info(cpu_logger,"PID: %d - Ejecutando: %d - <PARAMETROS> %s %s %s %s %s",pcb.pid,instr->instr_code,instr->argumentos[0],instr->argumentos[1],instr->argumentos[2],instr->argumentos[3],instr->argumentos[4]); //PID: <PID> - Ejecutando: <INSTRUCCION> - <PARAMETROS>
                execute(instr);
                pcb.pc++;
                mostrarPCB(pcb);
                
            }
            else {
                printf("Error al obtener la instrucción de memoria\n");
            }
            //check interrupt, como hago para fijarme si mellego una interrupcion de kernel?
            //alguna estructura para guardar interrupciones
            liberar_instruccion(instr);
            
            
        }
        enviar_pcb_kernel(motivo_desalojo);
        eliminar_buffer(buffer);
        free(pcb_recibido);
        log_info(cpu_logger,"Cambio de proceso");
        
    }
}


/*
    SUCCESS,
    INTERRUPT_QUANTUM,
    INTERRUPT_USER,
    OUT_OF_MEMORY,
    IO,
    WAIT,
    SIGNAL,
*/