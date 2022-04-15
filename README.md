# DirectorySyncDaemon
1. Sprawdzanie czy argumenty są ścieżkami do katalogów(błąd jeśli nie) :white_check_mark:
2. Opcjonalny argument do ustawienia długości oczekiwania demona (bazowo 5 min) :white_check_mark:
3. Porównanie katalogu źródłowego z docelowym
    - ignorowanie plików innych niż zwykłe :white_check_mark:
    - nowy plik w katalogu źródłowym jest kopiowany do docelowego :white_check_mark:
    - plik z późniejszą datą ostatniej modyfikacji w katalogu źródłowym 
      jest aktualizowany w docelowym :white_check_mark:
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
