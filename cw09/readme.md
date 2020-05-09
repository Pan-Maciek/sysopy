# Zadania - Zestaw 9

Opis problemu:  
W ramach zadania należy zaimplementować rozwiązanie problemu śpiącego golibrody.  
Salon składa się z gabinetu oraz poczekalni. W gabinecie znajduje się jedno krzesło dla aktualnie obsługiwanego klienta. W poczekalni znajduje się określona liczba krzeseł (K) dla oczekujących.  
  
Zachowania golibrody:  
  
- Gdy kończy golić brodę klienta, pozwala mu opuścić gabinet, po czym idzie do poczekalni sprawdzić, czy jakiś klient tam czeka:
  - Jeśli tak, przyprowadza go do gabinetu, sadza na krześle i goli jego brodę. (Komunikat: Golibroda: czeka ___ klientow, gole klienta ID)
  - Jeśli nie, to idzie spać. (Komunikat: Golibroda: ide spac)  
  
Zachowanie klientów:  
  
- Jeśli przyszedł do salonu, golibroda jest zajęty oraz wszystkie krzesła w poczekalni są zajęte to wychodzi. (Komunikat: Zajete; ID)
- Jeśli przyszedł do salonu i golibroda jest zajęty, ale są wolne miejsca w poczekalni to zajmuje jedno z krzeseł. (Komunikat: Poczekalnia, wolne miejsca: ___; ID)
- Jeśli przyszedł do salonu i golibroda śpi to go budzi i zajmuje krzesło w gabinecie. (Komunikat: Budze golibrode; ID)  
  
Należy zaimplementować program, w którym golibroda oraz klienci to osobne wątki. Powinny one podczas działania wypisywać komunikaty o podejmowanych akcjach. Użycie odpowiednich mechanizmów ma zagwarantować niedopouszczenie, np. do zdarzeń:  

- W których dwóch klientów zajmuje jedno krzesło w poczekalni.
- Golibroda śpi, choć jest klient w poczekalni.  
  
Golenie brody przez golibrode powinno zajmować mu losową liczbę sekund. Do spania golibrody powinny być wykorzystane Warunki Sprawdzające (Condition Variables).  
Gdy klient wychodzi z salonu z powodu braku miejsca w poczekalni, powinien odczekać losową liczbe sekund i spróbować wejść jeszcze raz.  
  
Program należy zaimplementować korzystając z wątków i mechanizmów synchronizacji biblioteki POSIX Threads. Argumentami wywołania programu są: liczba krzeseł K i liczba klientów N. Po uruchomieniu programu wątek główny tworzy wątki dla golibrody i dla klientów (dla klientów wątki powinny być tworzone z losowym opóźnieniem). Należy doprowadzić do sytuacji w której wszystkie krzesła w poczekalni są zajęte.