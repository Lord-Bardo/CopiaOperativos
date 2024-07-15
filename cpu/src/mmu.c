#include "../include/mmu.h"

int min(int a, int b) {
    return (a < b) ? a : b;
}


int obtener_pagina(int dl){
    return floor(dl/tamanio_pagina);
}

int obtener_offset(int dl, int pagina){
    return dl - (pagina * tamanio_pagina);;
}
int obtener_frame(int pagina){
	if(pagina<10){
		int memoria[10]= {5,1,7,23,7,8,1,8,3,69};
		return memoria[pagina];
	}
	return -1;
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
        direcciones[i].frame = obtener_frame(pagina);
        direcciones[i].offset = offset;
        direcciones[i].bytes = bytes;

        offset = 0;
        tamanio_total -= bytes;
        pagina ++;
        i++;
    }

}







