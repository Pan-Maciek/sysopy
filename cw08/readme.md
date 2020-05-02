# Wątki - podstawy

## Zadanie 1.

Napisz program współbieżnie wyliczający histogram dla obrazu o wymiarze nxm przy użyciu wątków, zakładając, że obraz jest macierzą reprezentującą obraz. Dla uproszczenia rozważamy jedynie obrazy w 256 odcieniach szarości. Każdy element macierzy jest więc liczbą całkowitą z zakresu 0 do 255. Program należy zaimplementować w różnych wariantach:

**Wariant 1**: Podział zadania na podzadania odbywa się na zasadzie dekompozycji w dziedzinie problemu, gdzie dziedziną jest zbiór liczb - każdy wątek zlicza wystąpienia dla określonego zestawu liczb. Zbiór liczb dla wątku można przydzielić w dowolny sposób, ale taki, by każdy wątek dostał inne liczby i zadanie było podzielone równo na wszystkie wątki. (30%)

**Wariant 2**: Podział dokonywany w dziedzinie problemu dotyczącego obrazu, a nie znaków. 

* Podział blokowy – k-ty wątek zlicza wartości pikseli w pionowym pasku o współrzędnych x-owych w przedziale od `(k−1)∗ceil(N/m)` do `k∗ceil(N/m)−1`, gdzie `N` to szerokość wyjściowego obrazu a `m` to liczba stworzonych wątków. (35%)
* Podział cykliczny – k-ty wątek zlicza wartości pikseli, których współrzędne x-owe to: k−1, k−1+m, k−1+2∗m, k−1+3∗m, itd. (ponownie, m to liczba stworzonych wątków). (35%)

Program przyjmuje następujące argumenty:

1. liczbę wątków,
2. sposób podziału obrazu pomiędzy wątki, t.j. jedną z trzech opcji: `sign / block / interleaved`,
3. nazwę pliku z wejściowym obrazem,
4. nazwę pliku wynikowego.

Po wczytaniu danych (wejściowy obraz) wątek główny tworzy tyle nowych wątków, ile zażądano w argumencie wywołania. Utworzone wątki równolegle tworzą histogram obrazu. Każdy stworzony wątek odpowiada za wygenerowanie części histogramu obrazu. Po wykonaniu obliczeń wątek kończy pracę i zwraca jako wynik (patrz `pthread_exit`) czas rzeczywisty spędzony na tworzeniu przydzielonej mu części wyjściowego obrazu. Czas ten należy zmierzyć z dokładnością do mikrosekund. 

Wątek główny czeka na zakończenie pracy przez wątki wykonujące podzadania. Po zakończeniu każdego wątku, wątek główny odczytuje wynik jego działania i wypisuje na ekranie informację o czasie, jaki zakończony wątek poświęcił na zliczanie (wraz z identyfikatorem zakończonego wątku). Dodatkowo, po zakończeniu pracy przez wszystkie stworzone wątki, wątek główny zapisuje powstały histogram (w formie linii zawierających zliczany odcień oraz liczbę jego wystąpień) do pliku wynikowego i wypisuje na ekranie czas rzeczywisty spędzony na wykonaniu zadania (z dokładnością do mikrosekund). W czasie całkowitym należy uwzględnić narzut związany z utworzeniem i zakończeniem wątków (ale bez czasu operacji wejścia/wyjścia).

Wykonaj pomiary czasu operacji dla obrazu o rozmiarze kilkaset na kilkaset pikseli. Eksperymenty wykonaj dla wszystkich trzech wariantów podziału obrazu pomiędzy wątki. Testy przeprowadź dla 1, 2, 4, i 8 wątków. Wyniki (czasy dla każdego wątku oraz całkowity czas wykonania zadania w zależności od wariantu) zamieść w pliku Times.txt i dołącz do archiwum z rozwiązaniem zadania.

## Format wejścia-wyjścia
Program powinien odczytywać i zapisywać obrazy w formacie ASCII PGM (Portable Gray Map). Pliki w tym formacie mają nagłówek postaci:

```
P2
W H
M
...
```

gdzie: W to szerokość obrazu w pikselach, H to wysokość obrazu w pikselach a M to maksymalna wartość piksela. Zakładamy, że obsługujemy jedynie obrazy w 256 odcieniach szarości: od 0 do 255 (a więc **M=255**). Po nagłówku, w pliku powinno być zapisanych W*H liczb całkowitych reprezentujących wartości kolejnych pikseli. Liczby rozdzielone są białymi znakami (np. spacją). Piksele odczytywane są wierszami, w kolejności od lewego górnego do prawego dolnego rogu obrazu.