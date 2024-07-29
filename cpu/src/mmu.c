#include "../include/mmu.h"
t_tlb tlb;

int min(int a, int b) {
    return (a < b) ? a : b;
}
// int *min_puntero(int a, int b) {
//     return (a < b) ? &a : &b;
// }

int consultar_tlb(int pid,int pagina){
    
    bool esta_la_pagina(void *entrada_void){
        t_entrada_tlb *entrada = (t_entrada_tlb *)entrada_void;
        return ((entrada->pid ==pid )&&(entrada->pagina==pagina));
    };
    t_entrada_tlb *entrada=list_find(tlb.entradas,esta_la_pagina);
    
    if(entrada!=NULL){
        log_info(cpu_logger_obligatorio,"PID: %d- TLB HIT - Pagina: %d",pid,pagina);
        entrada->tiempo=tlb.tiempo_actual;
        tlb.tiempo_actual++;
        return entrada->frame;
    }
    else{
        log_info(cpu_logger_obligatorio,"PID: %d- TLB MISS - Pagina: %d",pid,pagina);
        return -1;
    }

}
// int esta_la_pagina(void *entrada_void,int pid,int pagina){
//     t_entrada_tlb *entrada = (t_entrada_tlb *)entrada_void;
//     return ((entrada->pid ==pid )&&(entrada->pagina==pagina));

// }
void agregar_entrada_tlb(int pid,int pagina, int frame){
    t_entrada_tlb *nuevo = malloc(sizeof(t_entrada_tlb));
    nuevo->frame=frame;
    nuevo->pagina=pagina;
    nuevo->pid=pid;
    nuevo->tiempo = tlb.tiempo_actual;
    tlb.tiempo_actual++;
    if(list_size(tlb.entradas)<CANTIDAD_ENTRADAS_TLB){
        log_info(cpu_logger,"Agrego entrada \n");
        list_add(tlb.entradas,nuevo);
    }
    else{
        log_info(cpu_logger,"Reemplazo entrada \n");
        reemplazar_entrada(nuevo);
    }
}
void reemplazar_entrada(t_entrada_tlb *nuevo){
    if(strcmp(ALGORITMO_TLB,"FIFO")==0){
        reemplazar_fifo(nuevo);
    }
    else if(strcmp(ALGORITMO_TLB,"LRU")==0){
        reemplazar_lru(nuevo);
        
    }

} 
void reemplazar_fifo(t_entrada_tlb * nuevo){
    //log_info(cpu_logger,"Entrada a reemplazar: %d \n",tlb.reemplazar_fifo);
    list_replace_and_destroy_element(tlb.entradas,tlb.reemplazar_fifo,nuevo,free);
    tlb.reemplazar_fifo++;
    if(tlb.reemplazar_fifo == CANTIDAD_ENTRADAS_TLB){
        tlb.reemplazar_fifo=0;
    }
}
void reemplazar_lru(t_entrada_tlb * nuevo){
    t_entrada_tlb *reemplazar = list_get_minimum(tlb.entradas,comparar_tiempo);
    //log_info(cpu_logger,"Entrada a reemplazar (pagina): %d | Tiempo: %d \n", reemplazar->pagina, reemplazar->tiempo);
    list_remove_element(tlb.entradas,reemplazar);
    free(reemplazar);
    list_add(tlb.entradas,nuevo);
}

void *comparar_tiempo(void *entrada1_void,void * entrada2_void){
    t_entrada_tlb * entrada1 = (t_entrada_tlb *)entrada1_void;
    t_entrada_tlb * entrada2 = (t_entrada_tlb *)entrada2_void;
    if( min(entrada1->tiempo,entrada2->tiempo) == entrada1->tiempo ){
        return entrada1_void;
    }
    else{
        return entrada2_void;
    }
}

int obtener_pagina(int dl){
    return floor(dl/tamanio_pagina);
}

int obtener_offset(int dl, int pagina){
    return dl - (pagina * tamanio_pagina);
}
int obtener_frame(int pagina){
    int frame;
    if(CANTIDAD_ENTRADAS_TLB!=0){
        frame = consultar_tlb(pcb.pid,pagina);
        if(frame==-1){
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
                log_info(cpu_logger_obligatorio,"PID: %d - OBTENER MARCO - Página: %d - Marco: %d",pcb.pid,pagina,frame);
                agregar_entrada_tlb(pcb.pid,pagina,frame);
            }
            eliminar_buffer(buffer);
        }
    } 
    else{
        t_paquete *paquete = crear_paquete(FRAME);
        agregar_int_a_paquete(paquete,pcb.pid);
        agregar_int_a_paquete(paquete,pagina);
        enviar_paquete(fd_memoria,paquete);
        eliminar_paquete(paquete);
        t_buffer * buffer = crear_buffer();
        t_codigo_operacion cop;
        recibir_paquete(fd_memoria,&cop,buffer);
        if(cop!=FRAME){
            //log_info(cpu_logger,"ERROR AL RECIBIR EL FRAME DE MEMORIA");
        }
        else{
            buffer_desempaquetar(buffer,&frame);
            log_info(cpu_logger_obligatorio,"PID: %d - OBTENER MARCO - Página: %d - Marco: %d",pcb.pid,pagina,frame);
        }
        eliminar_buffer(buffer);
    }
    return frame;
}
/*Obtener Marco: PID: <PID> - OBTENER MARCO - Página: <NUMERO_PAGINA> - Marco: <NUMERO_MARCO>.*/
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
void mmu_leer_copy_string(int dl,int tamanio,void *cadena){
    int pagina = obtener_pagina(dl);
    int offset = obtener_offset(dl,pagina);
    int bytes_leidos;
    int df;
    int desplazamiento=0;
    char *aux=malloc(tamanio+1);
    
    while(tamanio>0){
        bytes_leidos=min(tamanio_pagina-offset,tamanio);
        df=obtener_df(dl);
        
        void * dato_partido=malloc(bytes_leidos);
        leer_un_frame(df,bytes_leidos,dato_partido);
        memcpy(cadena+desplazamiento,dato_partido,bytes_leidos);

        //SE PUEDE BORRAR
        memmove(aux+desplazamiento,dato_partido,bytes_leidos);
        aux[desplazamiento+bytes_leidos]='\0';
        //log_info(cpu_logger,"Estado cadena : %s",aux);
        //HASTA ACA
        
        free(dato_partido);
        tamanio-=bytes_leidos;
        offset=0;
        dl+=bytes_leidos;
        desplazamiento+=bytes_leidos;
    }
    free(aux);

}

void mmu_agregar_dirs_tamanio_paquete(int dl,int tamanio,t_paquete *paquete){
    int pagina = obtener_pagina(dl);
    int offset = obtener_offset(dl,pagina);
    int bytes_usar;
    int df;
    while (tamanio>0)
    {
        bytes_usar=min(tamanio_pagina-offset,tamanio);
        df = obtener_df(dl);
        agregar_int_a_paquete(paquete,df);
        agregar_int_a_paquete(paquete,bytes_usar);
        tamanio-=bytes_usar;
        dl+=bytes_usar;
    }
}
void mmu_escribir_copy_string(int dl,int tamanio,void * cadena){

    int pagina = obtener_pagina(dl);
    int offset = obtener_offset(dl,pagina);
    int bytes_usar;
    int df;
    int copiar_desde=0;
    while (tamanio>0)
    {
        bytes_usar= min(tamanio_pagina-offset,tamanio);
        df = obtener_df(dl);
        void *textoCortado = malloc(bytes_usar);
        memmove(textoCortado, cadena + copiar_desde, bytes_usar);
        escribir_un_frame(df,bytes_usar,textoCortado);
        tamanio-=bytes_usar;
        dl+=bytes_usar;
        copiar_desde+=bytes_usar;
        free(textoCortado);
        offset=0;
    }
}
int obtener_cantidad_dirs(int dl,int tamanio){
    int pagina = obtener_pagina(dl);
    int offset = obtener_offset(dl,pagina);
    int i=0;
    int bytes_usar=min(tamanio_pagina-offset,tamanio);
    while (tamanio>0)
    {
        tamanio-=bytes_usar;
        i++;
    }
    return i;

}
void leer_un_frame(int df, int bytes, void * dato){
    t_paquete *paquete = crear_paquete(SOLICITUD_LECTURA);
    agregar_a_paquete(paquete,&(pcb.pid),sizeof(int));
    agregar_a_paquete(paquete,&df,sizeof(int));
    agregar_a_paquete(paquete,&bytes, sizeof(int));
    enviar_paquete(fd_memoria,paquete);
    eliminar_paquete(paquete);

    t_codigo_operacion cop;
    t_buffer * buffer = crear_buffer();
    recibir_paquete(fd_memoria,&cop,buffer);

    if(cop!= DATO){
        //log_info(cpu_logger,"ERROR NO SE PUDO LEER EN MEMORIA, me llego un op_code distinto de DATO");
    }
    else{
        buffer_desempaquetar(buffer,dato);
        char * aux=malloc(bytes+1);
        memcpy(aux,dato,bytes);
        aux[bytes]='\0';
        log_info(cpu_logger_obligatorio,"PID: %d - Acción: LEER - Dirección Física: %d - Valor: %d", pcb.pid,df,(int)aux);
    }
    eliminar_buffer(buffer);
}


void mmu_leer(int dl, int bytes, void * valor){
    int pagina = obtener_pagina(dl);
    int offset = obtener_offset(dl,pagina);
    int df = obtener_df(dl);
    //log_info(cpu_logger,"PID: %d - Acción: LEER -Pagina : %d Offset: %d  Bytes: %d Dirección Física: %d - Valor: %d", pcb.pid,pagina,offset,bytes,df,(uint8_t)valor);    
    if(bytes==1){
        leer_un_byte(df,valor);
    }
    else{
        if( offset != 0 ){
            //puede que tengamos que leer dos pags y juntarlo
            int bytes_leer = min(tamanio_pagina -offset,bytes);
            bytes -= bytes_leer;
            void * dato_parte_1 = malloc(bytes_leer);
            leer_un_frame(df,bytes_leer,dato_parte_1);
            memcpy(valor, dato_parte_1 , bytes_leer);
            if(bytes>0){
                df = obtener_df(dl + bytes_leer);
                void *dato_parte_2 = malloc(bytes);
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
        //log_info(cpu_logger,"ERROR NO SE PUDO ESCRIBIR EN MEMORIA");
    }
    else{
        log_info(cpu_logger_obligatorio,"PID: %d - Acción: ESCRIBIR - Dirección Física: %d - Valor: %d", pcb.pid,df,(int)valor);
    }
}

void escribir_un_byte(int df,void *regd){
    escribir_un_frame(df,1,regd);
}
void leer_un_byte(int df,void *valor){
    leer_un_frame(df,1,valor);
}


