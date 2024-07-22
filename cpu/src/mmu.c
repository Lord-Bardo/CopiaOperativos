#include "../include/mmu.h"
int tamanio_pagina=4;
int min(int a, int b) {
    return (a < b) ? a : b;
}


int obtener_pagina(int dl){
    return floor(dl/tamanio_pagina);
}

int obtener_offset(int dl, int pagina){
    return dl - (pagina * tamanio_pagina);
}
int obtener_frame(int pagina){
    int frame;
	t_paquete *paquete = crear_paquete(FRAME);
    agregar_int_a_paquete(paquete,pcb.pid);
    agregar_int_a_paquete(paquete,pagina);
    enviar_paquete(fd_memoria,paquete);
    eliminar_paquete(paquete);
    t_buffer * buffer = crear_buffer();
    t_codigo_operacion cop;
    recibir_paquete(fd_memoria,&cop,buffer);
    if(cop!=FRAME){
        log_info(cpu_logger,"ERROR AL RECIBIR EL FRAME DE MEMORIA");
    }
    else{
        buffer_desempaquetar(buffer,&frame);
        log_info(cpu_logger,"PID: %d - OBTENER MARCO - Página: %d - Marco: %d",pcb.pid,pagina,frame);
    }
    eliminar_buffer(buffer);

    return frame;
}
/*Obtener Marco: “PID: <PID> - OBTENER MARCO - Página: <NUMERO_PAGINA> - Marco: <NUMERO_MARCO>”.*/
int obtener_df(int dl){
    int pagina = obtener_pagina(dl);
    int frame = obtener_frame(pagina);
    int offset = obtener_offset(dl,pagina);

    return  (frame *tamanio_pagina + offset); 
}

int obtener_bytes(int offset,int tamanio_total){
    return min(tamanio_total,(tamanio_pagina-offset));
}

void mmu(t_direccion *direcciones, int dl, int tamanio_total){
    int i = 0;
    int pagina = obtener_pagina(dl);
    int offset = obtener_offset(dl,pagina);
    int bytes = 0;

    while(tamanio_total>0){

        bytes = obtener_bytes(offset,tamanio_total);

        offset = 0;
        tamanio_total -= bytes;
        pagina ++;
        i++;
    }

}

void leer_un_frame(int df, int bytes, void * dato){
    t_paquete *paquete = crear_paquete(SOLICITUD_LECTURA);
    agregar_a_paquete(paquete,&(pcb.pid),sizeof(int));
    agregar_a_paquete(paquete,&df,sizeof(int));
    agregar_a_paquete(paquete,&bytes, sizeof(int));
    enviar_paquete(fd_memoria,paquete);
    eliminar_paquete(paquete);

    t_codigo_operacion cop;
    recibir_codigo_operacion(fd_memoria,&cop);

    if(cop!= DATO){
        log_info(cpu_logger,"ERROR NO SE PUDO LEER EN MEMORIA, me llego un op_code distinto de DATO");
    }
    else{
        t_buffer * buffer = crear_buffer();
        buffer_desempaquetar(buffer,dato);
        log_info(cpu_logger,"PID: %d - Acción: LEER - Dirección Física: %d - Valor: %d", pcb.pid,df,(uint32_t)dato);
        eliminar_buffer(buffer);
    }
}


void mmu_leer(int dl, int bytes, void * valor){
    int pagina = obtener_pagina(dl);
    int offset = obtener_offset(dl,pagina);
    int df = obtener_df(dl);
    log_info(cpu_logger,"PID: %d - Acción: LEER -Pagina : %d Offset: %d  Bytes: %d Dirección Física: %d - Valor: %d", pcb.pid,pagina,offset,bytes,df,valor);    
    if(bytes==1){
        leer_un_byte(df,valor);
    }
    else{
        if(offset==0){
            //puede que tengamos que leer dos pags y juntarlo
            int bytes_leer = min(tamanio_pagina -offset,bytes);
            bytes -= bytes_leer;
            void * dato_parte_1 = malloc(bytes_leer);
            leer_un_frame(df,bytes_leer,dato_parte_1);
            memcpy(valor, dato_parte_1 , bytes_leer);
            if(bytes>0){
                df = obtener_df(dl + bytes_leer);
                void * dato_parte_2 = malloc(bytes);
                leer_un_frame(df,bytes,dato_parte_2);
                memcpy(valor+bytes_leer, dato_parte_2,bytes);
                free(dato_parte_2);
            }
            free(dato_parte_1);
        }
        else{
            leer_un_frame(df,bytes,valor);
        }
    }
}


void mmu_escribir(int dl,int bytes, void *valor){
    int pagina = obtener_pagina(dl);
    int offset = obtener_offset(dl,pagina);
    int df = obtener_df(dl);
    log_info(cpu_logger,"PID: %d - Acción: ESCRIBIR -Pagina : %d Offset: %d  Bytes: %d Dirección Física: %d - Valor: %d", pcb.pid,pagina,offset,bytes,df,valor);
    if(bytes==1){
        escribir_un_byte(df,valor);
    }
    else{
        if(offset != 0){
            int bytes_escribir = min(tamanio_pagina -offset,bytes);
            bytes -= bytes_escribir;
            void * dato_parte_1 = malloc(bytes_escribir);
            memcpy(dato_parte_1, valor, bytes_escribir);
            escribir_un_frame(df,bytes_escribir,dato_parte_1);
            if(bytes>0){
                void * dato_parte_2 = malloc(bytes);
                memcpy(dato_parte_2, valor + bytes_escribir,bytes);
                df = obtener_df(dl + bytes_escribir);
                escribir_un_frame(df,bytes,dato_parte_2);
                free(dato_parte_2);
            }
            free(dato_parte_1);
        }
        else{
            escribir_un_frame(df,bytes,valor); //si nos dan tamaño pagina 2 habria que pensarlo
        }
    }
    
}


void escribir_un_frame(int df, int bytes, void *valor){
    t_paquete *paquete = crear_paquete(SOLICITUD_ESCRITURA);
    agregar_a_paquete(paquete,&(pcb.pid),sizeof(int));
    agregar_a_paquete(paquete,&df,sizeof(int));
    agregar_a_paquete(paquete,&bytes, sizeof(int));
    agregar_a_paquete(paquete,valor,bytes);
    enviar_paquete(fd_memoria,paquete);
    eliminar_paquete(paquete);

    t_codigo_operacion cop;
    recibir_codigo_operacion(fd_memoria,&cop);

    if(cop!= CONFIRMACION_ESCRITURA){
        log_info(cpu_logger,"ERROR NO SE PUDO ESCRIBIR EN MEMORIA");
    }
    else{
        log_info(cpu_logger,"PID: %d - Acción: ESCRIBIR - Dirección Física: %d - Valor: %d", pcb.pid,df,(uint8_t)valor);
    }
}

void escribir_un_byte(int df,void *regd){
    escribir_un_frame(df,1,regd);
}
void leer_un_byte(int df,void *valor){
    leer_un_frame(df,1,valor);
}




