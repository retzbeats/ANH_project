/*
 * Copyright © 2008-2010 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h> /* (Stadard Input/Output Header) Contiene las definiciones necesarias para las operaciones de las in/out*/ 
#include <unistd.h> /* Heade File que provee acceso al API POSIX de los sistemas UNIX (Base de las especificaciones unicas de UNIX)*/
#include <stdlib.h> /* (Standard Library).Estándar de propósito general de C que contiene las funciones para gestión de memoria dinámica, control de procesos y otras*/
#include <errno.h> /* Librerias de C que define los macros para los informes de códigos*/

#include <modbus.h> /*Libreria de modbus*/

int main(void)
{
    int socket; //declaracion de un entero socket
    modbus_t *ctx; // declaracion de apuntador a estructura subtipo modbus_t, ver modbus.h
    modbus_mapping_t *mb_mapping; // declaracion de apuntador a estructura tipo modbus_mapping_t ver modbus.h

    ctx = modbus_new_tcp("10.42.0.66", 1502); //almacena en ctx la ip y el puerto en ctx que es de tipo modbus_t
    /*modbus new tcp crea el contexto TCP para TCP/IPv4, el contexto tiene la direccion ip de esclavo modbus, el puerto
     y los datos del backend*/ 
    /* modbus_set_debug(ctx, TRUE); */

    mb_mapping = modbus_mapping_new(500, 500, 500, 500); 
    /*crea un modbus mapping limpio en la variable mb_mapping el primer parametro es el numero de coils, el segundo inputs
    el tercero holding registers y el cuarto input registers*/
    if (mb_mapping == NULL) {
        fprintf(stderr, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    socket = modbus_tcp_listen(ctx, 1);
    modbus_tcp_accept(ctx, &socket);

    for (;;) {
        uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH]; /*Type: unsigned char, busca registro que correspondan a la máxima longitud del paquete Modbus TCP/IP */
        int rc; /*Declaración rc de tipo entero*/

        rc = modbus_receive(ctx, query); /**/
        if (rc != -1) {
            /* rc is the query size */
            modbus_reply(ctx, query, rc, mb_mapping);
        } else {
            /* Connection closed by the client or error */
            break;
        }
    }

    printf("Quit the loop: %s\n", modbus_strerror(errno));

    modbus_mapping_free(mb_mapping);
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
