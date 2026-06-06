# Lights Out Solver — Relatório Técnico

**Disciplina:** Cálculo Numérico  
**Professor:** Bruno Xavier  
**Instituição:** UFERSA – Campus Pau dos Ferros  

---

## Sumário

1. [Objetivo](#1-objetivo)
2. [Descrição do Jogo](#2-descrição-do-jogo)
3. [Modelagem Matemática](#3-modelagem-matemática)
4. [Implementação](#4-implementação)
5. [Instruções de Execução](#5-instruções-de-execução)
6. [Exemplos de Entrada e Saída](#6-exemplos-de-entrada-e-saída)
7. [Discussão dos Resultados](#7-discussão-dos-resultados)

---

## 1. Objetivo

Implementar um solucionador para o jogo **Lights Out** utilizando modelagem matricial,
sistemas lineares, aritmética módulo 2 e eliminação gaussiana.

O programa é capaz de:

- Representar um tabuleiro do jogo;
- Construir automaticamente o sistema linear associado;
- Resolver o sistema utilizando eliminação gaussiana módulo 2;
- Determinar a solução.

---

## 2. Descrição do Jogo

O jogo **Lights Out** consiste em um tabuleiro **n × n** composto por luzes que podem
estar ligadas (`1`) ou desligadas (`0`). Ao pressionar uma posição do tabuleiro:

- a própria célula muda de estado;
- seus vizinhos ortogonais (cima, baixo, esquerda e direita) também mudam de estado.

O objetivo é **desligar todas as luzes** do tabuleiro.

---

## 3. Modelagem Matemática

Cada posição do tabuleiro corresponde a uma variável binária:

```
x_i ∈ {0, 1}
```

onde:

- `x_i = 1`: a posição **é** pressionada;
- `x_i = 0`: a posição **não é** pressionada.

O problema é representado pelo sistema linear:

```
A x = b  (mod 2)
```

onde:

- **A** é a matriz de influência (dimensão n² × n²);
- **x** é o vetor das jogadas (dimensão n²);
- **b** representa o estado inicial do tabuleiro (dimensão n²).

Todas as operações são realizadas em **módulo 2**.

A matriz **A** é construída de forma que cada coluna `k` representa o efeito de
pressionar o botão `k`: marca-se `1` na própria célula e `1` nos vizinhos ortogonais
válidos.

---

## 4. Implementação

### 4.1 Estruturas de Dados

O programa utiliza uma estrutura `Solver` que armazena:

- `n`: lado do tabuleiro;
- `n2`: n × n;
- `A`: matriz aumentada [A | b] com dimensão máxima fixa (MAX_N2 × MAX_N2+1);
- `x`: vetor solução.

### 4.2 Funções Principais

| Função | Descrição |
|--------|-----------|
| `build_system` | Constrói a matriz de influência **A** e o vetor **b** |
| `gauss_mod2` | Eliminação Gaussiana (Gauss-Jordan) módulo 2 |
| `extract_solution` | Extrai o vetor solução da forma escalonada reduzida |
| `verify_solution` | Aplica as jogadas e verifica se todas as luzes são apagadas |
| `read_board` | Interface de leitura do tabuleiro |
| `print_board` | Exibe o tabuleiro formatado |
| `print_augmented` | Exibe a matriz aumentada |

### 4.3 Eliminação Gaussiana Módulo 2

O algoritmo implementa:

1. **Busca de pivô** — percorre a coluna atual em busca de um `1`;
2. **Troca de linhas** — move a linha do pivô para a posição correta;
3. **Eliminação** — zera a coluna do pivô em **todas as outras linhas** (Gauss-Jordan);
4. **Verificação de consistência** — detecta linhas [0 … 0 | 1] (sistema impossível);
5. **Detecção de variáveis livres** — identifica colunas sem pivô;

### 4.4 Limitações

- Tamanho máximo do tabuleiro: **5 × 5** (`MAX_N = 5`);
- Entrada apenas via teclado (stdin);
- Variáveis livres são fixadas em `0` arbitrariamente.

---

## 5. Instruções de Execução

### 5.1 Compilação

Compile o código com qualquer compilador C (C99 ou superior):

```bash
gcc -std=c99 -Wall -Wextra -o projeto projeto.c
```

Para compilar com otimização:

```bash
gcc -std=c99 -Wall -Wextra -O2 -o projeto projeto.c
```

### 5.2 Execução Interativa

Execute o programa e siga as instruções no terminal:

```bash
./projeto
```

O programa solicitará:

1. O tamanho `n` do tabuleiro (1 a 5);
2. Os valores do tabuleiro (0 ou 1), linha por linha.

### 5.3 Execução com Redirecionamento de Arquivo

Prepare um arquivo de entrada (ex.: `entrada.txt`) no formato:

```
<n>
<valores do tabuleiro linha por linha>
```

Exemplo (`entrada.txt`):

```
3
1 0 1
1 1 0
0 1 0
```

Execute com redirecionamento:

```bash
./projeto < entrada.txt
```

### 5.4 Execução com Pipe

```bash
echo -e "3\n1 0 1\n1 1 0\n0 1 0\nn" | ./projeto
```

> **Nota:** Ao usar pipe, sempre inclua `n` no final para encerrar o programa.

---

## 6. Exemplos de Entrada e Saída

### 6.1 Exemplo 3×3 (do enunciado)

**Entrada:**

```
3
1 0 1
1 1 0
0 1 0
```

**Saída esperada:**

```
=== Tabuleiro inicial ===
  1 0 1
  1 1 0
  0 1 0

=== Vetor solucao x ===
  1 0 0 0 1 1 1 1 0

=== Mapa de jogadas (1 = pressionar) ===
  1 0 0
  0 1 1
  1 1 0

[OK] Solucao verificada: todas as luzes sao apagadas!
```

### 6.2 Exemplo 4×4

**Entrada:**

```
4
1 1 1 1
1 0 0 1
1 0 0 1
1 1 1 1
```

**Saída esperada:**

```
=== Tabuleiro inicial ===
  1 1 1 1
  1 0 0 1
  1 0 0 1
  1 1 1 1

[AVISO] Existem variaveis livres (multiplas solucoes possiveis).
Apresentando uma solucao com variaveis livres = 0.

=== Vetor solucao x ===
  0 0 0 0 1 1 1 1 1 1 1 1 0 0 0 0

=== Mapa de jogadas (1 = pressionar) ===
  0 0 0 0
  1 1 1 1
  1 1 1 1
  0 0 0 0

[OK] Solucao verificada: todas as luzes sao apagadas!
```

### 6.3 Exemplo 5×5 (padrão X)

**Entrada:**

```
5
1 0 0 0 1
0 1 0 1 0
0 0 1 0 0
0 1 0 1 0
1 0 0 0 1
```

**Saída esperada (solução com variáveis livres = 0):**

```
=== Tabuleiro inicial ===
  1 0 0 0 1
  0 1 0 1 0
  0 0 1 0 0
  0 1 0 1 0
  1 0 0 0 1

[AVISO] Existem variaveis livres (multiplas solucoes possiveis).
Apresentando uma solucao com variaveis livres = 0.

=== Vetor solucao x ===
  0 1 0 0 1 0 1 1 1 0 1 0 1 1 0 1 1 0 1 1 1 1 1 0 0

=== Mapa de jogadas (1 = pressionar) ===
  0 1 0 0 1
  0 1 1 1 0
  1 0 1 1 0
  1 1 0 1 1
  1 1 1 0 0

[OK] Solucao verificada: todas as luzes sao apagadas!
```

### 6.4 Exemplo 4×4 sem solução

**Entrada:**

```
4
0 0 0 0
0 0 0 0
0 0 0 0
0 0 0 1
```

**Saída esperada:**

```
=== Tabuleiro inicial ===
  0 0 0 0
  0 0 0 0
  0 0 0 0
  0 0 0 1

[RESULTADO] Este tabuleiro NAO possui solucao.
(O sistema linear e inconsistente - posto insuficiente.)
```

---

## 7. Discussão dos Resultados

### 7.1 Existência de Solução

Nem todo tabuleiro possui solução. O programa detecta sistemas inconsistentes
através da presença de linhas na forma [0 … 0 | 1] após a eliminação.

### 7.2 Múltiplas Soluções

Alguns tabuleiros admitem múltiplas soluções (variáveis livres). O programa
identifica esses casos e apresenta uma solução arbitrária (variáveis livres = 0).

### 7.3 Relação com Álgebra Linear

- **Posto da matriz**: o número de pivôs após eliminação indica o posto;
- **Dependência linear**: colunas sem pivô representam dependência linear entre
  as jogadas;
- **Espaço nulo**: as variáveis livres geram o espaço nulo da matriz;
- **Solução única**: ocorre quando o posto é máximo (n²) e o sistema é consistente.

### 7.4 Observações

- O padrão 4×4 testado apresentou variáveis livres (soluções múltiplas).
- O padrão 5×5 em X também apresentou variáveis livres.
- Tabuleiros 3×3 podem ter solução única, múltiplas soluções ou nenhuma solução.

---
