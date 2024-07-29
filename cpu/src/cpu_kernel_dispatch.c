#include "../include/cpu_kernel_dispatch.h"


int salir_ciclo_instruccion=0;
t_codigo_operacion motivo_desalojo; 

void iniciar_ciclo_instruccion(t_pcb pcb_recibido){
	
    while(1){
        inicializarPCB(&pcb);
        motivo_desalojo = SUCCESS;
        t_codigo_operacion cod_op;
		t_buffer * buffer = crear_buffer();
        t_pcb *pcb_recibido = malloc(sizeof(t_pcb));


	    recibir_paquete(fd_kernel_dispatch,&cod_op,buffer);
       
        mostrarPCB(pcb);
		buffer_desempaquetar_pcb(buffer,pcb_recibido);
        salir_ciclo_instruccion=0;
        copiarContexto(*pcb_recibido);
	    mostrarPCB(pcb);

        while(!salir_ciclo_instruccion){
	        t_instruccion* instr = crear_instruccion();
        
            enviar_fetch_memoria(pcb.pid, pcb.pc);
          
            recibir_instruccion(instr);
            //log_info(cpu_logger,"PID: %d FETCH - Program Counter: %d",pcb.pid,pcb.pc);
            log_info(cpu_logger_obligatorio,"PID: %d FETCH - Program Counter: %d",pcb.pid,pcb.pc);
	        //aca deberiamos mandarle a memoria un pc y pid para que nos devuelva una instruccion.

	        if (instr != NULL) {
                //mostrarInstruccion(*instr);
                log_n_parametros(list_size(instr->argumentos),instr); //PID: <PID> - Ejecutando: <INSTRUCCION> - <PARAMETROS>
                execute(instr);
                if(!(instr->instr_code == JNZ || ( instr->instr_code == SET && strcmp(list_get(instr->argumentos, 0),"PC") == 0 ))){
                    pcb.pc++;
                }  
                mostrarPCB(pcb);
                
            }
            else {
                printf("Error al obtener la instrucción de memoria\n");
            }
            //check interrupt, como hago para fijarme si mellego una interrupcion de kernel?
            //alguna estructura para guardar interrupciones
            liberar_instruccion(instr);
            //si me desalojo en el execute no tengo que checkear interrupciones ni mandar el pcb despues.
            check_interrupt();
        }
        
        eliminar_buffer(buffer);
        free(pcb_recibido);
       // log_info(cpu_logger,"Cambio de proceso");
        
    }
}

char * mostrar_instr_code(t_instr_code ic){
    switch (ic){
	case EXIT:
		return "EXIT";
		break;
	case SET:
		return "SET";
		break;
	case SUM:
		return "SUM";
		break;
	case SUB:
		return "SUB";
		break;
	case JNZ:
		return "JNZ";
		break;
	case MOV_IN:
		return "MOV_IN";
		break;
	case MOV_OUT:
		return "MOV_OUT";
		break;
	case RESIZE:
		return "RESIZE";
		break;
	case COPY_STRING:
		return "COPY_STRING";
		break;
	case WAIT:
		return "WAIT";
		break;
	case SIGNAL:
		return "SIGNAL";
		break;
	case IO_GEN_SLEEP:
		return "IO_GEN_SLEEP";
		break;
	case IO_STDIN_READ:
        return "IO_STDIN_READ";
		break;
	case IO_STDOUT_WRITE:
        return "IO_STDOUT_WRITE";
		break;		
	case IO_FS_CREATE:
        return "IO_FS_CREATE";
		break;
	case IO_FS_DELETE:
		return "IO_FS_DELETE";
		break;
	case IO_FS_TRUNCATE:
        return "IO_FS_TRUNCATE";
		break;
	case IO_FS_WRITE:
        return "IO_FS_WRITE";
		break;
	case IO_FS_READ:
        return "IO_FS_READ";
		break;
	default:
        log_info(cpu_logger,"CODIGO DE INSTRUCCION DESCONOCIDO");
		break;
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