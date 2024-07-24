#include "../include/procesos.h"

t_pcb pcb;




uint8_t get_reg_chico(char* registro_string) {
    if (strcmp("AX", registro_string) == 0) {
        return get_reg_ax();
    }
    if (strcmp("BX", registro_string) == 0) {
        return get_reg_bx();
    }
    if (strcmp("CX", registro_string) == 0) {
        return get_reg_cx();
    }
    if (strcmp("DX", registro_string) == 0) {
        return get_reg_dx();
    }
    return 0;  // Valor de retorno predeterminado en caso de no encontrar el registro
}

uint32_t get_reg_grande(char* registro_string) {
    if (strcmp("EAX", registro_string) == 0) {
        return get_reg_eax();
    }
    if (strcmp("EBX", registro_string) == 0) {
        return get_reg_ebx();
    }
    if (strcmp("ECX", registro_string) == 0) {
        return get_reg_ecx();
    }
    if (strcmp("EDX", registro_string) == 0) {
        return get_reg_edx();
    }
    return 0;  // Valor de retorno predeterminado en caso de no encontrar el registro
}


int es_reg_chico(char *registro){
    return ((strcmp("AX",registro)== 0) || (strcmp("BX",registro)== 0) ||  (strcmp("CX",registro)== 0) || (strcmp("DX",registro)== 0));
}
void set_reg_chico(char * registro_string, uint8_t valor){
    if(strcmp("AX",registro_string)== 0){
		set_reg_ax(valor);
	}
	if(strcmp("BX",registro_string)== 0){
		set_reg_bx(valor);
	}
	if(strcmp("CX",registro_string)== 0){
		set_reg_cx(valor);
	}
	if(strcmp("DX",registro_string)== 0){
		set_reg_dx(valor);
	}

}
void set_reg_grande(char * registro_string, uint32_t valor){
    if(strcmp("EAX",registro_string)== 0){
		set_reg_eax(valor);
	}
    else{
        if(strcmp("EBX",registro_string)== 0){
            set_reg_ebx(valor);
        }
        else{
            if(strcmp("ECX",registro_string)== 0){
                set_reg_ecx(valor);
            }
            else{
                if(strcmp("EDX",registro_string)== 0){
                    set_reg_edx(valor);
                }
                else{
                    if(strcmp("PC",registro_string)== 0){
                        set_reg_pc(valor);
                    }
                    else{
                        if(strcmp("DI",registro_string)== 0){
                        set_reg_di(valor);
                        }
                        else{
                            if(strcmp("SI",registro_string)== 0){
                                set_reg_si(valor);
                            }
                        }
                    }
                }
            }
        }
    }
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

// Funciones para setear registros en t_registros
void set_reg_ax(uint8_t valor) {
    pcb.registros.ax = valor;
}

void set_reg_bx(uint8_t valor) {
    pcb.registros.bx = valor;
}

void set_reg_cx(uint8_t valor) {
    pcb.registros.cx = valor;
}

void set_reg_dx(uint8_t valor) {
    pcb.registros.dx = valor;
}

void set_reg_eax(uint32_t valor) {
    pcb.registros.eax = valor;
}

void set_reg_ebx(uint32_t valor) {
    pcb.registros.ebx = valor;
}

void set_reg_ecx(uint32_t valor) {
    pcb.registros.ecx = valor;
}

void set_reg_edx(uint32_t valor) {
    pcb.registros.edx = valor;
}

void set_reg_pc(uint32_t valor) {
    pcb.pc = valor;
}

void set_reg_di(uint32_t valor) {
    pcb.registros.di = valor;
}

void set_reg_si(uint32_t valor) {
    pcb.registros.si = valor;
}

// Funciones para obtener registros en t_registros
uint8_t get_reg_ax(void) {
    return pcb.registros.ax;
}

uint8_t get_reg_bx(void) {
    return pcb.registros.bx;
}

uint8_t get_reg_cx(void) {
    return pcb.registros.cx;
}

uint8_t get_reg_dx(void) {
    return pcb.registros.dx;
}

uint32_t get_reg_eax(void) {
    return pcb.registros.eax;
}

uint32_t get_reg_ebx(void) {
    return pcb.registros.ebx;
}

uint32_t get_reg_ecx(void) {
    return pcb.registros.ecx;
}

uint32_t get_reg_edx(void) {
    return pcb.registros.edx;
}

uint32_t get_reg_di(void) {
    return pcb.registros.di;
}

uint32_t get_reg_si(void) {
    return pcb.registros.si;
}

// Funciones para setear pid y pc en t_pcb
void set_pid(int valor) {
    pcb.pid = valor;
}

void set_pc(uint32_t valor) {
    pcb.pc = valor;
}

// Funciones para obtener pid y pc en t_pcb
int get_pid(void) {
    return pcb.pid;
}

uint32_t get_pc(void) {
    return pcb.pc;
}






