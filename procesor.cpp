#include <iostream>
#include <cmath>

// Definicje stałych określających limity pamięciowe interpretera
#define MAX_INS_SIZE 20000   // Maksymalny rozmiar ciągu instrukcji wejściowych
#define MAX_LIST_SIZE 32800  // Maksymalny rozmiar pojedynczego ciągu znaków na stosie

using namespace std;

// Struktura węzła stosu - implementacja listy jednokierunkowej
struct Node {
    char* lista;    // Dynamiczna tablica przechowująca dane (np. liczby jako stringi)
    Node* next;     // Wskaźnik na kolejny element stosu
};  

char wejscie[MAX_INS_SIZE];
// Object Pooling: Wskaźnik na listę wolnych węzłów, aby uniknąć częstego new/delete
Node* pula_wolnych_nodeow = nullptr;

// Rekurencyjne obliczanie długości ciągu znaków (odpowiednik strlen)
int strlen_rekurencyjnie(char* lista, int index) {
    if(lista[index] == '\0') return 0;
    return strlen_rekurencyjnie(lista, index + 1) + 1;
}

// Rekurencyjne kopiowanie ciągu znaków (odpowiednik strcpy)
void strcpy_rekurencyjnie(char*& dir, char* src, int index) {
    dir[index] = src[index];
    if (src[index] == '\0') return;
    strcpy_rekurencyjnie(dir, src, index+1);
}

// Rekurencyjne czyszczenie (zerowanie) tablicy znaków
void wyczysc(char* tekst, int i, int index) {
    if (i >= index) return;
    tekst[i] = '\0';
    wyczysc(tekst, i + 1, index);
}

// Konwersja liczby zapisanej jako string na typ int (używane przy skokach i indeksowaniu)
void konwertowanie_inta(Node* stos, int* index, int dlugosc) {
    if(stos->lista[dlugosc] == '\0') return;
    konwertowanie_inta(stos, index, dlugosc + 1);
    int mnoznik = 1;
    if(dlugosc != 0) mnoznik = pow(10, dlugosc);
    *index += (stos->lista[dlugosc] - '0') * mnoznik;
}

// Przeszukiwanie stosu w celu znalezienia elementu na konkretnej głębokości
Node* znajdz_pozycje(Node* stos, int index) {
    if (index == 0) return stos;
    return znajdz_pozycje(stos->next, index - 1);
}

// Funkcja pomocnicza usuwająca zera wiodące w reprezentacji liczbowej
void usuniecie_zer(Node*& stos, int index, bool* flaga_do_znaczacych, int* dlugosc) {
    if(stos->lista[index] == '\0') return;
    usuniecie_zer(stos, index + 1, flaga_do_znaczacych, dlugosc);
    if(stos->lista[index] == '0' && *flaga_do_znaczacych == false) {
        stos->lista[index] = '\0';
        (*dlugosc)--;
    }
    else if(stos->lista[index] != '0') *flaga_do_znaczacych = true;
}

// Normalizacja liczb: usuwanie zbędnych znaków minus i zer
void usuwanie_zer_minusow(Node*& stos, int* dlugosc_a, int* dlugosc_b) {
    if(*dlugosc_a > 0 && stos->lista[*dlugosc_a] == '-') {
        stos->lista[*dlugosc_a] = '\0';
        (*dlugosc_a) -= 1;
    }
    if(*dlugosc_b > 0 && stos->next->lista[*dlugosc_b] == '-') {
        stos->next->lista[*dlugosc_b] = '\0';
        (*dlugosc_b) -= 1;
    }
    int index = 0;
    bool flaga = false;
    usuniecie_zer(stos, index, &flaga, dlugosc_a);
    flaga = false;
    usuniecie_zer(stos->next, index, &flaga, dlugosc_b);
}
   
// Instrukcja [']: Dodanie nowego elementu na szczyt stosu (z użyciem puli węzłów)
void obsluga_apostrofu(Node*& stos) {
    Node* top;
    if (pula_wolnych_nodeow != nullptr) {
        top = pula_wolnych_nodeow;
        pula_wolnych_nodeow = pula_wolnych_nodeow->next;
    } else {
        top = new Node;
        top->lista = new char[MAX_LIST_SIZE];
    }
    top->lista[0] = '\0';
    top->next = stos;
    stos = top;
}

// Instrukcja [,]: Usunięcie elementu ze szczytu stosu i oddanie węzła do puli
void obsluga_przecinka(Node*& stos) {
    if (stos == nullptr) return;
    Node* do_usuniecia = stos;
    stos = stos->next;
    do_usuniecia->next = pula_wolnych_nodeow;
    pula_wolnych_nodeow = do_usuniecia;
}

// Instrukcja [:]: Duplikacja elementu na szczycie stosu
void obsluga_dwukropka(Node*& stos) {
    Node* top;
    if (pula_wolnych_nodeow != nullptr) {
        top = pula_wolnych_nodeow;
        pula_wolnych_nodeow = pula_wolnych_nodeow->next;
    } else {
        top = new Node;
        top->lista = new char[MAX_LIST_SIZE];
    }
    top->lista[0] = '\0';
    strcpy_rekurencyjnie(top->lista, stos->lista, 0);
    top->next = stos;
    stos = top;
}

// Instrukcja [;]: Zamiana dwóch górnych elementów stosu (Swap)
void obsluga_srednika(Node*& stos) {
    char* temp = stos->next->lista;
    stos->next->lista = stos->lista;
    stos->lista = temp;
}

// Instrukcja [@]: Pobranie kopii elementu z głębi stosu na szczyt
void obsluga_at(Node*& stos) {
    int index = 0;
    konwertowanie_inta(stos, &index, 0);
    obsluga_przecinka(stos);
    Node* cel = znajdz_pozycje(stos, index);
    obsluga_apostrofu(stos);
    strcpy_rekurencyjnie(stos->lista, cel->lista, 0);
}

// Instrukcja [&]: Debugowanie - wypisanie całej zawartości stosu
void obsluga_amperstand(Node*& stos, int index) {
    if(stos == nullptr) return;
    obsluga_amperstand(stos->next, index + 1);
    cout << index << ": " << stos->lista << endl;
}

// Instrukcja [>]: Wypisanie pierwszego znaku ze szczytu stosu
void obsluga_wieksze(Node*& stos) {
    cout << stos->lista[0];
    obsluga_przecinka(stos);
}

// Instrukcja [-]: Zmiana znaku liczby na ujemny lub usuwanie minusa
void obsluga_minusa(Node*& stos, int index, bool* flaga) {
    if(stos->lista[index] == '\0'){
        if(index == 0){
            stos->lista[0] = '-';
            stos->lista[1] = '\0';
            return;
        }
        return;
    }
    obsluga_minusa(stos, index + 1, flaga);
    if(*flaga) return;
    if(stos->lista[index] == '-') {
        stos->lista[index] = '\0';
        *flaga = true;
    }
    if (stos->lista[index] != '-' && stos->lista[index+1] == '\0') {
        stos->lista[index+1] = '-';
        stos->lista[index+2] = '\0';
        *flaga = true;
    }
}

// Instrukcja [^]: Usuwanie znaku minus (wartość bezwzględna / unarny plus)
void obsluga_daszka(Node*& stos, int index, bool* flaga) {
    if(stos->lista[index] == '\0') return;
    obsluga_daszka(stos, index + 1, flaga);
    if(stos->lista[index] == '-') {
        stos->lista[index] = '\0';
        *flaga = true;
    }
}

// Konwersja liczby całkowitej na reprezentację ASCII (string)
void znalezienie_ASCII(int liczba, Node*& stos, int i) {
    if (liczba == 0 && i == 0) {
        stos->lista[0] = '0';
        stos->lista[1] = '\0';
        return;
    }
    if (liczba == 0) {
        stos->lista[i] = '\0';
        return;
    }
    znalezienie_ASCII(liczba/10, stos, i + 1);
    stos->lista[i] = (liczba % 10) + '0';
}

// Instrukcja [[]: Zamiana znaku na jego kod ASCII
void obsluga_kwadratu_prawy(Node*& stos) {
    int n = stos->lista[0];
    obsluga_przecinka(stos);
    obsluga_apostrofu(stos);
    znalezienie_ASCII(n, stos, 0);
}

// Instrukcja []]: Zamiana kodu ASCII na znak
void obsluga_kwadratu_lewy(Node*& stos) {
    int index = 0;
    konwertowanie_inta(stos, &index, 0);
    wyczysc(stos->lista, 0, MAX_LIST_SIZE);
    char znak_do_umieszczenia = index;
    stos->lista[0] = znak_do_umieszczenia;
    stos->lista[1] = '\0';
}

// Przesunięcie tablicy znaków w celu manipulacji stringami
void zmiana_listy(Node*& stos, int index) {
    if(index == 0) return;
    zmiana_listy(stos, index - 1);
    stos->lista[index - 1] = stos->lista[index];
}

// Instrukcja [$]: Wycięcie pierwszego znaku i pchnięcie go jako nowej listy
void obsluga_dolara(Node*& stos) {
    char znak_do_nowej_listy = stos->lista[0];
    int dlugosc = strlen_rekurencyjnie(stos->lista, 0) + 1;
    zmiana_listy(stos, dlugosc);
    obsluga_apostrofu(stos);
    stos->lista[0] = znak_do_nowej_listy;
    stos->lista[1] = '\0';
}

// Funkcja pomocnicza do łączenia dwóch list znaków
void dodanie_do_listy(char*& dir, int index, int dlugosc, char* src) {
    int nowy_index = dlugosc + index;
    dir[nowy_index] = src[index];
    if (src[index] == '\0') return;
    dodanie_do_listy(dir, index + 1, dlugosc, src);
}

// Instrukcja [#]: Konkatenacja (łączenie) dwóch list znaków
void obsluga_hasza(Node*& stos) {
    char* tekst = new char[MAX_LIST_SIZE];
    wyczysc(tekst, 0, MAX_LIST_SIZE);
    strcpy_rekurencyjnie(tekst, stos->lista, 0);
    obsluga_przecinka(stos);
    int dlugosc = strlen_rekurencyjnie(stos->lista, 0);
    dodanie_do_listy(stos->lista, 0, dlugosc, tekst);
    delete[] tekst;
}

// Porównywanie dwóch liczb zapisanych jako stringi (leksykograficznie + długość)
bool porownaj(Node* a, Node* b, int index, bool* flaga) {
    if (a->lista[index] == '\0') return false;
    bool wynik = porownaj(a, b, index + 1, flaga);
    if(*flaga == false) {
        if (a->lista[index] > b->lista[index]) {
            *flaga = true;
            return true;
        }
        if (a->lista[index] < b->lista[index]) {
            *flaga = true;
            return false;
        }
    }
    return wynik;
}

// Instrukcja [<]: Porównanie czy a < b (zwraca '1' lub '0')
void obsluga_mniejsze(Node*& stos) {
    bool czy_mniejsze = true;
    int dlugosc_a = strlen_rekurencyjnie(stos->lista, 0) - 1;
    int dlugosc_b = strlen_rekurencyjnie(stos->next->lista, 0) - 1;
    bool minus_a = false, minus_b = false;
    if (stos->lista[dlugosc_a] == '-') minus_a = true;
    if (stos->next->lista[dlugosc_b] == '-') minus_b = true;
    usuwanie_zer_minusow(stos, &dlugosc_a, &dlugosc_b);
    if ((minus_a && !minus_b) || (stos->lista[0] == '\0' && stos->next->lista[0] == '\0')) czy_mniejsze = false;
    bool flaga = false;
    if(!minus_a && !minus_b) {
        if(dlugosc_b > dlugosc_a) czy_mniejsze = false;
        if(dlugosc_a == dlugosc_b) czy_mniejsze = porownaj(stos, stos->next, 0, &flaga);
    }
    else if(minus_a && minus_b) {
        if(dlugosc_b < dlugosc_a) czy_mniejsze = false;
        if(dlugosc_a == dlugosc_b) czy_mniejsze = porownaj(stos->next, stos, 0, &flaga);
    }
    obsluga_przecinka(stos);
    obsluga_przecinka(stos);
    obsluga_apostrofu(stos);
    stos->lista[0] = czy_mniejsze ? '1' : '0';
    stos->lista[1] = '\0';
}

// Sprawdzanie czy dwa stringi na stosie są identyczne
void sprawdz_czy_rowne(Node*& stos, bool* flaga, int index) {
    if(stos->lista[index] == '\0') return;
    if(stos->next->lista[index] != stos->lista[index]) *flaga = false;
    if(*flaga == false) return;
    sprawdz_czy_rowne(stos, flaga, index + 1);
}

// Instrukcja [=]: Porównanie czy a == b
void obsluga_rowna_sie(Node*& stos) {
    bool czy_rowne = true;
    bool czy_puste = false;
    if(stos->lista[0] =='\0' || stos->next->lista[0] == '\0') {
        if((stos->lista[0] == '\0' && stos->next->lista[0] != '\0') || (stos->lista[0] != '\0' && stos->next->lista[0] == '\0')) {
            czy_rowne = false;
        }
        czy_puste = true;
    }
    if(czy_puste == false) {
        int dlugosc_a = strlen_rekurencyjnie(stos->lista, 0) - 1;
        int dlugosc_b = strlen_rekurencyjnie(stos->next->lista, 0) - 1;
        bool minus_a = false, minus_b = false;
        if (stos->lista[dlugosc_a] == '-') minus_a = true;
        if (stos->next->lista[dlugosc_b] == '-') minus_b = true;
        usuwanie_zer_minusow(stos, &dlugosc_a, &dlugosc_b);
        if(dlugosc_a == dlugosc_b && dlugosc_a > 0 && dlugosc_b > 0 && minus_a == minus_b) {
            sprawdz_czy_rowne(stos, &czy_rowne, 0);
        }
        else czy_rowne = false;
        if(stos->lista[0] == '\0' && stos->next->lista[0] == '\0') czy_rowne = true;
    }
    obsluga_przecinka(stos);
    obsluga_przecinka(stos);
    obsluga_apostrofu(stos);
    stos->lista[0] = czy_rowne ? '1' : '0';
    stos->lista[1] = '\0';
}

// Instrukcja [!]: Logiczna negacja (0 zamienia na 1, wszystko inne na 0)
void obsluga_wykrzyknika(Node*& stos) {
    bool flaga_wykrzyknik = false;
    if(stos->lista[0] == '\0' || (stos->lista[0] == '0' && stos->lista[1] == '\0')) flaga_wykrzyknik = true;
    obsluga_przecinka(stos);
    obsluga_apostrofu(stos);
    stos->lista[0] = flaga_wykrzyknik ? '1' : '0';
    stos->lista[1] = '\0';
}

// Instrukcja [~]: Pobranie aktualnego indeksu instrukcji na stos
void obsluga_tyldy(Node*& stos, int wskaznik) {
    obsluga_apostrofu(stos);
    znalezienie_ASCII(wskaznik, stos, 0);
}

// Warunek dla skoku [?]: Czy wartość na stosie jest "prawdą"
bool obsluga_zapytania(Node*& stos) {
    if(stos->lista[0] == '\0' || (stos->lista[0] == '0' && stos->lista[1] == '\0')) return false;
    else return true;
}

// Pomocnicza funkcja do zapożyczania przy odejmowaniu pisemnym
void odejmij_jeden(Node*& a, int index) {
    if(a->lista[index] != '0') {
        a->lista[index]--;
        return;
    }
    a->lista[index] = '9';
    odejmij_jeden(a, index + 1);
}

// Pisemne odejmowanie dwóch liczb w reprezentacji stringowej
void odejmowanie(Node*& a, Node*& b, char* tekst, int index) {
    if(a->lista[index] == '\0') return;
    if(b->lista[index] == '\0') {
        b->lista[index] = '0';
        b->lista[index + 1] = '\0';
    }
    if(a->lista[index] <= '0') a->lista[index] = '0';
    int operacja = (a->lista[index] - '0') - (b->lista[index] - '0');
    if(operacja < 0) {
        odejmij_jeden(a, index + 1);
        operacja += 10;
        tekst[index] = operacja%10 + '0';
    }
    else {
        tekst[index] = operacja%10 + '0';
    }
    odejmowanie(a, b, tekst, index+1);
}

// Porównanie pomocnicze dla operacji dodawania
void obsluga_plusa_porownanie(Node*& stos, bool* czy_mniejsze, int dlugosc_a, int dlugosc_b) {
    bool flaga = false;
    if(dlugosc_b > dlugosc_a) *czy_mniejsze = false;
    if(dlugosc_a == dlugosc_b) *czy_mniejsze = porownaj(stos, stos->next, 0, &flaga);
}

// Pisemne dodawanie dwóch liczb (obsługuje przeniesienie 'czy_dodac_jeden')
void dodawanie(Node*& a, Node*& b, int czy_minus, int index, char* tekst, int czy_dodac_jeden) {
    if(a->lista[index] == '\0') {
        if(czy_dodac_jeden == 1) {
            tekst[index] = '1';
            index++;
        }
        if(czy_minus == 1) {
            tekst[index] = '-';
            tekst[index+ 1] = '\0';
            return;
        }
        tekst[index] = '\0';
        return;
    }
    if(b->lista[index] == '\0') {
        b->lista[index] = '0';
        b->lista[index + 1] = '\0';
    }
    int operacja = (a->lista[index] - '0') + (b->lista[index] - '0') + czy_dodac_jeden;
    if(operacja> 9) {
        tekst[index] = (operacja%10) + '0';
        czy_dodac_jeden = 1;
    }
    else {
        tekst[index] = (operacja) + '0';
        czy_dodac_jeden = 0;
    }
    dodawanie(a, b, czy_minus, index + 1, tekst, czy_dodac_jeden);
}

// Logika wyboru między dodawaniem a odejmowaniem w zależności od znaków
void warunki_odejmowanie(Node*& stos, char* tekst, bool czy_mniejsze, bool minus_b) {
    if(czy_mniejsze == true) {
        if(minus_b) odejmowanie(stos, stos->next, tekst, 0);
        else {
            odejmowanie(stos, stos->next, tekst, 0);
            int dlugosc_tekst = strlen_rekurencyjnie(tekst, 0);
            tekst[dlugosc_tekst] = '-';
            tekst[dlugosc_tekst + 1] = '\0';
        }
    }
    else {
        if(minus_b) {
            odejmowanie(stos->next, stos, tekst, 0);
            int dlugosc_tekst = strlen_rekurencyjnie(tekst, 0);
            tekst[dlugosc_tekst] = '-';
            tekst[dlugosc_tekst + 1] = '\0';
        }
        else odejmowanie(stos->next, stos, tekst, 0);
    }
    obsluga_przecinka(stos);
    obsluga_przecinka(stos);
    obsluga_apostrofu(stos);
    strcpy_rekurencyjnie(stos->lista, tekst, 0);
}

// Instrukcja [+]: Arytmetyka dowolnej precyzji (dodawanie liczb jako stringi)
void obsluga_plusa(Node*& stos) {
    char* tekst = new char[1000]; // Tymczasowy bufor na wynik
    wyczysc(tekst, 0, 1000);
    int dlugosc_a = strlen_rekurencyjnie(stos->lista, 0) - 1;
    int dlugosc_b = strlen_rekurencyjnie(stos->next->lista, 0) - 1;
    bool minus_a = false, minus_b = false;
    if (stos->lista[dlugosc_a] == '-') minus_a = true;
    if (stos->next->lista[dlugosc_b] == '-') minus_b = true;
    usuwanie_zer_minusow(stos, &dlugosc_a, &dlugosc_b);
    bool czy_mniejsze = true;
    obsluga_plusa_porownanie(stos, &czy_mniejsze, dlugosc_a, dlugosc_b);

    if((!minus_a && !minus_b) || (minus_a && minus_b)) {
        if(!minus_a && !minus_b) {
            if(czy_mniejsze == true) dodawanie(stos, stos->next, 0, 0, tekst, 0);
            else dodawanie(stos->next, stos, 0, 0, tekst, 0);
        }
        else {
            if(czy_mniejsze == true) dodawanie(stos, stos->next, 1, 0, tekst, 0);
            else dodawanie(stos->next, stos, 1, 0, tekst, 0);
        }
        obsluga_przecinka(stos);
        obsluga_przecinka(stos);
        obsluga_apostrofu(stos);
        strcpy_rekurencyjnie(stos->lista, tekst, 0);
    }
    else {
        warunki_odejmowanie(stos, tekst, czy_mniejsze, minus_b);
    }
    delete[] tekst;
    // Normalizacja wyniku (zera wiodące i znak)
    int nowa_dlugosc_stosu = strlen_rekurencyjnie(stos->lista, 0) - 1;
    bool minus_nowy_a = false;
    if(nowa_dlugosc_stosu > 0 && stos->lista[nowa_dlugosc_stosu] == '-') {
        minus_nowy_a = true;
        stos->lista[nowa_dlugosc_stosu] = '\0';
        (nowa_dlugosc_stosu) -= 1;
    }
    bool flaga = false;
    usuniecie_zer(stos, 0, &flaga, &nowa_dlugosc_stosu);
    if(stos->lista[0] == '\0') {
        stos->lista[0] = '0';
        minus_nowy_a = false;
    }
    if(minus_nowy_a == true) {
        stos->lista[nowa_dlugosc_stosu + 1] = '-';
        stos->lista[nowa_dlugosc_stosu + 2] = '\0';
    }
}

// Przesunięcie znaku na początek napisu (używane przy wczytywaniu znaków [.])
void przesun_w_prawo(Node*& stos, int index) {
    if(stos->lista[index] == '\0') {
        stos->lista[index+1] = stos->lista[index];
        return;
    }
    przesun_w_prawo(stos, index + 1);
    stos->lista[index + 1] = stos->lista[index];
}

int main() {
    char n;
    int lecenie_po_instrukcji = 0;
    Node* stos = nullptr;

    // Wczytywanie instrukcji do bufora wejsciowego
    while(cin.get(n)) {
        if(n == '\n') break;
        wejscie[lecenie_po_instrukcji] = n;
        lecenie_po_instrukcji++;
    }

    int i = 0;
    // Główna pętla interpretera - przetwarzanie instrukcji krok po kroku
    while(i < lecenie_po_instrukcji) {
        bool byl_skok = false;
        switch(wejscie[i]) {
            case '\'': obsluga_apostrofu(stos); break;
            case ',':  obsluga_przecinka(stos); break;
            case ':':  obsluga_dwukropka(stos); break;
            case ';':  obsluga_srednika(stos); break;
            case '@':  obsluga_at(stos); break;
            case '&':  obsluga_amperstand(stos, 0); break;
            case '.': { // Wczytanie pojedynczego znaku z wejścia i pchnięcie na stos
                char c;
                cin.get(c);
                if (c == '\n') cin.get(c);
                if (stos == nullptr) obsluga_apostrofu(stos);
                przesun_w_prawo(stos, 0);
                stos->lista[0] = c;
                break;
            }
            case '>':  obsluga_wieksze(stos); break;
            case '-': { bool flaga = false; obsluga_minusa(stos, 0, &flaga); break; }
            case '^': { bool flaga_1 = false; obsluga_daszka(stos, 0, &flaga_1); break; }
            case '[':  obsluga_kwadratu_prawy(stos); break;
            case ']':  obsluga_kwadratu_lewy(stos); break;
            case '$':  obsluga_dolara(stos); break;
            case '#':  obsluga_hasza(stos); break;
            case '<':  obsluga_mniejsze(stos); break;
            case '=':  obsluga_rowna_sie(stos); break;
            case '!':  obsluga_wykrzyknika(stos); break;
            case '~':  obsluga_tyldy(stos, i); break;
            case '?': { // Skok warunkowy
                int skok_do_instrukcji = 0;
                konwertowanie_inta(stos, &skok_do_instrukcji, 0);
                obsluga_przecinka(stos);
                bool flaga_obslugujaca_w = obsluga_zapytania(stos);
                obsluga_przecinka(stos);
                if (flaga_obslugujaca_w) {
                    i = skok_do_instrukcji;
                    byl_skok = true;
                }
                break;
            }
            case '+':  obsluga_plusa(stos); break;
            default:   // Jeśli znak nie jest instrukcją, traktuj go jako literal i pchnij na stos
                if (stos == nullptr) obsluga_apostrofu(stos);
                przesun_w_prawo(stos, 0);
                stos->lista[0] = wejscie[i];
                break;
        }
        if(!byl_skok) i++;
    } 
    return 0;
}