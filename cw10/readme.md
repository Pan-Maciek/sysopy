# Zadania - Zestaw 10  
## Celem zadania jest napisanie prostego systemu pozwalającego na granie w kółko i krzyżyk w architekturze klient/serwer.  
  
Serwer nasłuchuje jednocześnie na gnieździe sieciowym i gnieździe lokalnym.  
Klienci po uruchomieniu przesyłają do serwera swoją nazwę, a ten sprawdza czy klient o takiej nazwie już istnieje - jeśli tak, to odsyła klientowi komunikat że nazwa jest już w użyciu; jeśli nie, to zapamiętuje klienta.  
Serwer ma za zadanie łączyć klientów w pary - w przypadku pierwszego klienta wysyła mu informacje o tym, że oczekuje na drugiego klienta. Gdy drugi klient dołączy się do serwera informuje ich o połączeniu w parę, losuje zaczynającego gracza i wysyła do nich pustą planszę wraz z informacją o przydzielonym znaku (X/O).  
Klienci następnie na zmianę wykonują ruchy aż do wygranej lub remisu - wybór pól może być wykonany poprzez ich numeracje (1-9). Wiadomości o ruchach powinny być wysyłane do serwera, który je przekaże drugiemu klientowi. Po zakończonej rozgrywce klient powinien wyrejestrować się z serwera.  
  
Serwer przyjmuje jako swoje argumenty:  
  
- numer portu TCP/UDP (zależnie od zadania)  
- ścieżkę gniazda UNIX  
  
Wątek obsługujący sieć powinien obsługiwać gniazdo sieciowe i gniazdo lokalne jednocześnie, wykorzystując w tym celu funkcje do monitorowania wielu deskryptorów (epoll/poll/select).  
Dodatkowo, osobny wątek powinien cyklicznie "pingować" zarejestrowanych klientów, aby zweryfikować że wciąż odpowiadają na żądania i jeśli nie - usuwać ich z listy klientów.  
Można przyjąć, że ilość klientów zarejestrowanych na serwerze jest ograniczona do maksymalnie kilkunastu.  
  
  
Klient przyjmuje jako swoje argumenty:  
  
- swoją nazwę (string o z góry ograniczonej długości)  
- sposób połączenia z serwerem (sieć lub komunikacja lokalna przez gniazda UNIX)  
- adres serwera (adres IPv4 i numer portu lub ścieżkę do gniazda UNIX serwera)  
  
Klient przy wyłączeniu Ctrl+C powinien wyrejestrować się z serwera.  
  
  
Zadanie 1 (50%)  
Komunikacja klientów i serwera odbywa się z użyciem protokołu strumieniowego.  
  
Zadanie 2 (50%)  
Komunikacja klientów i serwera odbywa się z użyciem protokołu datagramowego.  