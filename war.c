#include <stdio.h>
#include <stdlib.h> // Para alocação de memória (malloc, free) e rand
#include <string.h> // Para manipulação de strings (strcpy, strcmp)
#include <time.h>   // Para inicializar a semente de aleatoriedade (srand)

// --- DEFINIÇÃO DAS ESTRUTURAS ---

// Estrutura para representar um Território.
struct Territorio {
    char nome[30];
    char cor[10];
    int tropas;
};

// Estrutura para um Jogador.
struct Jogador {
    char cor[10];           // Cor do exército do jogador
    char *missao;           // Ponteiro para a missão
    int numTerritorios;     // Contador de territórios
};


// --- PROTÓTIPOS DAS FUNÇÕES ---
void cadastrarTerritorios(struct Territorio *mapa, int numTerritorios);
void atribuirMissao(char **destino, char *missoes[], int totalMissoes);
void exibirMissao(char *missao);
void liberarMemoria(struct Territorio *mapa, struct Jogador *jogadores, int numJogadores);
int rolarDado();
void exibirTerritorios(struct Territorio *mapa, int numTerritorios);
void atacar(struct Territorio *atacante, struct Territorio *defensor, struct Jogador *jogadorAtacante);
int verificarMissao(struct Jogador *jogador, int numTerritoriosJogador);


// --- IMPLEMENTAÇÃO DAS FUNÇÕES AUXILIARES E DE MISSÃO ---

// Simula a rolagem de um dado (1 a 6).
int rolarDado() {
    return (rand() % 6) + 1;
}

// Sorteia e aloca a missão para o jogador.
void atribuirMissao(char **destino, char *missoes[], int totalMissoes) {
    int indiceSorteado = rand() % totalMissoes;
    char *missaoSorteada = missoes[indiceSorteado];
    size_t tamanhoMissao = strlen(missaoSorteada) + 1;

    // Aloca memória dinamicamente para a string da missão
    *destino = (char*) malloc(tamanhoMissao);
    if (*destino == NULL) {
        printf("Erro de alocação de memória para a missão.\n");
        exit(1); 
    }
    
    strcpy(*destino, missaoSorteada);
}

// Exibe a missão do jogador.
void exibirMissao(char *missao) {
    printf("\nMISSAO: %s\n", missao);
    printf("------------------------------------------------------\n");
}

// Verifica se a missão (conquistar 3 territórios) foi cumprida.
int verificarMissao(struct Jogador *jogador, int numTerritoriosJogador) {
    if (numTerritoriosJogador >= 3) {
        return 1; // Missão cumprida
    }
    return 0; // Missão não cumprida
}


// --- IMPLEMENTAÇÃO DAS FUNÇÕES DE JOGO E ATAQUE ---

// Preenche os dados dos territórios.
void cadastrarTerritorios(struct Territorio *mapa, int numTerritorios) {
    int i;
    for (i = 0; i < numTerritorios; i++) {
        printf("--- Cadastro do Território %d ---\n", i + 1);

        printf("Nome: ");
        scanf("%29s", (mapa + i)->nome);

        printf("Cor do Exército: ");
        scanf("%9s", (mapa + i)->cor);

        do {
            printf("Tropas (min 1): ");
            scanf("%d", &(mapa + i)->tropas);
        } while ((mapa + i)->tropas < 1);

        printf("\n");
    }
}

// Exibe as informações de todos os territórios.
void exibirTerritorios(struct Territorio *mapa, int numTerritorios) {
    int i;
    printf("\n======================================================\n");
    printf("STATUS ATUAL DO MAPA:\n");
    printf("======================================================\n");
    for (i = 0; i < numTerritorios; i++) {
        printf("[%d] Nome: %s\n", i, (mapa + i)->nome);
        printf("    Cor: %s\n", (mapa + i)->cor);
        printf("    Tropas: %d\n", (mapa + i)->tropas);
        printf("------------------------------------------------------\n");
    }
}

// Simula um ataque entre dois territórios e atualiza seus dados.
void atacar(struct Territorio *atacante, struct Territorio *defensor, struct Jogador *jogadorAtacante) {
    if (atacante->tropas < 2) {
        printf("\nO território %s precisa de pelo menos 2 tropas para atacar!\n", atacante->nome);
        return;
    }
    
    // Conquista automática se o defensor não tiver tropas
    if (defensor->tropas < 1) {
        if (strcmp(atacante->cor, defensor->cor) != 0) {
            printf("\n%s conquistado automaticamente!\n", defensor->nome);
            jogadorAtacante->numTerritorios++; 
        }
        
        strcpy(defensor->cor, atacante->cor);
        defensor->tropas = 1;
        atacante->tropas -= 1;
        return;
    }

    printf("\n--- BATALHA ---\n");
    int dadoAtacante = rolarDado();
    int dadoDefensor = rolarDado();

    printf("Dados: Atacante (%d) vs Defensor (%d)\n", dadoAtacante, dadoDefensor);

    if (dadoAtacante > dadoDefensor) {
        printf("ATACANTE VENCEU! %s conquistou %s.\n", atacante->nome, defensor->nome);
        
        if (strcmp(atacante->cor, defensor->cor) != 0) {
            jogadorAtacante->numTerritorios++; 
            printf("Conquista! Total de territórios: %d\n", jogadorAtacante->numTerritorios);
        }

        // Lógica de Transferência de Tropas (mantendo 1 no atacante)
        int tropasDisponiveis = atacante->tropas - 1; 
        int tropasTransferidas = (tropasDisponiveis >= 1) ? (tropasDisponiveis / 2) : 0;
        if (tropasTransferidas < 1 && tropasDisponiveis >= 1) tropasTransferidas = 1;

        // Atualiza posse e tropas
        strcpy(defensor->cor, atacante->cor);
        defensor->tropas = tropasTransferidas;
        atacante->tropas -= tropasTransferidas;

    } else { 
        printf("ATACANTE PERDEU ou EMPATOU. Defensor manteve.\n");
        atacante->tropas -= 1;
        printf("%s perdeu 1 tropa e agora tem %d.\n", atacante->nome, atacante->tropas);
    }
}


// --- FUNÇÃO DE LIBERAÇÃO DE MEMÓRIA ---

// Libera toda a memória alocada dinamicamente.
void liberarMemoria(struct Territorio *mapa, struct Jogador *jogadores, int numJogadores) {
    if (mapa != NULL) {
        free(mapa);
    }
    
    // Libera a missão de cada jogador (malloc), depois o vetor de jogadores (calloc)
    if (jogadores != NULL) {
        int i;
        for (i = 0; i < numJogadores; i++) {
            if (jogadores[i].missao != NULL) {
                free(jogadores[i].missao); 
            }
        }
        free(jogadores);
    }

    printf("\nMemória liberada com sucesso.\n");
}


// --- FUNÇÃO PRINCIPAL ---

int main() {
    srand(time(NULL));

    // Vetor de Missões
    char *missoesGerais[] = {
        "Conquistar 3 territórios (Regra Simples: 3 territorios)",
        "Eliminar todas as tropas da cor Vermelha.",
        "Conquistar todos os territórios da Oceania.",
        "Ter um exército com 10 ou mais tropas em um unico território.",
        "Conquistar o território 'Alaska'."
    };
    int totalMissoes = sizeof(missoesGerais) / sizeof(missoesGerais[0]);

    struct Territorio *mapa = NULL;
    struct Jogador *jogadores = NULL;
    int numTerritorios = 0;
    int numJogadores = 2;

    printf("--- Jogo War Estruturado ---\n");

    // 1. Alocação de Territórios
    printf("Quantos territórios no mapa? (Min 2): ");
    scanf("%d", &numTerritorios);

    if (numTerritorios < 2) {
        printf("Numero de territórios insuficiente. Encerrando.\n");
        return 1;
    }

    mapa = (struct Territorio*) calloc(numTerritorios, sizeof(struct Territorio));
    // ... Verificação de alocação de mapa

    // 2. Alocação e Inicialização de Jogadores
    jogadores = (struct Jogador*) calloc(numJogadores, sizeof(struct Jogador));
    // ... Verificação de alocação de jogadores

    // Definição das Cores
    strcpy(jogadores[0].cor, "Azul");
    strcpy(jogadores[1].cor, "Vermelho");

    // 3. Atribuição de Missões
    atribuirMissao(&(jogadores[0].missao), missoesGerais, totalMissoes);
    atribuirMissao(&(jogadores[1].missao), missoesGerais, totalMissoes);
    
    printf("\nJOGADOR AZUL:\n");
    exibirMissao(jogadores[0].missao);
    printf("\nJOGADOR VERMELHO:\n");
    exibirMissao(jogadores[1].missao);
    
    // 4. Cadastro e Contagem Inicial
    cadastrarTerritorios(mapa, numTerritorios);
    
    // Contagem inicial de territórios
    int i;
    for(i = 0; i < numTerritorios; i++) {
        if (strcmp(mapa[i].cor, jogadores[0].cor) == 0) {
            jogadores[0].numTerritorios++;
        } else if (strcmp(mapa[i].cor, jogadores[1].cor) == 0) {
            jogadores[1].numTerritorios++;
        }
    }
    
    // 5. Loop do Jogo (Ataques)
    char continuarAtaque;
    int jogadorAtual = 0; 
    int escolhaAtacante, escolhaDefensor;
    
    do {
        exibirTerritorios(mapa, numTerritorios);

        printf("\nTURNO DO JOGADOR: %s\n", jogadores[jogadorAtual].cor);
        
        // Loop interno para garantir uma escolha válida
        do {
            printf("\nOpções: [Índice] ou -1 para encerrar.\n");
            
            printf("Atacante (cor %s): ", jogadores[jogadorAtual].cor);
            if (scanf("%d", &escolhaAtacante) != 1) {
                while(getchar() != '\n'); 
                escolhaAtacante = -2;
                continue;
            }
            if (escolhaAtacante == -1) break; 
            
            printf("Defensor (cor inimiga): ");
            if (scanf("%d", &escolhaDefensor) != 1) {
                while(getchar() != '\n');
                escolhaDefensor = -2;
                continue;
            }

            // 1. Validação de Índices
            if (escolhaAtacante < 0 || escolhaAtacante >= numTerritorios ||
                escolhaDefensor < 0 || escolhaDefensor >= numTerritorios ||
                escolhaAtacante == escolhaDefensor) {
                printf("Escolha inválida. Tente novamente.\n");
                continue;
            }
            
            struct Territorio *atacante = mapa + escolhaAtacante;
            struct Territorio *defensor = mapa + escolhaDefensor;

            // 2. Validação: Atacante deve ser do jogador atual
            if (strcmp(atacante->cor, jogadores[jogadorAtual].cor) != 0) {
                printf("Território atacante nao pertence a sua cor. Escolha outro.\n");
                continue;
            }

            // 3. Validação: Não pode atacar território da mesma cor
            if (strcmp(atacante->cor, defensor->cor) == 0) {
                printf("Nao pode atacar territorio da mesma cor. Escolha um inimigo.\n");
                continue; 
            }
            
            break;

        } while (escolhaAtacante != -1);
        
        if (escolhaAtacante == -1) {
            continuarAtaque = 'n';
            break;
        }

        // Executa o ataque
        struct Territorio *atacante = mapa + escolhaAtacante;
        struct Territorio *defensor = mapa + escolhaDefensor;
        
        atacar(atacante, defensor, &jogadores[jogadorAtual]);
        
        // Verificação de Missão
        if (verificarMissao(&jogadores[jogadorAtual], jogadores[jogadorAtual].numTerritorios)) {
            printf("\n\nJOGADOR %s VENCEU!\n", jogadores[jogadorAtual].cor);
            printf("Missão Cumprida: %s\n", jogadores[jogadorAtual].missao);
            continuarAtaque = 'n'; 
            break;
        }
        
        // Troca de turno
        jogadorAtual = 1 - jogadorAtual; 

        printf("\nProximo turno? (s/n): ");
        scanf(" %c", &continuarAtaque);

    } while (continuarAtaque == 's' || continuarAtaque == 'S');

    // 6. Liberação de memória
    liberarMemoria(mapa, jogadores, numJogadores);
    
    printf("\nPrograma finalizado.\n");

    return 0;
}
