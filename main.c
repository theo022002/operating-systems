#include <stdio.h>

#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "auxiliary.h"
#include "child.h"


pid_t *dimiourgiaDiergasiwn(int N, unsigned int grammes, unsigned int tmimata, unsigned int grammesAnaTmima, unsigned int aitiseis, unsigned int maxGrammi);
void anamoniPaidiwn(pid_t *children_pids, int N);

int main(int argc, char** argv) 
{
    char *filename;
    void *shmp;
    int shmid, N;
    pid_t *children_pids;
    unsigned int tmimata, vathmos_katatmisis=100, grammes=0, max_grammi, aitiseis;
    FILE *file;
    struct SharedMemStruct *smsp;
    double xronikiStigmi;
    clock_t start = clock();   
    
    // Diavazei ta orismata
    if(argc != 5) 
    {
        printf("Ta orismata den einai eparkh:\n");
        printf("%s <N> <aitiseis> <vathmos_katatmisis> <arxeio>\n", argv[0]);
        return 0;
    }
    
    N = atoi(argv[1]);
    aitiseis = atoi(argv[2]);
    vathmos_katatmisis = atoi(argv[3]);
    filename = argv[4];
    
    // anoigma toy arxeiou
    file = fopen( filename, "r"); 
    
    // anagnwsi stoixeiwn tou arxeiou
    statistikaArxeiou(file, &grammes, &max_grammi);

    // an oi grammes einai ligoteres apo 1000 stamataei
    if(grammes < 1000) 
    {    
        printf("Arxeio me ligoteres apo 1000 grammes\n");
        fclose( file );
        return 0;
    }
    
    // Ypologizei ta tmimata analoga ton vathmo katatmisis
    tmimata = grammes / vathmos_katatmisis;
    if( grammes % vathmos_katatmisis != 0 ) 
    {
        tmimata++;
    }

    // Dimiourgia koinis mnimis
    shmid = shmget(SHM_KEY, sizeof(struct SharedMemStruct)+(max_grammi*vathmos_katatmisis+vathmos_katatmisis), 0666|IPC_CREAT);
    if( shmid == -1 ) 
    {
        printf("Provlima desmeusis koinis mnimis");
        return -1;
    }

    // Syndesi me tin koini mnimi
    shmp = shmat(shmid, NULL, 0);
    if (shmp == (void *) -1) 
    {
       printf( "Provlima syndesis me tin koini mnimi" );
       return -1;
    }
    
    // Proetoimasia koinis mnimis
    smsp = (struct SharedMemStruct*) shmp;
    sem_init(&(smsp->semRead), 1, 1);
    sem_init(&(smsp->semReplace), 1, 1);
    sem_init(&(smsp->semReplaceDone), 1, 0);
    sem_init(&(smsp->semWrite), 1, 0);
    sem_init(&(smsp->semNoReadersAtSection), 1, 0);
    smsp->aitima = 0;
    smsp->readers = 0;
    smsp->trexonTmima = 0;
    smsp->synolikesAitiseis = N*aitiseis;
    
    // dimiourgia paidiwn
    children_pids = dimiourgiaDiergasiwn(N, grammes, tmimata, vathmos_katatmisis, aitiseis, max_grammi);
    
    // anamoni gia tin trofodotisi twn tmimatwn otan xreaistei
    while (1) 
    {
        P(&(smsp->semWrite));  //mplokaroume thn gonikh diergasia mexri na ginei kapoia aithsh
            // an teleiwsan oi aitiseis, termatizei kai h goniki
            if( smsp->synolikesAitiseis == 0 )
            {
                break;
            }

            xronikiStigmi = (double)(clock()-start) / CLOCKS_PER_SEC;
            printf("-------------------------------");
            printf("Goniki: Xronos Exodou %.3lf tou tmimatos %u\n", xronikiStigmi ,smsp->trexonTmima);

            fortwsiTmimatos(file, ((char*)shmp)+sizeof(struct SharedMemStruct), smsp->aitima, vathmos_katatmisis, max_grammi);

            xronikiStigmi = (double)(clock()-start)/CLOCKS_PER_SEC;
            printf("Goniki: Xronos Eisodou %.3lf tou tmimatos %u\n", xronikiStigmi ,smsp->aitima);
            smsp->trexonTmima = smsp->aitima;
            
        V( &(smsp->semReplaceDone) );      // xypnaei tin diergasia pou perimenei to sygkekrimeno tmima
    }

    
    // aposyndesi koinis mnimis
    if (shmdt(shmp) == -1) 
    {
        printf( "Provlima aposyndesis me tin koini mnimi" );
        return -1;
    }
    
    // Anamoni na teleiwsoun oi diergasies paidia
    anamoniPaidiwn(children_pids, N);
    
    // diagrafi koinis mnimis
    sem_destroy(&(smsp->semRead));
    sem_destroy(&(smsp->semReplace));
    sem_destroy(&(smsp->semNoReadersAtSection));
    sem_destroy(&(smsp->semReplaceDone));
    sem_destroy(&(smsp->semWrite));
    
    if (shmctl(shmid, IPC_RMID, 0) == -1) 
    {
        printf("Provlima diagrafis koinis mnimis");
        return -1;
    }
    
    fclose(file);       // kleisimo tou arxeiou
    printf("Termatismos\n");
    
    return 0;
}


pid_t * dimiourgiaDiergasiwn(int N, unsigned int grammes, unsigned int tmimata, unsigned int grammesAnaTmima, unsigned int aitiseis, unsigned int maxGrammi)
{
    int i;
    pid_t *children_pids, p;
    
    // Dimiourgia pinaka me ta pid twn paidiwn
    children_pids = malloc(N*sizeof(pid_t));
    
    // Dimiourgia N paidiwn
    for (i = 0; i < N; i++) 
    {    
        p = fork ();
        if (p < 0) 
        {
            printf("Forking %d child failed\n", i+1);
        }
        else if(p == 0) 
        {
            childProcess(grammes, tmimata, grammesAnaTmima, aitiseis, maxGrammi);
            exit(0);
        }
        children_pids[i] = p;
    }
    return children_pids;
}

// Synartisi pou perimenei ta paidia na teleiwsoun
void anamoniPaidiwn(pid_t *children_pids, int N)
{  
    int i;
    for (i = 0; i < N; i++) 
    {
        wait(NULL);
    }
    // apodesmeusi pinaka
    free(children_pids);
}