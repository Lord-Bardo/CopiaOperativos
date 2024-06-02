/*  #include "../include/cpu_mmu.h" 

int obtener_direccion_fisica(int direccio_logica){
    int num_pag = floor(direccion_logica/tamanio_pag); //supongo tamanio_pag será un valor invariable y conocido por todos (global)
    int desplazamiento = dirección_logica - num_pag * tamanio_pag;

    // aca habría que enviar el numero de página a la memoria para que la memoria me devuelva el numero de frame
    
    int direccion_fisica = num_frame * tamanio_pag + desplazamiento;
    return direccion_fisica;
}  */