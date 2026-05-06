# Documentație Utilizare AI (Proiect OS - Faza 1)

## 1. Tool-ul utilizat
Am folosit asistentul AI Gemini (Hadef).

## 2. Prompturile folosite
**Prompt 1 (pentru parse_condition):**
"Am un program în C și trebuie să parsez argumente din linia de comandă. Am un string de forma 'field:operator:value'. Scrie o funcție `int parse_condition(const char *input, char *field, char *op, char *value)` care să despartă acest string în cele 3 componente."

**Prompt 2 (pentru match_condition):**
"Am o structură `Report` care conține `int severity`, `char category[24]`, și `char inspector[24]`. Scrie o funcție `int match_condition(Report *r, const char *field, const char *op, const char *value)` care compară câmpurile structurii cu valoarea dată, în funcție de operatorul cerut (==, !=, <, <=, >, >=), și returnează 1 dacă se potrivește, altfel 0."

## 3. Ce a generat AI-ul
Pentru `parse_condition`, AI-ul a generat o implementare bazată pe `sscanf` cu formatul `%[^:]:%[^:]:%s` pentru a separa stringul pe baza caracterului dublu-punct `:`.
Pentru `match_condition`, a generat o serie de instrucțiuni `if` și `strcmp` pentru a identifica câmpul (ex: "severity"), urmate de conversia valorii din string în integer folosind `atoi()` și aplicarea operatorilor logici C (`==`, `>=`, etc.).

## 4. Ce am modificat și de ce
**Completarea operatorilor:** Codul generat inițial acoperea doar operatorii de bază. Am adăugat manual restul operatorilor (`<`, `<=`, `!=`) în logica din `match_condition` pentru a acoperi toate cazurile de test.
**Corecții de memorie (Debugging):** Am cerut ajutorul AI-ului pentru a rezolva un warning de `buffer overflow` apărut la citire. Am modificat codul pentru a folosi `sizeof(r.description)` în loc de dimensiuni hardcodate în funcția `fgets`.
**Implementarea proprie (Logica Filter):** Conform cerinței, am scris singur logica principală a comenzii `filter`: deschiderea `reports.dat` cu `open()`, citirea record cu record folosind un loop `read()`, parsarea condiției cu `parse_condition` și verificarea fiecărui rând cu `match_condition`.
