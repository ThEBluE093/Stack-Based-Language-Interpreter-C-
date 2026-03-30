Stack-Based Language Interpreter (C++)

### O projekcie
Projekt przedstawia niskopoziomową implementację interpretera ezoterycznego języka programowania opartego na architekturze stosowej. Program parsuje ciąg instrukcji i wykonuje operacje na dynamicznej strukturze stosu, obsługując niestandardowe operacje arytmetyczne oraz sterowanie przepływem.

### Główne cechy:
Własna implementacja struktur danych: Wykorzystanie list jednokierunkowych i dynamicznej alokacji pamięci bez użycia biblioteki STL.
Object Pooling: Mechanizm optymalizacji pamięci poprzez reużywanie węzłów stosu (`Node pooling`), co minimalizuje fragmentację pamięci i narzut czasowy alokacji.
Arbitrary-Precision Arithmetic: Obsługa dodawania i odejmowania liczb o dowolnej długości (przekraczających zakresy typów `int` czy `double`) realizowana na poziomie operacji na łańcuchach znaków.
Rekurencyjna logika: Większość algorytmów przetwarzania danych została zaimplementowana w sposób rekurencyjny.

## Technologie
Język: C++ (standard ISO)
Zarządzanie pamięcią: Ręczne (wskaźniki, dynamiczne tablice `char`)
Paradygmat: Programowanie strukturalne / Proceduralne

## Opis wybranych instrukcji
Interpreter obsługuje unikalny zestaw komend, m.in.:
* `'` – Pchnięcie nowej, pustej listy na stos.
* `:` – Duplikacja elementu na szczycie stosu.
* `;` – Zamiana miejscami dwóch górnych elementów (Swap).
* `+` / `-` – Operacje arytmetyczne na wielkich liczbach.
* `?` – Instrukcja skoku (Control Flow).
* `@` – Pobranie elementu z głębi stosu na szczyt.

## Przykład działania
Program wczytuje instrukcje w jednej linii, a następnie wykonuje operacje. Dzięki obsłudze kodów ASCII i dynamicznych skoków, język ten pozwala na tworzenie pętli i skomplikowanej logiki warunkowej.

## Autor
[Jakub Rozwadowski]