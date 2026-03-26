# Simulador de Esteira Mecânica (Produtor/Consumidor) Sistemas Operacionais
# Feito por Bernardo Salvador na linguagem C

Este projeto implementa um simulador de esteira mecânica utilizando o paradigma produtor/consumidor, com múltiplas entradas e saídas concorrentes.

A proposta é representar um sistema em que produtos são inseridos em uma esteira, processados e classificados de acordo com suas características.

---

## Objetivo

Simular uma esteira com:

* 3 processos produtores (entradas)
* 2 processos consumidores (saídas)
* Produtos com atributos como nome, preço, qualidade e tipo
* Controle de concorrência para evitar perda de dados

---

## Funcionamento

* Os produtores geram produtos em intervalos aleatórios e os inserem na esteira.
* A esteira é representada por um buffer circular com tamanho limitado.
* Os consumidores retiram os produtos e os classificam.
* O sistema utiliza:

  * pthread para criação de threads
  * mutex para controle de acesso ao buffer
  * semáforos para controle de ocupação

---

## Estrutura do Produto

Cada produto contém:

* id: identificador único
* nome: nome do item (ex: Sensor, Motor, etc.)
* preco: valor do produto
* qualidade:

  * 0: ruim
  * 1: mediocre
  * 2: boa
* tipo: categoria do produto

---

## Fluxo do Sistema

1. Produtores geram produtos com tempos aleatórios (mais lentos)
2. Produtos são inseridos no buffer (esteira)
3. Consumidores retiram produtos com tempo menor (mais rápidos)
4. Produtos são classificados conforme a qualidade
5. Ao final, estatísticas são exibidas

---

## Concorrência

O controle do sistema é feito com:

* pthread_mutex_t: protege acesso ao buffer
* sem_t vazio: controla espaços livres
* sem_t cheio: controla itens disponíveis

Isso garante:

* Nenhuma perda de produto
* Nenhum acesso simultâneo inválido
* Sincronização correta entre threads

---

## Saída do Programa

Durante a execução:

Inserção de produtos:

```
[Produtor 1] Inseriu Sensor_3 | R$45.20 | Qualidade: BOA
```

Processamento de produtos:

```
[Consumidor 2] Sensor_3 | R$45.20 -> BOA
```

Ao final:

```
===== ESTATÍSTICAS =====
Produzidos: 50
Consumidos: 50
Bons: 20
Mediocres: 15
Ruins: 15
```

---

## Compilação e Execução

Compilar:

```
gcc esteira.c -o esteira -lpthread
```

Executar:

```
./esteira
```

## Observações

* Cada produtor possui comportamento distinto.

---

## Autor
Desenvolvido por Bernardo Petry Salvador

Projeto desenvolvido para fins acadêmicos em Sistemas Operacionais / Programação Concorrente.
