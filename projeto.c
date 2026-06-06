/*
 * Lights Out Solver
 * Cálculo Numérico - UFERSA
 * Implementação: Modelagem Matricial + Eliminação Gaussiana Módulo 2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_N    5          /* tamanho máximo do lado do tabuleiro */
#define MAX_N2  (MAX_N * MAX_N)  /* número máximo de células */

/* ------------------------------------------------------------------ */
/* Tipos                                                               */
/* ------------------------------------------------------------------ */

typedef struct {
    int n;                        /* lado do tabuleiro           */
    int n2;                       /* n*n                         */
    int A[MAX_N2][MAX_N2 + 1];   /* matriz aumentada [A | b]    */
    int x[MAX_N2];                /* vetor solução               */
} Solver;

/* ------------------------------------------------------------------ */
/* Utilitários                                                         */
/* ------------------------------------------------------------------ */

/* Converte (linha i, coluna j) com base 0 para índice linear 0-based */
static inline int idx(int i, int j, int n)
{
    return i * n + j;
}

/* Imprime o tabuleiro n×n a partir de um vetor v (base 0) */
static void print_board(const char *titulo, const int *v, int n)
{
    printf("\n=== %s ===\n", titulo);
    for (int i = 0; i < n; i++) {
        printf("  ");
        for (int j = 0; j < n; j++) {
            printf("%d ", v[idx(i, j, n)]);
        }
        printf("\n");
    }
}

/* Imprime a matriz aumentada */
static void print_augmented(const Solver *s)
{
    printf("\nMatriz aumentada [A | b]:\n");
    for (int i = 0; i < s->n2; i++) {
        printf("  ");
        for (int j = 0; j < s->n2; j++) {
            printf("%d ", s->A[i][j]);
        }
        printf("| %d\n", s->A[i][s->n2]);
    }
}

/* ------------------------------------------------------------------ */
/* 1. Construção da matriz de influência A e vetor b                  */
/* ------------------------------------------------------------------ */

static void build_system(Solver *s, const int *board)
{
    int n  = s->n;
    int n2 = s->n2;

    /* Zera a matriz aumentada */
    memset(s->A, 0, sizeof(s->A));

    /* Para cada botão k, calcula quais células ele afeta */
    for (int k = 0; k < n2; k++) {
        int ki = k / n;   /* linha do botão */
        int kj = k % n;   /* coluna do botão */

        /* A própria célula */
        s->A[k][k] = 1;

        /* Vizinho de cima */
        if (ki - 1 >= 0)
            s->A[idx(ki - 1, kj, n)][k] = 1;

        /* Vizinho de baixo */
        if (ki + 1 < n)
            s->A[idx(ki + 1, kj, n)][k] = 1;

        /* Vizinho da esquerda */
        if (kj - 1 >= 0)
            s->A[idx(ki, kj - 1, n)][k] = 1;

        /* Vizinho da direita */
        if (kj + 1 < n)
            s->A[idx(ki, kj + 1, n)][k] = 1;
    }

    /* Vetor b: estado inicial vetorizado (última coluna) */
    for (int i = 0; i < n2; i++) {
        s->A[i][n2] = board[i];
    }
}

/* ------------------------------------------------------------------ */
/* 2. Eliminação Gaussiana Módulo 2                                   */
/* ------------------------------------------------------------------ */

/*
 * Retorna:
 *   1  -> sistema com solução única
 *   0  -> sistema sem solução (inconsistente)
 *  -1  -> infinitas soluções (variáveis livres)
 */
static int gauss_mod2(Solver *s)
{
    int n2  = s->n2;
    int col = 0;   /* coluna do pivô atual */

    for (int row = 0; row < n2 && col < n2; row++) {
        /* ---- busca pivô na coluna 'col' a partir de 'row' ---- */
        int pivot = -1;
        for (int i = row; i < n2; i++) {
            if (s->A[i][col] == 1) {
                pivot = i;
                break;
            }
        }

        if (pivot == -1) {
            /* Sem pivô nessa coluna: variável livre, volta linha */
            row--;
            col++;
            continue;
        }

        /* ---- troca de linhas ---- */
        if (pivot != row) {
            int tmp[MAX_N2 + 1];
            memcpy(tmp,          s->A[row],   (n2 + 1) * sizeof(int));
            memcpy(s->A[row],    s->A[pivot], (n2 + 1) * sizeof(int));
            memcpy(s->A[pivot],  tmp,         (n2 + 1) * sizeof(int));
        }

        /* ---- eliminação (todas as linhas, exceto a do pivô) ---- */
        for (int i = 0; i < n2; i++) {
            if (i != row && s->A[i][col] == 1) {
                for (int j = 0; j <= n2; j++) {
                    s->A[i][j] = (s->A[i][j] + s->A[row][j]) % 2;
                }
            }
        }

        col++;
    }

    /* ---- verifica consistência ---- */
    /* Após eliminação, procura linhas [0 0 ... 0 | 1] */
    for (int i = 0; i < n2; i++) {
        int all_zero = 1;
        for (int j = 0; j < n2; j++) {
            if (s->A[i][j]) { all_zero = 0; break; }
        }
        if (all_zero && s->A[i][n2] == 1)
            return 0;   /* inconsistente */
    }

    /* Verifica se há variáveis livres */
    /* (alguma coluna sem pivô 1 na diagonal após escalonamento) */
    for (int i = 0; i < n2; i++) {
        if (s->A[i][i] != 1) {
            /* pode haver variável livre - solução múltipla */
            /* para simplificar, assume x=0 para livres     */
            return -1;
        }
    }

    return 1;   /* solução única */
}

/* ------------------------------------------------------------------ */
/* 3. Extrai o vetor solução da forma escalonada reduzida              */
/* ------------------------------------------------------------------ */

static void extract_solution(Solver *s)
{
    int n2 = s->n2;
    for (int i = 0; i < n2; i++) {
        /* Na forma escalonada reduzida (Gauss-Jordan), cada linha
         * com pivô tem apenas um 1 na coluna do pivô.
         * Procura qual coluna tem o pivô desta linha.             */
        int pivot_col = -1;
        for (int j = 0; j < n2; j++) {
            if (s->A[i][j] == 1) { pivot_col = j; break; }
        }
        if (pivot_col >= 0) {
            s->x[pivot_col] = s->A[i][n2];
        }
    }
}

/* ------------------------------------------------------------------ */
/* 4. Verifica solução: aplica as jogadas e checa se tudo apaga       */
/* ------------------------------------------------------------------ */

static int verify_solution(const Solver *s, const int *board_orig)
{
    int n  = s->n;
    int n2 = s->n2;
    int board[MAX_N2];
    memcpy(board, board_orig, n2 * sizeof(int));

    for (int k = 0; k < n2; k++) {
        if (!s->x[k]) continue;

        int ki = k / n;
        int kj = k % n;

        /* Aplica o pressionamento: inverte células afetadas */
        board[idx(ki,     kj,     n)] ^= 1;
        if (ki - 1 >= 0) board[idx(ki - 1, kj,     n)] ^= 1;
        if (ki + 1 < n)  board[idx(ki + 1, kj,     n)] ^= 1;
        if (kj - 1 >= 0) board[idx(ki,     kj - 1, n)] ^= 1;
        if (kj + 1 < n)  board[idx(ki,     kj + 1, n)] ^= 1;
    }

    for (int i = 0; i < n2; i++) {
        if (board[i]) return 0;
    }
    return 1;
}

/* ------------------------------------------------------------------ */
/* Interface de entrada                                                */
/* ------------------------------------------------------------------ */

static int read_board(Solver *s)
{
    printf("Informe o tamanho n do tabuleiro (n x n), n entre 1 e %d: ", MAX_N);
    for (;;) {
        int ret = scanf("%d", &s->n);
        if (ret == EOF) return 0;
        if (ret == 1 && s->n >= 1 && s->n <= MAX_N) break;
        if (ret == 0) scanf("%*s");
        printf("Tamanho invalido. Digite um valor entre 1 e %d: ", MAX_N);
    }
    s->n2 = s->n * s->n;

    printf("Informe o tabuleiro (%d valores 0 ou 1, linha por linha):\n", s->n2);
    int board[MAX_N2];
    for (int i = 0; i < s->n; i++) {
        for (int j = 0; j < s->n; j++) {
            int v;
            for (;;) {
                int ret = scanf("%d", &v);
                if (ret == EOF) return 0;
                if (ret == 1 && (v == 0 || v == 1)) break;
                if (ret == 0) scanf("%*s");
                printf("\nValor invalido na posicao (%d,%d). Digite 0 ou 1: ", i + 1, j + 1);
            }
            board[idx(i, j, s->n)] = v;
        }
    }

    /* Guarda o tabuleiro original e constrói o sistema */
    int orig[MAX_N2];
    memcpy(orig, board, s->n2 * sizeof(int));

    print_board("Tabuleiro inicial", board, s->n);
    build_system(s, board);
    print_augmented(s);

    /* Resolve */
    memset(s->x, 0, sizeof(s->x));
    int status = gauss_mod2(s);

    if (status == 0) {
        printf("\n[RESULTADO] Este tabuleiro NAO possui solucao.\n");
        printf("(O sistema linear e inconsistente - posto insuficiente.)\n");
        return 1;
    }

    extract_solution(s);

    if (status == -1) {
        printf("\n[AVISO] Existem variaveis livres (multiplas solucoes possiveis).\n");
        printf("Apresentando uma solucao com variaveis livres = 0.\n");
    }

    /* Exibe solução */
    printf("\n=== Vetor solucao x ===\n  ");
    for (int i = 0; i < s->n2; i++) printf("%d ", s->x[i]);
    printf("\n");

    print_board("Mapa de jogadas (1 = pressionar)", s->x, s->n);

    /* Verifica */
    if (verify_solution(s, orig)) {
        printf("\n[OK] Solucao verificada: todas as luzes sao apagadas!\n");
    } else {
        printf("\n[ERRO] A verificacao falhou. Verifique a implementacao.\n");
    }

    return 1;
}

/* ------------------------------------------------------------------ */
/* main                                                                */
/* ------------------------------------------------------------------ */

int main(void)
{
    printf("=========================================\n");
    printf("   Lights Out Solver - Calculo Numerico  \n");
    printf("   UFERSA - Eliminacao Gaussiana mod 2   \n");
    printf("=========================================\n\n");

    for (;;) {
        Solver s;
        memset(&s, 0, sizeof(s));
        read_board(&s);

        printf("\nDeseja resolver outro tabuleiro? (s/n): ");
        char continuar;
        for (;;) {
            int ret = scanf(" %c", &continuar);
            if (ret == EOF) return 0;
            if (ret == 1 && (continuar == 's' || continuar == 'S' || continuar == 'n' || continuar == 'N')) break;
            printf("Opcao invalida. Digite s ou n: ");
        }
        if (continuar == 'n' || continuar == 'N') break;
    }

    printf("\nEncerrando programa.\n");
    return 0;
}