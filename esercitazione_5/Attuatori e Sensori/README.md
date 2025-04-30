# Attuatori e Sensori

Sistema di gestione temperatura per edificio simulato con processi client-server e socket TCP in C.

## Componenti

- `server`: processo centrale che riceve misure e notifica gli attuatori
- `hub`: media tra sensori e server
- `sensore`: genera temperatura e invia al hub
- `attuatore`: si sottoscrive ai sensori e decide se accendere o spegnere il riscaldamento

## Compilazione

```bash
make
