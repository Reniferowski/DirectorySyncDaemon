# DirectorySyncDaemon
1. Sprawdzanie czy argumenty są ścieżkami do katalogów(błąd jeśli nie) :white_check_mark:
2. Opcjonalny argument do ustawienia długości oczekiwania demona (bazowo 5 min)
3. Porównanie katalogu źródłowego z docelowym
    - ignorowanie plików innych niż zwykłe
    - nowy plik w katalogu źródłowym jest kopiowany do docelowego
    - plik z późniejszą datą ostatniej modyfikacji w katalogu źródłowym 
      jest aktualizowany w docelowym
    - Znalezienie pliku w katalogu docelowym, który nie znajduje się
      w źródłowym powoduje usunięcie tego pliku
4. Budzenie demona poprzez SIGUSR1
5. Logowanie
    - uśpienie/obudzenie
    - kopiowanie plików
    - usuwanie plików
    - każdy log posiada aktualną datę

Dodatkowo:
1. Opcjonalny argument -R, który pozwala synchronizować również podkatalogi
2. Zmiana metody kopiowania zależna od rozmiaru kopiowanych plików
    - Można ustawić próg dodając opcjonalny argument