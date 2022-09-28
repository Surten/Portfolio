#include <stdio.h>
#include <string.h>

int main() {
    int pocet = 0, cislo = 0, x = 0;
    int zbytek = 0, gem = 0, gene = 0, delitele[333];
    char pole[1000000];
    int prvocislo[100];
    char pomoc[100];
    for (int i = 1; i < 1000000; ++i) {
        pole[i] = 1;
    }
    for (int k1 = 0; k1 < 100; ++k1) {
        prvocislo[k1] = 0;
    }
    pole[0] = 0;
    pole[1] = 0;
    for (int j = 2; j < 1000000; ++j) {
        if (pole[j] == 1) {
            for (int k = 2 * j; k < 1000000; k += j) {
                pole[k] = 0;
            }
        }
    }
    int i;
    while(1) {
        x = 0;
        gem = 0;
        scanf("%s", pomoc);
        i = strlen(pomoc);
        if(strlen(pomoc) == 1 && pomoc[0] == '0'){
            break;
        }
        for (int n = 0; n < i; ++n) {
            if (pomoc[n] == '1') {
                prvocislo[n] = 1;
            }
            if (pomoc[n] == '2') {
                prvocislo[n] = 2;
            }
            if (pomoc[n] == '3') {
                prvocislo[n] = 3;
            }
            if (pomoc[n] == '4') {
                prvocislo[n] = 4;
            }
            if (pomoc[n] == '5') {
                prvocislo[n] = 5;
            }
            if (pomoc[n] == '6') {
                prvocislo[n] = 6;
            }
            if (pomoc[n] == '7') {
                prvocislo[n] = 7;
            }
            if (pomoc[n] == '8') {
                prvocislo[n] = 8;
            }
            if (pomoc[n] == '9') {
                prvocislo[n] = 9;
            }
            if (pomoc[n] == '0') {
                prvocislo[n] = 0;
            }
            if (pomoc[n] != '0' && pomoc[n] != '1' && pomoc[n] != '2' && pomoc[n] != '3' && pomoc[n] != '4' && pomoc[n] != '5' && pomoc[n] != '6' && pomoc[n] != '7' && pomoc[n] != '8' && pomoc[n] != '9') {
                fprintf(stderr, "Error: Chybny vstup!\n");
                return 100;
            }
        }
        if(i>6){x = 1000000;}
        else {
            x = prvocislo[5] + 10 * prvocislo[4] + 100 * prvocislo[3] + 1000 * prvocislo[2] +
                10000 * prvocislo[1] + 100000 * prvocislo[0];
        }
        int vysledek[i];
        for (int j1 = 0; j1 < 333; ++j1) {
            delitele[j1] = 0;
        }
        printf("Prvociselny rozklad cisla %s je:\n", pomoc);
        if(prvocislo[0] == 1 && i == 1){
            printf("1\n");
            continue;
        }
        for (int m = 0; m < 100; ++m) {
            for (int l = 0; l < x+2; ++l) {
                if (pole[l] == 1) {
                    zbytek = 0;
                    for (int j = 0; j < i; ++j) {
                        vysledek[j] = 0;
                    }
                    for (int ii = 0; ii < i; ++ii) {
                        zbytek = (zbytek * 10) + prvocislo[ii];
                        if (zbytek >= l) {
                            vysledek[ii] = zbytek / l;
                            zbytek = zbytek % l;
                        }
                    }
                    if (zbytek == 0) {
                        delitele[gene] = l;
                        for (int j = 0; j < i; ++j) {
                            prvocislo[j] = vysledek[j];
                        }
                        gene++;
                    }
                }
            }
        }
        for (int i1 = 0; i1 < 333; ++i1) {
            if (delitele[i1] != 0) {
                cislo = delitele[i1];
                for (int j = i1; j < 333; ++j) {
                    if (delitele[j] == cislo) {
                        pocet++;
                        delitele[j] = 0;
                    }
                }
                if (pocet == 1) {
                    if (gem == 0) {
                        printf("%d", cislo);
                        gem = 1;
                    } else {
                        printf(" x %d", cislo);
                    }
                } else {
                    if (gem == 0) {
                        gem = 1;
                        printf("%d^%d", cislo, pocet);
                    } else {
                        printf(" x %d^%d", cislo, pocet);
                    }
                }
                delitele[i1] = 0;
            }
            pocet = 0;
            cislo = 0;
        }
        printf("\n");
    }
}
