#include <utils/planificadores.h>


<<<<<<< HEAD
struct{
   
=======
typedef struct{
    __uint32_t pc;
>>>>>>> 9519930d52bcdfc5bfcf5c20b69baaee0325214a
    __uint8_t AX;
    __uint8_t BX;
    __uint8_t CX;
    __uint8_t DX;
    __uint32_t EAX;
    __uint32_t EBX;
    __uint32_t ECX;
    __uint32_t EDX;
    __uint32_t SI; //contiene la direccion logica de memoria de origen desde donde se va a copir un string
    __uint32_t DI;  //contiene la direccion logica de memoria de destino desde donde se va a copir un string
}t_registros;


<<<<<<< HEAD
struct{
    __uint32_t pc;
=======
typedef struct{
>>>>>>> 9519930d52bcdfc5bfcf5c20b69baaee0325214a
    int pid;
    int quantum;
    t_registros registros;
}t_pcb;