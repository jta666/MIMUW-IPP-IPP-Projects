/** @file
 * Implementacja klasy przechowującej przekierowania numerów telefonicznych
 *
 * @author Jakub Adamiak
 * @copyright Jakub Adamiak
 * @date 2022
 */


#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "phone_forward.h"

bool isNotPhoneNumber(char const* num) {
    if (!num) return true;

    int i = 0;
    while (num[i] != '\0') {
        if (num[i] != '*' && num[i] != '#')
            if (num[i] - 48 < 0 || num[i] - 48 > 9) return true;
        i++;
    }

    if (!i)  // przypadek pustego napisu
        return true;

    return false;
}

PhoneForward* phfwdNew(void) {
    PhoneForward* res = malloc(sizeof(PhoneForward));
    if (!res) return NULL;

    res->root = true;
    res->lvl = -1;
    res->numOnLvl = -1;
    res->original = NULL;
    res->redirected = NULL;
    res->nextCurr = NULL;
    res->nextNext = NULL;

    return res;
}

PhoneNumbers* phnumNew(void) {
    PhoneNumbers* res = malloc(sizeof(PhoneNumbers));
    if (!res) return NULL;

    res->num = NULL;
    res->next = NULL;
    return res;
}

PhoneNumbers* createPhoneNumber(char const* num) {
    PhoneNumbers* res = malloc(sizeof(PhoneNumbers));
    if (!res) return NULL;

    char* newNum = malloc(sizeof(char) * (strlen(num) + 1));
    if (!newNum) {
        free(res);
        return NULL;
    }
    strcpy(newNum, num);

    res->num = newNum;
    res->next = NULL;
    return res;
}

bool phnumAdd(PhoneNumbers* pn, const char* num) {
    PhoneNumbers* walker = pn->next;  // pn to zawsze pusta głowa;
    PhoneNumbers* walkerPrev = pn;

    while (walker) {
        walkerPrev = walker;
        walker = walker->next;
    }

    PhoneNumbers* newOne = createPhoneNumber(num);
    if (!newOne) return false;

    walkerPrev->next = newOne;
    return true;
}

PhoneForward* createPhoneForward(int lvl, int numOnLvl, char const* num1, char const* num2) {
    PhoneForward* res = malloc(sizeof(PhoneForward));
    if (!res) return NULL;
    res->root = false;
    res->lvl = lvl;
    res->numOnLvl = numOnLvl;

    char* newOriginal = malloc(sizeof(char) * (strlen(num1) + 1));
    char* newRedirected = malloc(sizeof(char) * (strlen(num2) + 1));
    if (!newOriginal || !newRedirected) {
        free(newOriginal);
        free(newRedirected);
        free(res);
        return NULL;
    }
    strcpy(newOriginal, num1);
    strcpy(newRedirected, num2);

    res->original = newOriginal;
    res->redirected = newRedirected;
    res->nextCurr = NULL;
    res->nextNext = NULL;

    return res;
}

PhoneForward* createConnector(int lvl, int numOnLvl) {
    PhoneForward* res = malloc(sizeof(PhoneForward));
    if (!res) return NULL;

    res->root = false;
    res->lvl = lvl;
    res->numOnLvl = numOnLvl;
    res->original = NULL;
    res->redirected = NULL;
    res->nextCurr = NULL;
    res->nextNext = NULL;

    return res;
}

void fillConnector(PhoneForward* connector, char const* num1, char const* num2) {
    char* newOriginal = malloc(sizeof(char) * (strlen(num1) + 1));
    char* newRedirected = malloc(sizeof(char) * (strlen(num2) + 1));
    if (!newOriginal || !newRedirected) return;

    strcpy(newOriginal, num1);
    strcpy(newRedirected, num2);
    free(connector->original);
    free(connector->redirected);

    connector->original = newOriginal;
    connector->redirected = newRedirected;
}

/**
 * Pomocnicza funkcja rekurencyjna do phfwdAdd.
 */
bool phfwdAdd_(PhoneForward* curr, PhoneForward* prev, char const* num1, char const* num2, int len) {
    /*if (isNotPhoneNumber(num1) || isNotPhoneNumber(num2))  // błąd - to nie są numery
        return false;

    if (!strcmp(num1, num2))  // błąd - oba napisy są takie same
        return false;*/

    if (!curr) {  // znaleźliśmy się na pustym poziomie
        int lvl = prev->lvl + 1;
        if ((size_t)len == (size_t)lvl + 1) {  // to jest właściwy poziom
            PhoneForward* newOne = createPhoneForward(lvl, num1[lvl], num1, num2);
            if (!newOne)
                return false;
            prev->nextNext = newOne;
            return true;
        } else {  // musimy zejść niżej, potrzebujemy łącznika
            PhoneForward* newConnector = createConnector(lvl, num1[lvl]);
            if (!newConnector) return false;
            prev->nextNext = newConnector;
            return phfwdAdd_(NULL, newConnector, num1, num2, len);
        }
    }

    if (curr)
        if (curr->root)  // schodzimy z korzenia
            return phfwdAdd_(curr->nextNext, curr, num1, num2, len);

    int lvl = prev->lvl + 1;

    // poszukujemy węzła z odpowiednią cyfrą w numerze
    bool foundBranch = false;
    PhoneForward* walker = curr;
    PhoneForward* walkerPrev = NULL;
    while (walker) {
        if (walker->numOnLvl == num1[lvl]) {
            foundBranch = true;
            break;
        }
        walkerPrev = walker;
        walker = walker->nextCurr;
    }

    if (foundBranch) {
        // znaleźliśmy pusty łącznik, któy pasuje do numeru num1
        if (!walker->original && (size_t)len == (size_t)lvl + 1) {
            fillConnector(walker, num1, num2);
            return true;
        }
        // znaleźliśmy dodane wcześniej przekierowanie numeru num1
        if (walker->original && !strcmp(walker->original, num1)) {
            fillConnector(walker, num1, num2);
            return true;
        }
        return phfwdAdd_(walker->nextNext, walker, num1, num2, len);
    } else {
        // jesteśmy na dobrym poziomie dla numeru num1
        if (strlen(num1) == (size_t)lvl + 1) {
            PhoneForward* newOne = createPhoneForward(lvl, num1[lvl], num1, num2);
            if (!newOne) return false;
            walkerPrev->nextCurr = newOne;
            return true;
        } else {  // jesteśmy za wysoko dla numeru num1
            PhoneForward* newConnector = createConnector(lvl, num1[lvl]);
            if (!newConnector) return false;
            walkerPrev->nextCurr = newConnector;
            return phfwdAdd_(newConnector->nextNext, newConnector, num1, num2, len);
        }
    }

    return true;
}

bool phfwdAdd(PhoneForward* curr, char const* num1, char const* num2) {
    if (!curr) return false;

    if (isNotPhoneNumber(num1) || isNotPhoneNumber(num2))  // błąd - to nie są numery
        return false;

    if (!strcmp(num1, num2))  // błąd - oba napisy są takie same
        return false;

    return phfwdAdd_(curr, NULL, num1, num2, strlen(num1));
}

void phfwdDelete(PhoneForward* pf) {
    if (!pf) return;

    phfwdDelete(pf->nextNext);
    phfwdDelete(pf->nextCurr);
    free(pf->original);
    free(pf->redirected);
    free(pf);
    pf = NULL;
}

void phnumDelete(PhoneNumbers* pnum) {
    if (!pnum) return;

    phnumDelete(pnum->next);
    free(pnum->num);
    free(pnum);
}

/**
 * Pomocnicza funkcja rekurencyjna do phfwdRemove.
 */
void phfwdRemove_(PhoneForward* curr, PhoneForward* prev, char const* num) {
    if (!curr || isNotPhoneNumber(num))  // błąd - nie ma struktury lub num nie jest numerem
        return;

    if (curr)
        if (curr->root)  // schodzimy z korzenia
            phfwdRemove_(curr->nextNext, curr, num);

    // poszukujemy węzła z odpowiednią cyfrą w numerze
    bool foundBranch = false;
    PhoneForward* walker = curr;
    PhoneForward* walkerPrev = NULL;
    while (walker && walker->lvl >= 0) {
        if (walker->numOnLvl == num[walker->lvl]) {
            foundBranch = true;
            break;
        }
        walkerPrev = walker;
        walker = walker->nextCurr;
    }

    if (!foundBranch)  // nie ma przekierowań z prefiksem num
        return;

    if (strlen(num) > (size_t)curr->lvl + 1)
        // jesteśmy za wysoko dla prefiksu num
        phfwdRemove_(walker->nextNext, walker, num);

    // znaleźliśmy węzeł, rozłączamy go od drzewa i usuwamy
    if (strlen(num) == (size_t)curr->lvl + 1) {
        if (walker == prev->nextNext) {
            prev->nextNext = walker->nextCurr;
            walker->nextCurr = NULL;
            phfwdDelete(walker);
        } else {
            walkerPrev->nextCurr = walker->nextCurr;
            walker->nextCurr = NULL;
            phfwdDelete(walker);
        }
    }
}

void phfwdRemove(PhoneForward* pf, char const* num) {
    phfwdRemove_(pf, NULL, num);
}

/**
 * Pomocnicza funkcja rekurencyjna do phfwdGet.
 */
PhoneNumbers* phfwdGet_(PhoneForward const* pf, char const* num, Redirection* best) {
    if (pf)
        if (pf->root)  // schodzimy z korzenia
            return phfwdGet_(pf->nextNext, num, best);

    // poszukujemy węzła z odpowiednią cyfrą w numerze
    bool foundBranch = false;
    PhoneForward const* walker = pf;
    while (walker) {
        if (walker->numOnLvl == num[walker->lvl]) {
            foundBranch = true;
            break;
        }
        walker = walker->nextCurr;
    }

    // to się na pewno w końcu wydarzy
    if (!foundBranch) {
        PhoneNumbers* res = phnumNew();
        if (!res) return NULL;
        if (best->redirected) {  // znaleźliśmy przekierowanie
            int orgLen = strlen(best->original);
            int redLen = strlen(best->redirected);
            int numLen = strlen(num);
            int newLen = numLen - orgLen + redLen;

            char* newNum = malloc((newLen + 1) * sizeof(char));
            if (!newNum) {
                free(res);
                return NULL;
            }
            int k = 0;
            for (int i = 0; i < redLen; i++) newNum[k++] = best->redirected[i];
            for (int i = orgLen; i < numLen; i++) newNum[k++] = num[i];
            newNum[k] = '\0';

            bool success = true;
            if (!phnumAdd(res, newNum)) {
                phnumDelete(res);
                free(newNum);
                return false;
            }
            free(newNum);
            if (!success) return NULL;
        } else {  // nie znaleźliśmy przekierowania
            if (!phnumAdd(res, num)) return NULL;
        }
        return res;
    }

    if (walker->original) {  // we check if there is a redirection here
        best->original = walker->original;
        best->redirected = walker->redirected;
    }

    return phfwdGet_(walker->nextNext, num, best);
}

PhoneNumbers* phfwdGet(PhoneForward const* pf, char const* num) {
    if (!pf)  // błąd - nie ma struktury
        return NULL;

    if (isNotPhoneNumber(num)) return phnumNew();

    Redirection best = {NULL, NULL};
    Redirection* bestPtr = &best;
    return phfwdGet_(pf, num, bestPtr);
}

char const* phnumGet(PhoneNumbers const* pnum, size_t idx) {
    if (!pnum) return NULL;

    PhoneNumbers const* walker = pnum->next;  // pnum to zawsze pusta głowa

    size_t i = 0;
    while (walker && i < idx) {
        walker = walker->next;
        i++;
    }

    if (i != idx)  // błąd - indeks był za duży
        return NULL;
    else if (walker)
        return walker->num;
    else
        return NULL;
}

size_t more(size_t n) {
    return (3 * n / 2 + 1);
}

/**
 * Pomocnicza funkcja rekurencyjna do phfwdReverse.
 */
void phfwdReverse_(PhoneForward const* curr, char const* toCheck, char*** silo,
                   int* currSize, int* maxSize, char const* num) {
    if (!curr)
        return;

    if (curr->nextCurr)
        phfwdReverse_(curr->nextCurr, toCheck, silo, currSize, maxSize, num);
    if (curr->nextNext)
        phfwdReverse_(curr->nextNext, toCheck, silo, currSize, maxSize, num);

    if (!silo)
        return;

    if (curr->redirected && !strcmp(curr->redirected, toCheck)) { // strcmp może dziwnie działać w tym miejscu
        int orgLen = strlen(curr->original);
        int redLen = strlen(curr->redirected);
        int numLen = strlen(num);
        int newLen = numLen - redLen + orgLen;

        char* newNum = malloc((newLen + 1) * sizeof(char));
        if (!newNum) {
            free(newNum);
            for (int i = 0; i < *currSize; i++) {
                free((*silo)[i]);
                (*silo)[i] = NULL;
            }
            free(*silo);
            *silo = NULL;
            return;
        }
        int k = 0;
        for (int i = 0; i < orgLen; i++)
            newNum[k++] = curr->original[i];
        for (int i = redLen; i < numLen; i++)
            newNum[k++] = num[i];
        newNum[k] = '\0';

        if (*currSize >= *maxSize) {
            int upgradedSize = more(*maxSize);
            char** temp = realloc((*silo), sizeof(char*) * upgradedSize);
            if (!temp) {
                for (int i = 0; i < *currSize; i++) {
                    free((*silo)[i]);
                    (*silo)[i] = NULL;
                }
                free(*silo);
                *silo = NULL;
                return;
            }
            *silo = temp;
            *maxSize = upgradedSize;
        }

        bool found = false;

        for (int i = 0; i < *currSize; i++) {
            char* x = (*silo)[i];
            if (!strcmp(newNum, (*silo)[i]))
                found = true;
        }

        if (!found) {
            (*silo)[*currSize] = newNum;
            *currSize = *currSize + 1;
        } else
            free(newNum);
    }

}

/**
 * Pomocnicza funkcja do porównywania numerów w funkcji qsort.
 */
int cmpStr(void const *a, void const *b) {
    //char const *aa = (char const *)a;
    //char const *bb = (char const *)b;

    //return strcmp(aa, bb);
    return strcmp(*((char**) a), *((char**) b));
}


PhoneNumbers* phfwdReverse(PhoneForward const* pf, char const* num) {
    if (!pf) return NULL; // do zmiany

    if (isNotPhoneNumber(num))
        return phnumNew();

    int cl = 0;
    int* currLen = &cl;
    int ml = 10;
    int* maxLen = &ml;
    char** numSilo = malloc(sizeof(char*) * *maxLen);
    if (!numSilo)
        return NULL;

    int numLen = strlen(num);

    for (int i = 0; i < numLen; i++) {
        char* prefix = malloc(sizeof(char) * (i + 2));
        if (!prefix) {
            for (int j = 0; j < *currLen; j++) {
                free(numSilo[j]);
                numSilo[j] = NULL;
            }
            free(numSilo);
            numSilo = NULL;
            return NULL;
        }
        prefix[i + 1] = '\0';
        for (int j = 0; j <= i; j++)
            prefix[j] = num[j];
        phfwdReverse_(pf, prefix, &numSilo, currLen, maxLen, num);
        if (!numSilo) {
            free(prefix);
            return NULL;
        }
        free(prefix);
    }

    if (*currLen >= *maxLen) { // jeśli nie ma miejsca, to powiększamy tablicę
        char** temp = realloc(numSilo, sizeof(char*) * (*maxLen + 1));
        if (temp) {
            numSilo = temp;
            *maxLen = *maxLen + 1;
        } else {
            for (int j = 0; j < *currLen; j++) {
                free(numSilo[j]);
                numSilo[j] = NULL;
            }
            free(numSilo);
            numSilo = NULL;
            return NULL;
        }
    }

    char* newNum = malloc(sizeof(char) * (numLen + 1));
    if (!newNum) {
        for (int j = 0; j < *currLen; j++) {
            free(numSilo[j]);
            numSilo[j] = NULL;
        }
        free(numSilo);
        numSilo = NULL;
        return NULL;
    }
    strcpy(newNum, num);

    if (numSilo) {
        numSilo[*currLen] = newNum;
        *currLen = *currLen + 1;
    }

    if (numSilo)
        qsort(numSilo, *currLen, sizeof(*numSilo), cmpStr);

    PhoneNumbers* res = phnumNew();
    if (!res) {
        for (int j = 0; j < *currLen; j++) {
            free(numSilo[j]);
            numSilo[j] = NULL;
        }
        free(numSilo);
        numSilo = NULL;
        return NULL;
    }

    if (numSilo)
    for (int i = 0; i < *currLen; i++) {
        if (!phnumAdd(res, numSilo[i])) {
            for (int j = 0; j < *currLen; j++) {
                free(numSilo[j]);
                numSilo[j] = NULL;
            }
            free(numSilo);
            numSilo = NULL;
            phnumDelete(res);
            return NULL;
        }
    }

    if (numSilo) {
        for (int i = 0; i < *currLen; i++)
            free(numSilo[i]);
        free(numSilo);
        //free(newNum);
        //char x = numSilo[*currLen - 1];
    }

    return res;
}

bool contains(PhoneNumbers* pnum, char const* num) {
    if (!pnum)
        return false;

    PhoneNumbers const* walker = pnum->next;
    while(walker) {
        if (!strcmp(walker->num, num))
            return true;
        walker = walker->next;
    }

    return false;
}

PhoneNumbers* phfwdGetReverse(PhoneForward const* pf, char const* num) {
    PhoneNumbers* pnum = phfwdReverse(pf, num);
    if (!pnum) return NULL;

    PhoneNumbers* walkerPrev = pnum;
    PhoneNumbers const* walker = pnum->next;  // pnum to zawsze pusta głowa

    while (walker) {
        if (!contains(phfwdGet(pf, walker->num), num)) {
            walkerPrev->next = walker->next;
            free(walker->num);
            free(walker);
        }
        walker = walker->next;
    }

    return pnum;
}