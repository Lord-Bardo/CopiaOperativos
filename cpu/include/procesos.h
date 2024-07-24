#ifndef PROCESOS_H_
#define PROCESOS_H_


#include "cpu_gestor.h"
#include "cpu_utils.h"


void inicializarPCBAleatorio(t_pcb *pcb);
void copiarContexto(t_pcb pcb_recibido);
void mostrarPCB(t_pcb auxiliar);
void inicializarPCB(t_pcb *pcb);


int es_reg_chico(char *registro);

uint32_t get_reg_grande(char* registro_string);
uint8_t get_reg_chico(char* registro_string);

void set_reg_chico(char * registro, uint8_t valor);
void set_reg_grande(char * registro, uint32_t valor);

void set_reg_ax(uint8_t valor);
void set_reg_bx(uint8_t valor);
void set_reg_cx(uint8_t valor);
void set_reg_dx(uint8_t valor);
void set_reg_eax(uint32_t valor);
void set_reg_ebx(uint32_t valor);
void set_reg_ecx(uint32_t valor);
void set_reg_edx(uint32_t valor);
void set_reg_pc(uint32_t valor);
void set_reg_di(uint32_t valor);
void set_reg_si(uint32_t valor);

uint8_t get_reg_ax(void);
uint8_t get_reg_bx(void);
uint8_t get_reg_cx(void);
uint8_t get_reg_dx(void);
uint32_t get_reg_eax(void);
uint32_t get_reg_ebx(void);
uint32_t get_reg_ecx(void);
uint32_t get_reg_edx(void);
uint32_t get_reg_di(void);
uint32_t get_reg_si(void);

void set_pid(int valor);
void set_pc(uint32_t valor);

int get_pid(void);
uint32_t get_pc(void);


#endif