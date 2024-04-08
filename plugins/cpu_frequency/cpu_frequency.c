/*******************************************************
 Copyright (C) 2018-2023 Georges Da Costa <georges.da-costa@irit.fr>

    This file is part of Mojitos.

    Mojitos is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Mojitos is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MojitO/S.  If not, see <https://www.gnu.org/licenses/>.

 *******************************************************/

/*here, you just have to modify every word "plugin" by your word"*/

#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h> 
#include <sys/resource.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <net/if.h>
//#include "plugin_ebpf.h"
#include "cpu_frequency.skel.h"

#define ERROR_OPEN_PROG -1
#define ERROR_LOAD_PROG -2
#define ERROR_ACCESS_ELEM -3

#define NB_SENSOR 1
#define NB_DATA 1

char *_labels_plugin_ebpf[NB_SENSOR] = {
    "%s:frequency",
};

struct Freq {
    uint64_t values[NB_DATA];
    uint64_t tmp_values[NB_DATA];
    struct cpu_frequency *skel;
    char labels[NB_DATA][128];
    int error;
};

typedef struct Freq Freq;


unsigned int init_cpu_frequency_ebpf(char *dev, void **ptr)
{

    if(dev==NULL){
        exit(1);
    }

    struct Plugin *state = malloc(sizeof(struct Plugin));
    memset(state, '\0', sizeof(*state));

    state->skel = cpu_frequency_bpf__open();

    if(!(state->skel)){
        printf("Impossible d'ouvrir le programme\n");
        state->error=ERROR_OPEN_PROG;
        exit(ERROR_OPEN_PROG);
    }


    if( cpu_frequency_bpf__load(state->skel) < 0 ){
        printf("impossible de charger le programme dans le kernel\n");
        state->error=ERROR_LOAD_PROG;
        clean_cpu_frequency_ebpf(state);
        exit(ERROR_LOAD_PROG);
        
    }
    *ptr = (void *) state;



    return NB_SENSOR;
    
}




/* récupère les données */
unsigned int get_cpu_frequency_ebpf(uint64_t *results, void *ptr)
{
    Freq *state = ( Freq *)ptr;

    int val;


    for (int i = 0; i < NB_DATA; i++) {

        if (bpf_map__lookup_elem(state->skel,&i,sizeof(int),&val,sizeof(int),BPF_ANY) <0){
            printf("Erreur : impossible de lire les informations contenus dans les maps \n");
            state->error=ERROR_ACCESS_ELEM;
            clean_cpu_frequency_ebpf(state);
            exit(ERROR_ACCESS_ELEM);
        }


        results[i] = state->tmp_values[i];
        
        state->tmp_values[i] = val; 

    }

    return NB_DATA;
}

void clean_cpu_frequency_ebpf(void *ptr){
    Freq *state = ( Freq *)ptr;

    if (state == NULL) {
        return;
    }

    if ( state ->error < -1 || state->error == 0 ){

        if ( state->error < -3 || state->error == 0 ){

            cpu_frequency_bpf__detach(state->skel);
        }

        cpu_frequency_bpf__destroy(state->skel);
    }
    free(state->skel);
    free(state);
}


void label_cpu_frequency_ebpf(char **labels, void *ptr)
{
    struct Freq *state = (struct Freq *) ptr;

    for (int i = 0; i < state->ndev; i++) {
        labels[i] = state->labels[i];
    }
}