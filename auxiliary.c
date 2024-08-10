#include "auxiliary.h"
#include <stdlib.h>
#include <string.h>

/* Synartisi pou diavazei to arxeio
 * kai vriskei tin megisti grammi kai to plithos twn grammwn
 */
void statistikaArxeiou(FILE *file, unsigned int *grammes, unsigned int *max_grammi) 
{
    int c;
    unsigned int mikos_grammis=0;
    
    *grammes = 0;
    *max_grammi = 0;
    
    // Prwta metraei tis grammes tou areiou
    while (!feof(file)) 
    {
        c = fgetc(file);
        mikos_grammis++;
        
        if(c == '\n') {     // an einai allagi grammis
            (*grammes)++;
            
            if( mikos_grammis > *max_grammi ) {          // euresi megistou
                *max_grammi = mikos_grammis;
            }
            mikos_grammis = 0;
        }
    }
    
    // epanaferei ton deikti stin arxi tou arxeiou
    fseek(file, 0, SEEK_SET);
}



void fortwsiTmimatos(FILE *file, char *buffer, unsigned int aa_tmima, unsigned int grammesAnaTmima, unsigned int maxGrammi)
{
    char *str = malloc((maxGrammi+2)*sizeof(char));
    unsigned int i, j;
    
    // epanaferei ton deikti stin arxi tou arxeiou
    fseek(file, 0, SEEK_SET);
    
    // Metakinisi sto swsto tmima
    if(aa_tmima > 0) 
    {
        for (i = 0; (i < aa_tmima); i++) 
        {
            for (j = 0; j < grammesAnaTmima; j++) 
            {
                fgets(str, maxGrammi, file);
            }
        }
    }
    
    // Anagnwsi tou tmimatos sto buffer
    i = 0;
    buffer[0] = '\0';
    for (j = 0; j < grammesAnaTmima; j++) 
    {
        fgets(str, maxGrammi, file);
        strcat(buffer, str);
        
        // an eftase sto telos tou arxeiou stamataei
        if(feof(file))
            break;
    }
    free(str);
}

// Eggrafi grammis sto arxeio
void eggrafiGrammis(FILE* file, char *buffer, unsigned int tyxaiaGrammi, unsigned int maxGrammi)
{
    unsigned int i=0;
    unsigned int j=0;
    
    if(tyxaiaGrammi > 0)
    {
        for (i = 0; i < tyxaiaGrammi-1; i++) 
        {
            // trwei tis grammes mexri na ftasei stin zoutoumeni
            while (buffer[j] != '\n' && buffer[j] != '\0') { j++; /*putchar(buffer[j]);*/ }
            j++;
        }
    }

    // Metaferei tin grammi sto arxeio
    while (buffer[j] != '\n' && buffer[j] != '\0') 
    {
        fputc(buffer[j++], file);
    }
    fputc('\n', file);
    
}

void P(sem_t *sem)
{
    sem_wait(sem);
}

void V(sem_t *sem)
{
    sem_post(sem);
}