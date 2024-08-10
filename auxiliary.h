#ifndef AUXILIARY_H
#define AUXILIARY_H

#include <semaphore.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

#define SHM_KEY 0x1234              // kleidi koinis mnimis

struct SharedMemStruct 
{
    sem_t semReplace;               // simaforos gia tin pithani antikatastasi tmimatos
    sem_t semNoReadersAtSection;    // simaforos gia tin enimerwsi tou reader oti efygan oi alloi readers
    sem_t semRead;                  // simaforos gia na auxanei to plithos twn readers
    sem_t semReplaceDone;           // simaforos pou mplokarei ton reader pou perimenei antikatastasi tmimatos
    sem_t semWrite;                 // simaforos gia ton goniki diergasia
    unsigned int trexonTmima;       // Trexon arithmos tmimatos
    unsigned int aitima;            // Arithmos tmimatos pou exei ginei nea aitisi
    unsigned int readers;           // Plithos twn diergasiwn-readers pou diavazoun tautoxrona to tmima
    unsigned int synolikesAitiseis; // Plithos twn aitisewn - apotelei flag termatismou
};


void statistikaArxeiou(FILE *file, unsigned int *grammes, unsigned int *max_grammi);
void fortwsiTmimatos(FILE *file, char *buffer, unsigned int aa_tmima, unsigned int grammesAnaTmima, unsigned int maxGrammi);
void eggrafiGrammis( FILE* file, char *buffer, unsigned int tyxaiaGrammi, unsigned int maxGrami );
void P(sem_t *sem);
void V(sem_t *sem);


#endif /* AUXILIARY_H */