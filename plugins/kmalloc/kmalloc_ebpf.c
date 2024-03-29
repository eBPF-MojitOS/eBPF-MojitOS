#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <linux/bpf.h>
#include <signal.h>
#include "util.h"
#include "kmalloc_ebpf.skel.h"


#define NB_DATA 6
#define NB_SENSOR 2


#define ERROR_OPEN_PROG -1
#define ERROR_LOAD_PROG -2
#define ERROR_ACCESS_ELEM -3


char *_labels_kmalloc_ebpf[NB_SENSOR] = {
    "%s:bytes_req",
    "%s:bytes_alloc",
};

struct Kmalloc {
    uint64_t values[NB_SENSOR];
    uint64_t tmp_values[NB_SENSOR];
    struct kmalloc_ebpf_bpf *skel;
    char labels[NB_SENSOR][128];
    int error;
    int ndata;
};

typedef struct Kmalloc Kmalloc;

//Capture du signal pour arrêter le programme ( à enlever )
int fin = 0;
static void signaltrap(int signo)
{
    printf("Signal %d received\n", signo);
    fin = 1;
}

unsigned int init_kmalloc_ebpf(char *, void **){


    if(dev==NULL){
        exit(1);
    }

    struct Kmalloc*state = malloc(sizeof(struct Kmalloc));
    memset(state, '\0', sizeof(*state));

    state->skel = kmalloc_ebpf_bpf__open();

    if(!(state->skel)){
        printf("Impossible d'ouvrir le programme\n");
        state->error=ERROR_OPEN_PROG;
        exit(ERROR_OPEN_PROG);
    }


    if( kmalloc_ebpf_bpf__load(state->skel) < 0 ){
        printf("impossible de charger le programme dans le kernel\n");
        state->error=ERROR_LOAD_PROG;
        clean_kmalloc_ebpf(state);
        exit(ERROR_LOAD_PROG);
        
    }

    state->ndata=NB_SENSOR;

    *ptr = (void *) state;



    return NB_SENSOR;
}

unsigned int get_kmalloc_ebpf(uint64_t *results, void *ptr){


    Kmalloc *state = ( Kmalloc *)ptr;

    uint64_t bytes;



    for(int i=0;i<state->ndata;i++){
        if (bpf_map__lookup_elem(state->skel,&i,sizeof(int),&bytes,sizeof(uint64_t),BPF_ANY) <0){
            printf("Erreur : impossible de lire les informations contenus dans les maps \n");
            state->error= ERROR_ACCESS_ELEM;
            clean_kmalloc_ebpf(state);
            exit(ERROR_ACCESS_ELEM)
        }

  
        results[i] = modulo_substraction(bytes,state->tmp_values[i]);

        state->tmp_values[i]= bytes;

    }

    

    return state->ndata;
}

void clean_kmalloc_ebpf(void *){


    Kmalloc *state = ( Kmalloc *)ptr;


    if (state == NULL) {
        return;
    }


    if ( state ->error < -1 || state->error == 0 ){

        if ( state->error == -3 || state->error == 0 ){

            network_ebpf_ingress_bpf__detach(state->skel);
        }

        network_ebpf_ingress_bpf__destroy(state->skel);
    }

    free(state->skel);
    free(state);



}

void label_kmalloc_ebpf(char **labels, void *){


    struct Kmalloc *state = (struct Kmalloc *) ptr;

    for (int i = 0; i < state->ndata; i++) {
        labels[i] = state->labels[i];
    }
}



//----------à elever lors de la release----------------------//


int main(void)
{
    signal(SIGINT, signaltrap);


    struct test_bpf *skel = test_bpf__open();
    test_bpf__load(skel);
    test_bpf__attach(skel);

	while(true){

        if (fin)
            break;
	}

    //On détache le programme bpf
    test_bpf__detach(skel);
    test_bpf__destroy(skel);

    return 0;
}