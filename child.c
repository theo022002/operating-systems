#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>

#include "auxiliary.h"
#include "child.h"

// Diergasia pou ektelei to kathe paidi
void childProcess(unsigned int grammes, unsigned int tmimata, unsigned int vathmos_katatmisis, unsigned int aitiseis, unsigned int max_grammi)
{
    FILE *file;
    char filename[128];
    int i, shmid;
    void *shmp;
    struct SharedMemStruct *smsp;
    float pithanotita;
    unsigned int tyxaioTmima, tyxaiaGrammi;
    double xronikiStigmi;
    clock_t start = clock();
    
    // Arxikopoihsh tis gennitrias me ton xrono meion to id tis diergasias
    srand(time(NULL)-getpid());
    
    // fortnwei tin koini mnimi
    shmid = shmget(SHM_KEY, sizeof(struct SharedMemStruct)+(max_grammi*vathmos_katatmisis+vathmos_katatmisis), 0);
    if (shmid == -1) 
    {
        printf("Provlima desmeusis koinis mnimis\n");
        return ;
    }

    // Attach to the segment to get a pointer to it.
    shmp = shmat(shmid, NULL, 0);
    if (shmp == (void *) -1) 
    {
        printf( "Provlima syndesis me tin koini mnimi\n" );
        return ;
    }
    smsp = (struct SharedMemStruct *) shmp;
    
    // Dimiourgia onomatos arxeiou kai anoigma
    sprintf(filename, "%u_.txt", getpid());
    file = fopen( filename, "w");

    // dialegei neo tyxaio tmima kai tyxaia grammi
    tyxaioTmima = rand() % tmimata;
    if(tyxaioTmima < tmimata-1)
        tyxaiaGrammi = rand() % vathmos_katatmisis;
    else
        tyxaiaGrammi = rand() % (grammes%vathmos_katatmisis);
    
    for (i = 0; i < aitiseis; i++) 
    {
        // eggrafi xronou ypovolis aitimatos sto arxeio
        xronikiStigmi = (double)(clock()-start)/CLOCKS_PER_SEC;
        fprintf(file, "%.5lf ", xronikiStigmi);

        // Anagwnsi tmimatwn apo tis diergasies
        P(&(smsp->semReplace));
        P(&(smsp->semRead));
            // an zitaei antikatastasi tmimatos
            if( smsp->trexonTmima != tyxaioTmima ) 
            {
                smsp->aitima = tyxaioTmima;
                printf("Paidi %u: Aitisi gia to tmima %u\n",getpid(),tyxaioTmima);
                
                // Anamenei na teleiwsei h anagnwsi twn allws diergasiwn
                if (smsp->readers > 0) 
                {
                    V( &(smsp->semRead) );          // Afinei ton simatoforo semRead gia na fygoun oi alloi reader
                    P( &(smsp->semNoReadersAtSection) );
                    P( &(smsp->semRead) );
                }

                V( &(smsp->semWrite) );            // Xypanei tin vasiki diergasia gia na fortwsei to tmima
                P( &(smsp->semReplaceDone) );
            }
            
            printf("Paidi %u: Enarxi anagnwsis tmimatos %u kai grammis %u\n",getpid(),smsp->trexonTmima, tyxaiaGrammi);
            smsp->readers++;
        
        V( &(smsp->semRead) );
        V( &(smsp->semReplace) );
        
        // Tautoxrono diavasma xwris amoivaio apoklismo
        xronikiStigmi = (double)(clock()-start)/CLOCKS_PER_SEC;
        fprintf(file, "%.5lf <%u,%u> ", xronikiStigmi, tyxaioTmima, tyxaiaGrammi);
        eggrafiGrammis( file, ((char *)shmp)+sizeof(struct SharedMemStruct),  tyxaiaGrammi, max_grammi );
        usleep(20*1000);
        
        P(&(smsp->semRead));
            
            printf("Paidi %u: Lixi anagnwsis tmimatos %u\n",getpid(),smsp->trexonTmima);
        
            smsp->synolikesAitiseis --;
            smsp->readers--;
            
            // an itan o teleutaios reader tote xypnaei auton pou zitise antikatastasi
            if( smsp->readers == 0 && smsp->trexonTmima!=smsp->aitima ) 
            {
                V(&(smsp->semNoReadersAtSection));
            }
            //an htan h teleutaia aitisi tote eidopoiei tin goniki na termatisei
            if( smsp->synolikesAitiseis == 0 ) 
            {
                V(&(smsp->semWrite));
            }
            
        V(&(smsp->semRead));
        
        pithanotita = ((float) rand()) / RAND_MAX;
        if(pithanotita <= 0.3) 
        {
            // dialegei neo tyxaio tmima kai tyxaia grammi
            tyxaioTmima = rand() % tmimata;
            if (tyxaioTmima < tmimata-1)
                tyxaiaGrammi = rand() % vathmos_katatmisis;
            else
                tyxaiaGrammi = rand() % (grammes%vathmos_katatmisis);
        }
    }

    // aposyndesi koinis mnimis
    if (shmdt(shmp) == -1) 
    {
        printf( "%u: Provlima aposyndesis me tin koini mnimi\n", getpid() );
    }

    // Kleisimo tou arxeiou
    fclose( file );
}