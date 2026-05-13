# Documentație privind utilizarea uneltelor AI în cadrul proiectului

Studet: Alexandru Haidet  
An universitar: 2026  
Disciplina: Sisteme de Operare  
Proiect: Sistem de raportare și monitorizare a infrastructurii urbane  


---

## Faza 1: Gestiunea fișierelor și filtrarea datelor

În prima etapă a proiectului, am apelat la asistentul Claude doar pentru a rezolva o cerință punctuală din specificație, mai exact scrierea a două funcții auxiliare necesare comenzii de filtrare: parse_condition și match_condition. Restul logicii de sistem a fost scrisă integral manual.

Pentru funcția parse_condition, am cerut asistentului să creeze o rutină în C care primește un șir de caractere de forma field:operator:value și îl împarte în cele trei componente, folosind ca separator caracterul două puncte. 

Codul returnat inițial folosea o abordare bazată pe localizarea caracterului două puncte prin apelarea funcției strchr de două ori consecutiv. Ca să nu altereze argumentul original primit din linia de comandă, codul copia șirul într-o zonă temporară înainte de prelucrare. 

Ulterior, într-o altă variantă, unealta mi-a propus să folosesc strtok. Am refuzat implementarea respectivă din start, deoarece strtok este o funcție distructivă care modifică direct zona de memorie originală și nu oferă siguranță în medii de execuție concurente sau handlere de semnal. Am păstrat ideea cu strchr, dar am adăugat manual verificări stringente. Am forțat terminarea explicită cu caracterul nul a fiecărui buffer de ieșire la dimensiunea maximă permisă, asigurându-mă că nu risc o depășire de buffer dacă se introduc argumente prea lungi în terminal. De asemenea, am pus o condiție ca funcția să dea eroare direct dacă vreunul dintre șirurile extrase rămâne gol după procesare.

Din acest exercițiu am observat că operatorii au lungimi diferite în text, motiv pentru care un parser naiv ar fi dat erori mari dacă valoarea de căutat ar fi conținut și ea caracterul două puncte. Totuși, codul generat inițial avea o scăpare: dimensionase greșit spațiul pentru operator la doar trei octeți, omițând faptul că pentru caractere ca >= ai nevoie de două poziții de text plus obligatoriul terminator nul de șir.

Pentru a doua funcție, match_condition, am transmis structura mea binară și am cerut o rutină care să verifice dacă un raport respectă o anumită condiție pe baza câmpurilor de severitate, categorie, inspector sau timestamp.

Afișajul primit de la AI a fost un lanț imens de ramificații de tip if și else if pentru fiecare câmp în parte. În interiorul fiecărei ramuri, se făcea conversia textului primit în tipul nativ de variabilă și se scria codul de verificare pentru toți cei șase operatori posibili.

După ce am analizat structura codului primit, am decis să îl rescriu parțial din mai multe motive de siguranță și mentenanță. Am eliminat zecile de linii duplicate extrăgând logica de evaluare a operatorilor într-o mică funcție internă statică numită compare_values. Aceasta primește rezultatul brut al unei comparări standard din C și decide valoarea de adevăr în funcție de operator.

Cel mai critic aspect a fost la manipularea câmpului timestamp. AI-ul a propus o simplă scădere aritmetică între valoarea din structură și valoarea convertită din text. Pe mașinile pe 64 de biți unde tipul time_t are 8 octeți, o astfel de operație matematică directă între timestamp-uri îndepărtate poate duce rapid la o depășire de semn masivă, dând peste cap rezultatul filtrării. Am înlocuit totul cu o verificare clasică bazată pe operatori de relație. De asemenea, am adăugat protecții la pointeri nuli chiar în prima linie a funcției, element de siguranță pe care generatorul automic l-a omis complet, și am introdus un mesaj de avertizare pe fluxul de eroare standard în cazul în care utilizatorul introduce un câmp inexistent.

Analiza bucăților de cod generate mi-a arătat clar că instrumentele automate scapă din vedere aspecte fundamentale legate de limitele de memorie și tipurile de date specifice programării de sistem. Logica de bază, cum ar fi citirea secvențială din fișier cu apelul de nivel jos read, deschiderea descriptorilor și aplicarea regulii cumulative pentru mai multe condiții simultane, am conceput-o și implementat-o exclusiv manual.

---

## Faza 2: Managementul proceselor și gestionarea semnalelor în UNIX

Pentru această etapă, am ales în mod deliberat să nu folosesc deloc inteligența artificială. Implementarea unui comportament asincron stabil și interacțiunea corectă dintre procese și nucleul sistemului de operare necesită un raționament logic foarte riguros, care nu poate fi lăsat pe seama unui algoritm generativ.

Toată logica programului monitor_reports și modificările din city_manager au fost scrise de la zero. Am configurat prinderea și tratarea semnalelor SIGINT și SIGUSR1 folosind exclusiv structura POSIX sigaction, evitând complet apelul de sistem învechit și nesigur numit signal. 

Cel mai important aspect de design pe care a trebuit să îl analizez singur a fost evitarea condițiilor de concurență ce apar la utilizarea funcției pause. Dacă semnalul din partea managerului ajunge fix în fereastra dintre verificarea variabilei globale și intrarea efectivă în starea de repaus a procesului, monitorul riscă să rămână blocat permanent. Am rezolvat manual aceste probleme de sincronizare, asigurând în paralel crearea, scrierea corectă și ștergerea fișierului de PID de pe disc, precum și execuția curată a comenzilor externe prin cuplul fork și exec.

---

## Faza 3: Gestiunea canalelor de comunicare și redirectări

Nici în cadrul acestei faze nu am utilizat vreun instrument de tip AI. Logica din spatele utilitarului city_hub, rutinele procesului intermediar hub_mon și programele externe destinate calculării scorurilor au fost dezvoltate integral manual.

Am implementat crearea conductelor anonime prin apeluri pipe și am gestionat tabela descriptorilor de fișiere ai proceselor copii folosind dup2. O decizie personală de arhitectură a fost transmiterea descriptorului de scriere către monitor prin intermediul unei variabile de mediu dinamice, în loc să forțez o redirecționare rigidă a fluxului standard de ieșire. Această abordare îmi permite să păstrez programul de monitorizare complet modular, ceea ce înseamnă că el poate fi rulat și testat manual ca executabil independent în terminal, dar comunică la fel de curat și atunci când este pornit în interiorul hub-ului central.