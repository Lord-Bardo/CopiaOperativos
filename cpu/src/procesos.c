#include "../include/procesos.h"

t_pcb pcb;


void * obtener_registro(char *registro_string){
	if(strcmp("AX",registro_string)== 0){
		return &pcb.registros.ax;
	}
	if(strcmp("BX",registro_string)== 0){
		return &pcb.registros.bx;
	}
	if(strcmp("CX",registro_string)== 0){
		return &pcb.registros.cx;
	}
	if(strcmp("DX",registro_string)== 0){
		return &pcb.registros.dx;
	}
	if(strcmp("EAX",registro_string)== 0){
		return &pcb.registros.eax;
	}
	if(strcmp("EBX",registro_string)== 0){
		return &pcb.registros.ebx;
	}
	if(strcmp("ECX",registro_string)== 0){
		return &pcb.registros.ecx;
	}
	if(strcmp("EDX",registro_string)== 0){
		return &pcb.registros.edx;
	}
	return NULL;
}

//esto va en otor lado creo
void inicializarPCB(t_pcb *pcb) {
    pcb->pid = 0;
    pcb->pc = 0;
    pcb->registros.ax = 0;
    pcb->registros.bx = 0;
    pcb->registros.cx = 0;
    pcb->registros.dx = 0;
    pcb->registros.eax = 0;
    pcb->registros.ebx = 0;
    pcb->registros.ecx = 0;
    pcb->registros.edx = 0;
    pcb->registros.di = 0;
    pcb->registros.si = 0;
}
void mostrarPCB(t_pcb auxiliar){
	printf("PID: %d\n", auxiliar.pid);
    printf("Registros:\n");
    printf("PC: %d\n", auxiliar.pc);
    printf("AX: %d\n", auxiliar.registros.ax);
    printf("BX: %d\n", auxiliar.registros.bx);
    printf("CX: %d\n", auxiliar.registros.cx);
    printf("DX: %d\n", auxiliar.registros.dx);
    printf("EAX: %d\n", auxiliar.registros.eax);
    printf("EBX: %d\n", auxiliar.registros.ebx);
    printf("ECX: %d\n", auxiliar.registros.ecx);
    printf("EDX: %d\n", auxiliar.registros.edx);
    printf("DI: %d\n", auxiliar.registros.di);
    printf("SI: %d\n", auxiliar.registros.si);
}


void copiarContexto(t_pcb pcb_recibido){
	pcb.pid = pcb_recibido.pid;
    pcb.pc = pcb_recibido.pc;
    pcb.registros.ax = pcb_recibido.registros.ax;
    pcb.registros.bx = pcb_recibido.registros.bx;
    pcb.registros.cx = pcb_recibido.registros.cx;
    pcb.registros.dx = pcb_recibido.registros.dx;
    pcb.registros.eax = pcb_recibido.registros.eax;
    pcb.registros.ebx = pcb_recibido.registros.ebx;
    pcb.registros.ecx = pcb_recibido.registros.ecx;
    pcb.registros.edx = pcb_recibido.registros.edx;
    pcb.registros.di = pcb_recibido.registros.di;
    pcb.registros.si = pcb_recibido.registros.si;
}
void inicializarPCBAleatorio(t_pcb *pcb) {
    pcb->pid = rand();
    pcb->pc = 0;
    pcb->registros.ax = 4;
    pcb->registros.bx = (u_int8_t) rand(); //ojo aca el random x que son de int_8
    pcb->registros.cx = 6;
    pcb->registros.dx = (u_int8_t)3123410;
    pcb->registros.eax = rand();
    pcb->registros.ebx = rand();
    pcb->registros.ecx = rand();
    pcb->registros.edx = rand();
    pcb->registros.di = rand();
    pcb->registros.si = rand();
}
