# C++ pseudo-code-like language interpreter **po polsku**

## Wymagania

1. Kompilator kompatybilny z C++20
2. (zalecane) CMake 3.23 lub nowszy

## Rozpoczęcie

1. Sklonuj repozytorium
2. Skompiluj projekt. Jeżeli używasz CMake, to:
    - `cd <katalog_glowny_projektu>`
    - `mkdir build`
    - `cd build`
    - `cmake ..`
    - `make`
3. Uruchom program:
   `./cpp_interpreter_pl`

## Cechy Języka

### Podstawowe informacje
- Wszystko jest wyrażeniem. Instrukcje (takie jak [definiowanie funkcji](#funkcje)) zwrócą po prostu `nic`
- Każde poprawne instrukcje lub wyrażenia zostaną ewaluowane, a ich wynik wyświetlony
- Język jest niewrażliwy na białe znaki, **z wyjątkiem** nowych linii, które są zazwyczaj traktowane jako koniec instrukcji
- Instrukcje są oddzielane średnikami `;` lub nowymi liniami. Użycie `;` jest kluczowe przy pisaniu wielu instrukcji w jednej linii
- Każde wejście (input) może być kontynuowane w następnych liniach, jeśli linia wejściowa kończy się ukośnikiem odwrotnym `\`

<details><summary>Przykłady</summary>

1. Używanie średników

```
> "Hello, world!";      <- ok
"Hello, world!"
```

```
> "Hello, world!"; 2 + 2 * 2      <- ok
"Hello, world!"
6
```

```
> "Hello, world!" 2 + 2 * 2      <- nie ok
Błąd
```

2. Kontynuacja

```
> 2 + \
2 * 2        <- kontynuacja
6        <- wynik
```

</details>

### Variables

- There is **no** variable declaration
- Variable assignment uses `:=` operator
- Variable reassignment uses `=` operator
- Multiple variables can be assigned within a single statement

<details><summary>Przykłady</summary>

1. Używanie operatorów `:=` i `=`

```
> x := 6      <- ok
6
> x = 12      <- ok
12
```

```
> x = 6      <- nie ok
Błąd
```

2. Przypisanie wielu zmiennych

```
> x := y := z := 20
20
> z = y = x = "nowa"
"nowa"
```

</details>

### Typy danych

- Liczba całkowita (`calk`) - int
- Liczba zmiennoprzecinkowa (`zmienno`) - float
- Łańcuch znaków (`lancuch`) - string
- Wartość logiczna (`logiczna`) - boolean
- Lista - list
- Słownik - dictionary
- Nic - null

<details><summary>Szczegóły</summary>

1. Liczby zmiennprzecinkowe są tworzone przy pomocy kropki `.`. Bez niej zostaną potraktowane jako liczby całkowite

```
> zmienpoz := 3.
3.000000
> nieZmienpoz := 3
3
```

2. `calk` i `zmienno` **nie mogą** wykonywać opreacji arytmetycznych razem.
   Najpierw jedno z nich musi mieć narzucony typ na ten drugiego ([Narzucanie typów](#narzucanie-typów))

```
> calkow := 8
8
> zmienpoz := 12.5
12.500000
> calkow + zmienpoz
Błąd
```

3. Łańcuchy mogą używać podwójnego cydzysłowu `"` lub pojedynczego `'`. *Powinny* działać ze wszystkimi poprawnymi
   znakami utf-8.
4. Wartości logiczne to `prawda` i `falsz`
5. Listy używają nawiasów kwadratowych `[]` i mogą trzymać każdy typ danych

```
> lista := [1, 3.14, prawda, ["hello"], falsz]
[1, 3.140000, prawda, ["hello"], falsz]
```

6. Słowniki używają nawiasów klamrowych `{}`, mogą trzymać tylko podstawowy typ
   danych (`calk`, `zmienno`, `logiczna`, `lancuch`)
   jako klucze i każdy typ danych jako wartości.

```
> slow := {"klucz": "wart", prawda: ["true", 2, 3], 3.: 145}      <- ok
{"klucz": "wart", prawda: ["true", 2, 3], 3.000000: 145}
```

```
> slow := {"klucz": "wart", ["niepoprawne"]: "niepoprawne"}      <- nie ok
Błąd
```

7. Listy, słowniki i łańcuchy mogą używać [metod](#metody) i indeksowania `[]`
8. Puste wartości (`nic`) mogą zostać stworzone tylko przez wyrażenia zwracające nic (np. definiowanie funkcji)

```
> pusta := wyswietl("Nic nie zwracam")
Nic nie zwracam                           <- napis
nic                                     <- wynik
```

</details>

### Operatory

- Arytmetyczne: `+`, `-`, `*`, `/`, `//` (dzielenie całkowite), `**` (potęgowanie), `%` (modulo)
- Porównywania: `==`, `!=`, `>`, `<`, `>=`, `<=`
- Logiczne: `&` (i), `|` (lub), `!` (nie)
- Unarne: `-` (negacja), `_` (wartość bezwzględna), `?` ([konwersja logiczna](#narzucanie-typów))

<details><summary>Szczegóły</summary>

1. Na liczbach można wykonywać wszystkie działania arytmetyczne, unarne i porównywania

```
> x := 4 * 2
8
> y := -x
-8
> _y == x
prawda
```

2. Na łańcucach można wykonywać konkatenację `+`, konwersję logiczną `?` i porównywania.
   Kiedy porównuje się 2 łańcuchy `==` lub `!=`, ich zawartości będą porównane: w przeciwnym wypadku - ich długości

```
> l1 := "Hello"
"Hello"
> l2 = l1 + ", world!"
"Hello, world!"
> l1 == l2
falsz
> l1 < l2
prawda
```

3. Operatory logiczne działąją tylko z wartościami logicznymi
4. W przeciwieństwie do normalnego narzucania typów, operator `?` będzie **zawsze** konwertował porawne wyrażenie na
   wartość logiczną, wg schematu:
    - puste listy `[]` na `falsz`, w przeciwnym wypadku na `prawda`
    - puste łańcuchy `""` na `falsz`, w przeciwnym wypadku na `prawda`
    - puste słowniki `{}` na `falsz`, w przeciwnym wypadku na `prawda`
    - `nic` na `falsz`

```
> lista := [1, 3, 4]
[1, 3, 4]
> ?lista
prawda
```

```
> slownik := {}
{}
> ?slownik
falsz
```

</details>

### Narzucanie Typów

- Narzucanie typów wykonuje się ze słowem kluczowym `jako`
- Wspierane typy do narzucania to: `calk`, `zmienno`, `lancuch`, `logiczna`

<details><summary>Przykłady</summary>

1. Narzucanie typów dla operacji binarnych

```
> c := 20
20
> zm := 14.5
14.500000
> c jako zmienno + zm      <- narzucenie zmienno na calk
34.500000
```

```
> zm := 0.
0.000000
> l := prawda
prawda
> l & zm jako logiczna      <- narzucenie logiczna na zmienno
falsz
```

```
> l := "Rok: "
"Rok: "
> c := 2024
2024
> l + c jako lancuch      <- narzucenie lancuch na calk
"Rok: 2024"
```

2. `jako logiczna` vs [operator](#operatory) `?`

```
> zm := 3.14
3.140000
> ?zm                <- ok
prawda
> zm jako logiczna       <- ok
prawda
```

```
> lista := [1, 3, 15]
[1, 3, 15]
> ?lista                  <- ok
prawda
> lista jako logiczna          <- nie ok
Błąd
```

</details>

### Metody

- Metody list: `dlugosc()`, `dodaj()`, `usun()`, `wstaw()`
- Metody słowników: `wielkosc()`, `usun()`, `istnieje()`
- Metody łańcuchów: `dlugosc()`, `ltrym()`, `ptrym()`

<details><summary>Szczegóły</summary>

1. Metody `dlugosc()` i `wielkosc()` nie przyjmują żadnych argumentów, zwracają długość/wielkosć wywoływacza
2. Metody `usun()` przyjmują indeks albo klucz elementu, który chcemy usunąć, jako argument
3. Metoda `dodaj()` przyjmuje 1 argument, który zostanie dodany na koniec listy
4. Metoda `wstaw()` przyjmuje indeks jako pierwszy argument i wartość, która ma być wstawiona na ten indeks jako drugi

```
> l := [1, 2, 3, 4, 5]
[1, 2, 3, 4, 5]
> l.wstaw(2, 2.5)
[1, 2, 2.5, 3, 4, 5]
```

5. Metoda `istnieje()` przyjmuje klucz jako argument i zwraca `prawda` jeżeli klucz istnieje, inaczej `falsz`
6. Metody `ltrym()` i `rtrym()` jako argument przyjmują łańcuch znaków, które mają być usunięte zaczynając od
   odpowiedniej dla nich pozycji dopóki nie napotkany zostanie inny znak.
   Kolejność tych znaków nie ma znaczenia

```
> llancuch := "Hhhhello, world!"
"Hhhhello, world!"
> llancuch.ltrym("Hh")
"ello, world!"
```

```
> plancuch := "Hello, world! aaaabbcde"
"Hello, world! aaaabbcde"
> plancuch.ptrym("abce d")
"Hello, world!"
```

</details>

### Struktury Kontrolne

- Jeżeli-wtedy: `jezeli warunek wtedy ... [inaczej ...] stop`
- Pętla 'dla': `dla i w n..m[:k] wykonuj ... stop` lub `dla klucz w slownik wykonuj ... stop`
- Pętla 'podczas gdy': `podczas gdy warunek wykonuj ... stop`

<details><summary>Szczegóły</summary>

1. Wszystkie struktury kontrolne mogą być kontynuowane w następnych liniach bez użycia `\`, po odpowiadającym im
   ropoczynającym słowie kluczowym.
   Wejście (input) przestanie być pobierane wtedy, kiedy dla ostatniego bloku zostanie przekazane słowo kluczowe `stop`

```
> jezeli prawda wtedy      <- kontynuuje
    x := 5
stop                <- przerywa
```

```
> jezeli prawda wtedy      <- pierwszy blok
    x := 5
    jezeli falsz wtedy      <- drugi blok
        x = x + 5
    inaczej
        x = x - 5
    stop               <- pierwszy stop
stop              <- drugi stop
```

2. Wszystkie struktury kontrolne mogą być także przekazane w jednej linii

```
> x := jezeli 1 > 0 wtedy 15 inaczej "fifteen" stop         <- ostatnia ewaluowana wartość to 15
15                                                     <- wartość przypisana do x
```

3. Pętle 'dla' mogą iterować przez jakiś zakres liczbowy, a także przez klucze słownika.
   Używając pętli opartej na zakresie liczbowym, krok robiony po każdej iteracji może zostać sprecyzowane przy
   użyciu `:`. Domyślny krok jest równy 1

```
> suma := 0
0                         <- przed
> dla i w 1..6 wykonuj       <- krok = 1
    suma = suma + i
stop
21                       <- po
```

```
> suma := 0
0                          <- przed
> dla i w 1..6:2 wykonuj         <- krok = 2
    suma = suma + i
stop
9                         <- po
```

```
> slownik := {"jeden": 1, "dwa": 2, "trzy": 3}
{"jeden": 1, "dwa": 2, "trzy": 3}                  <- przed
> dla klucz w slownik wykonuj
    slownik[klucz] = 0
stop
> slownik
{"jeden": 0, "dwa": 0, "trzy": 0}               <- po
```

4. Pętle 'dla' nie mogą iterować przez listy bezpośrednio, jednak to samo zachowanie moze zostać osiągnięte
   przy użyciu [metody](#metody) `dlugosc()`

```
> lista := [1, -2, 3, -4, 5]
[1, -2, 3, -4, 5]                          <- przed
> dla i w 0..lista.dlugosc()-1 wykonuj
   lista[i] = lista[i] + 1
stop
> lista
[2, -1, 4, -3, 6]                     <- po
```

5. Maksymalna liczba iteracji pętl 'podczas gdy' to 99999

</details>

### Funkcje

- Definiowanie funkcji: `zdef nazwa_funkcji(parametry) jako ... stop`
- Wywołanie funkcji: `nazwa_funkcji(argumenty)`

<details><summary>Szczegóły</summary>

1. Definiowanie funkcji zawsze zwraca `nic`
2. Funkcje mogą użyć słowa kluczowego `zwroc`, aby wyraźnie zwrócić daną wartość.
   Bez tego słowa, ostatnie ewaluowane wyrażenie będzie domyślnie zwrócone

```
> zdef dodaj2(x) jako
   jezeli (x < 0) wtedy zwroc 0 stop
   x + 2
stop
> dodaj2(-1)
0
> dodaj2(1)
3
```

3. Kiedy chcemy zwrócić `nic` używając `return`, po słowie kluczowym `return` musi nastąpić nowa linia lub średnik `;`

```
> zdef zwrocJezeliDwa(x) jako
   jezeli x != 2 wtedy zwroc; stop       <- ok
   x
stop
> zwrocJezeliDwa(0)
null
> zwrocJezeliDwa(2)
2
```

```
> zdef zwrocJezeliDwa(x) as
   jezeli x != 2 wtedy zwroc stop       <- nie ok
   x
stop
Błąd
```

4. Funkcje nie mogą być zdefiniowane z nazwą wbudowanej funkcji

```
> zdef wyswietl() jako zwroc 20 stop
Błąd
```

5. Podczas definiowania funkcji można użyć argumentu zmiennej długości przy użyciu `..`.
   Ten parametr jest wtedy traktowany jako lista wszystkich argumentów przekazanych podczas wywoływania funkcji

```
> zdef suma(..arg) jako
   suma := 0.
   dla i w 0..arg.dlugosc()-1 wykonuj          <- iterowanie przez listę `arg`
      suma = suma + arg[i] jako zmienno
   stop
   suma                                     <- zwracana wartość
stop
> suma(13, -2, 9, 3.14, 80.5)
103.640000
```

</details>

### Wbudowane Funkcje

- `wyswietl()`: Wysiwetla wartości oddzielone `,` na standardowym wyjściu. Narzucanie typu na łańcuch nie jest konieczne
- `typ()`: Zwraca typ danej wartości w postaci łańcucha
- `zaokraglijzp()`: Zaokrągla liczbę zmiennoprzecinkową do podanej precyzji
- `zaokraglij()`: Zaokrągla liczbę zmiennoprzecinkową do najbliższej całkowitej
- `podloga()`: Zaokrągla liczbę zmiennoprzecinkową do najbliższej całkowitej nie większej niż podana liczba
- `sufit()`: Zaokrągla liczbę zmiennoprzecinkową do najbliższej całkowitej nie mniejszej niż podana liczba

<details><summary>Przykłady</summary>

1. Używanie `typ()`

```
> zp := 3.14
3.140000
> zdef wyswietlCalk(i) jako
   jezeli typ(i) != "calk" wtedy
      wyswietl("Nie całkowita!")
      zwroc
   stop
   wyswietl(i)
stop
> wyswietl(zp)
null                             <- nie wyswietlone
```

2. Używanie `zaokraglijzp()`

```
> zp := 3.141592
3.141592
> zaokraglijzp(zp, 2)
3.140000
```

</details>
