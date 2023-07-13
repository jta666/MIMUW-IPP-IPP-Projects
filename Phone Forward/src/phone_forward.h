/** @file
 * Interfejs klasy przechowującej przekierowania numerów telefonicznych
 *
 * @author Marcin Peczarski <marpe@mimuw.edu.pl>
 * @author modified by Jakub Adamiak
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#ifndef __PHONE_FORWARD_H__
#define __PHONE_FORWARD_H__

#include <stdbool.h>
#include <stddef.h>
/**
 * To jest struktura przechowująca przekierowania numerów telefonów.
 */
struct PhoneForward;
/**
 * Jest zwyczajnym drzewem z wyróżnionym korzeniem,
 * którego węzeł zawiera wskaźnik na poziom niżej
 * i w prawo na tym samym poziomie, fragment numeru telefonu
 * i jego przekierowanie, a ponadto kilka pomocniczych danych,
 * udokumentowanych poniżej.
 */
typedef struct PhoneForward{
    bool root; /**< informacja, czy węzeł jest korzeniem drzewa */
    int lvl; /**< poziom węzła w drzewie; korzeń ma poziom -1 */
    int numOnLvl; /**< cyfra w numerze telefonu odpowiadająca poziomowi węzła */
    char *original; /**< oryginalny fragment numeru telefonu */
    char *redirected; /**< przekierowanie oryginalnego fragmentu */
    struct PhoneForward* nextCurr; /**< wskaźnik na kolejny węzeł w prawo */
    struct PhoneForward* nextNext; /**< wskaźnik na kolejny węzeł w dół */
}PhoneForward;

/**
 * To jest struktura przechowująca ciąg numerów telefonów.
 */
struct PhoneNumbers;
/**
 * Jest zwyczajną listą z wyróżnioną głową.
 */
typedef struct PhoneNumbers {
    char *num; /**< numer telefonu */
    struct PhoneNumbers* next;  /**< wskaźnik na następny element */
}PhoneNumbers;

/**
 * To jest pomocnicza struktura danych przechowywująca tylko
 * fragment numeru telefonu i jego przekierowanie.
 * Jest wykorzystywana tylko w funkcji phfwdGet.
 */
typedef struct Redirection {
    char *original; /**< fragment numeru telefonu */
    char *redirected; /**< przekierowanie */
}Redirection;

/** @brief Sprawdza, czy napis nie jest numerem.
 * Pomocnicza funkcja sprawdzający, czy podany napis nie reprezentuje
 * poprawnego numeru telefonu.
 * @param[in] num - wskaźnik na napis reprezentujący domniemany numer telefonu;
 * @return Wartość @p true, jeśli napis nie jest poprawnym numerem telefonu
 *         Wartość @p false w przeciwnym wypadku.
 */
bool isNotPhoneNumber(char const *num);

/** @brief Tworzy nową strukturę.
 * Tworzy nową strukturę niezawierającą żadnych przekierowań.
 * @return Wskaźnik na utworzoną strukturę lub NULL, gdy nie udało się
 *         alokować pamięci.
 */
PhoneForward *phfwdNew(void);

/** @brief Tworzy nową strukturę.
 * Tworzy nową strukturę niezawierającą żadnych numerów telefonu.
 * @return Wskaźnik na utworzoną strukturę lub NULL, gdy nie udało się
 *         alokować pamięci.
 */
PhoneNumbers *phnumNew(void);

/** @brief Tworzy nowy pełny węzeł drzewa przekierowań.
 * Tworzy nowy węzeł, w którym faktycznie jest jakieś przekierowanie.
 * @param[in] lvl - poziom drzewa, na którym znajdzie się nowy węzeł;
 * @param[in] numOnLvl - cyfra w numerze odpowiadająca poziomowi węzła;
 * @param[in] num1   - wskaźnik na napis reprezentujący prefiks numerów
 *                     przekierowywanych;
 * @param[in] num2   - wskaźnik na napis reprezentujący prefiks numerów,
 *                     na które jest wykonywane przekierowanie.
 * @return Wskaźnik na strukturę przechowującą nowy węzeł lub NULL, gdy nie
 *         udało się alokować pamięci.
 */
PhoneForward *createPhoneForward(int lvl, int numOnLvl, char const *num1, char const *num2);

/** @brief Tworzy nowy pusty węzeł drzewa przekierowań.
 * Tworzy nowy węzeł, w którym nie ma żadnego przekierowania, służy jako
 * przejściówka łącząca korzeń drzewa z dłuższymi przekierowywanymi prefiksami,
 * które mogą znajdować się na niższych poziomach.
 * @param[in] lvl - poziom drzewa, na którym znajdzie się nowy węzeł;
 * @param[in] numOnLvl - cyfra w numerze odpowiadająca poziomowi węzła;
 * @return Wskaźnik na strukturę przechowującą nowy węzeł lub NULL, gdy nie
 *         udało się alokować pamięci.
 */
PhoneForward *createConnector(int lvl, int numOnLvl);

/** @brief Wypełnia pusty węzeł drzewa przekierowań.
 * Dodaje przekierowanie do węzła, który uprzednio służył tylko jako
 * przejściówka do niższych poziomów.
 * @param[in,out] connector - wskaźnik na strukturę przechowującą pusty węzeł;
 * @param[in] num1   - wskaźnik na napis reprezentujący prefiks numerów
 *                     przekierowywanych;
 * @param[in] num2   - wskaźnik na napis reprezentujący prefiks numerów,
 *                     na które jest wykonywane przekierowanie.
 */
void fillConnector(PhoneForward *connector, char const *num1, char const *num2);

/** @brief Tworzy nowy węzeł ciagu numerów telefonu.
 * Tworzy nowy węzeł listy reprezentującej ciag numerów telefonu.
 * @param[in] num   - wskaźnik na napis reprezentujący numer telefonu;
 * @return Wskaźnik na strukturę przechowującą nowy węzeł lub NULL, gdy nie
 *         udało się alokować pamięci.
 */
PhoneNumbers *createPhoneNumber(char const *num);

/** @brief Usuwa strukturę.
 * Usuwa strukturę wskazywaną przez @p pf. Nic nie robi, jeśli wskaźnik ten ma
 * wartość NULL.
 * @param[in] pf - wskaźnik na usuwaną strukturę.
 */
void phfwdDelete(PhoneForward *pf);

/** @brief Dodaje przekierowanie.
 * Dodaje przekierowanie wszystkich numerów mających prefiks @p num1, na numery,
 * w których ten prefiks zamieniono odpowiednio na prefiks @p num2. Każdy numer
 * jest swoim własnym prefiksem. Jeśli wcześniej zostało dodane przekierowanie
 * z takim samym parametrem @p num1, to jest ono zastępowane.
 * Relacja przekierowania numerów nie jest przechodnia.
 * @param[in,out] pf - wskaźnik na strukturę przechowującą przekierowania
 *                     numerów;
 * @param[in] num1   - wskaźnik na napis reprezentujący prefiks numerów
 *                     przekierowywanych;
 * @param[in] num2   - wskaźnik na napis reprezentujący prefiks numerów,
 *                     na które jest wykonywane przekierowanie.
 * @return Wartość @p true, jeśli przekierowanie zostało dodane.
 *         Wartość @p false, jeśli wystąpił błąd, np. podany napis nie
 *         reprezentuje numeru, oba podane numery są identyczne lub nie udało
 *         się alokować pamięci.
 */
bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2);

/** @brief Dodaje numer telefonu.
 * Dodaje nowy numer teleofnu do ciągu numerów.
 * @param[in,out] pn - wskaźnik na strukturę przechowującą ciąg
 *                     numerów;
 * @param[in] num   - wskaźnik na napis reprezentujący dodawany numer;
 * @return Wartość @p true, jeśli przekierowanie zostało dodane.
 *         Wartość @p false, jeśli nie udało
 *         się alokować pamięci.
 */
bool phnumAdd(PhoneNumbers *pn, const char *num);

/** @brief Usuwa przekierowania.
 * Usuwa wszystkie przekierowania, w których parametr @p num jest prefiksem
 * parametru @p num1 użytego przy dodawaniu. Jeśli nie ma takich przekierowań
 * lub napis nie reprezentuje numeru, nic nie robi.
 * @param[in,out] pf - wskaźnik na strukturę przechowującą przekierowania
 *                     numerów;
 * @param[in] num    - wskaźnik na napis reprezentujący prefiks numerów.
 */
void phfwdRemove(PhoneForward *pf, char const *num);

/** @brief Wyznacza przekierowanie numeru.
 * Wyznacza przekierowanie podanego numeru. Szuka najdłuższego pasującego
 * prefiksu. Wynikiem jest ciąg zawierający co najwyżej jeden numer. Jeśli dany
 * numer nie został przekierowany, to wynikiem jest ciąg zawierający ten numer.
 * Jeśli podany napis nie reprezentuje numeru, wynikiem jest pusty ciąg.
 * Alokuje strukturę @p PhoneNumbers, która musi być zwolniona za pomocą
 * funkcji @ref phnumDelete.
 * @param[in] pf  - wskaźnik na strukturę przechowującą przekierowania numerów;
 * @param[in] num - wskaźnik na napis reprezentujący numer.
 * @return Wskaźnik na strukturę przechowującą ciąg numerów lub NULL, gdy nie
 *         udało się alokować pamięci.
 */
PhoneNumbers *phfwdGet(PhoneForward const *pf, char const *num);

/** @brief Wyznacza przekierowania na dany numer.
 * Wyznacza następujący ciąg numerów: jeśli istnieje numer @p x, taki, że
 * @p x da się przekierować zgodnie z dodanymi wcześniej przekierowaniami
 * na numer @p num, to numer @p x
 * należy do wyniku wywołania @ref phfwdReverse z numerem @p num. Dodatkowo ciąg
 * wynikowy zawsze zawiera też numer @p num. Wynikowe numery są posortowane
 * leksykograficznie i nie mogą się powtarzać. Jeśli podany napis nie
 * reprezentuje numeru, wynikiem jest pusty ciąg. Alokuje strukturę
 * @p PhoneNumbers, która musi być zwolniona za pomocą funkcji @ref phnumDelete.
 * @param[in] pf  - wskaźnik na strukturę przechowującą przekierowania numerów;
 * @param[in] num - wskaźnik na napis reprezentujący numer.
 * @return Wskaźnik na strukturę przechowującą ciąg numerów lub NULL, gdy nie
 *         udało się alokować pamięci.
 */
PhoneNumbers *phfwdReverse(PhoneForward const *pf, char const *num);

/** @brief Usuwa strukturę.
 * Usuwa strukturę wskazywaną przez @p pnum. Nic nie robi, jeśli wskaźnik ten ma
 * wartość NULL.
 * @param[in] pnum - wskaźnik na usuwaną strukturę.
 */
void phnumDelete(PhoneNumbers *pnum);


/** @brief Wyznacza przeciwobraz funkcji phfwdReverse.
 * dla podanej za pomocą wskaźnika pf bazy przekierowań i podanego numeru telefonu num wyznacza
 * posortowaną leksykograficznie listę wszystkich takich numerów telefonów
 * i tylko takich numerów telefonów x, że phfwdGet(x) = num.
 * Funkcja ta alokuje strukturę PhoneNumbers, która musi być zwolniona za pomocą funkcji phnumDelete.
 * Wynikiem funkcji jest NULL, gdy nie udało się alokować pamięci.
 * Jeśli podany napis num nie reprezentuje numeru, wynikiem jest pusty ciąg.
 * @param[in][out] pf - wskaźnik na strukturę przechowującą przekierowania.
 * @param[in] num - wskaźnik na numer, którego przeciwobraz phfwdReverse chcemy.
 */
PhoneNumbers *phfwdGetReverse(PhoneForward const *pf, char const *num);

/** @brief Udostępnia numer.
 * Udostępnia wskaźnik na napis reprezentujący numer. Napisy są indeksowane
 * kolejno od zera.
 * @param[in] pnum - wskaźnik na strukturę przechowującą ciąg numerów telefonów;
 * @param[in] idx  - indeks numeru telefonu.
 * @return Wskaźnik na napis reprezentujący numer telefonu. Wartość NULL, jeśli
 *         wskaźnik @p pnum ma wartość NULL lub indeks ma za dużą wartość.
 */
char const *phnumGet(PhoneNumbers const *pnum, size_t idx);

#endif /* __PHONE_FORWARD_H__ */