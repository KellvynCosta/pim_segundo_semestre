<<<<<<< HEAD
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define MAX 300

#define NOME_SZ 100
#define LOGIN_SZ 40
#define SENHA_SZ 40
#define TEL_SZ 30
#define EMAIL_SZ 100
#define TURMA_COD_SZ 40
#define TURMA_NOME_SZ 100
#define TITULO_SZ 120
#define TEMA_SZ 100
#define DESC_SZ 512
#define PROF_SZ 100

const unsigned char UTF8_BOM[3] = {0xEF,0xBB,0xBF};



typedef struct {
    char nome[NOME_SZ];
    char login[LOGIN_SZ];
    char senha[SENHA_SZ];
    int tipo;
    char telefone[TEL_SZ];
    char email[EMAIL_SZ];
    char turma[TURMA_COD_SZ];
} Usuario;

typedef struct {
    char codigo[TURMA_COD_SZ];
    char nome[TURMA_NOME_SZ];
} Turma;

typedef struct {
    char titulo[TITULO_SZ];
    char tema[TEMA_SZ];
    char descricao[DESC_SZ];
    char turma_codigo[TURMA_COD_SZ];
    char professor[PROF_SZ];
} Atividade;

typedef struct {
    char data[20];
    char turma[TURMA_COD_SZ];
    char aluno[NOME_SZ];
    char presenca[2];
    char professor[PROF_SZ];
} Frequencia;

void write_bom_if_file_empty(FILE *f) {
    fwrite(UTF8_BOM, 1, 3, f);
}

void skip_bom_if_present(FILE *f) {
    unsigned char head[3];
    size_t n = fread(head, 1, 3, f);
    if (n < 3) { fseek(f, 0, SEEK_SET); return; }
    if (!(head[0]==UTF8_BOM[0] && head[1]==UTF8_BOM[1] && head[2]==UTF8_BOM[2])) {
        fseek(f, 0, SEEK_SET);
    }
}



int carregarTurmas(Turma turmas[]) {
    FILE *f = fopen("turmas.csv", "rb");
    if (!f) return 0;
    skip_bom_if_present(f);
    int qtd = 0;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;
        char *p = strtok(line, ";");
        if (!p) continue;
        strncpy(turmas[qtd].codigo, p, TURMA_COD_SZ-1); turmas[qtd].codigo[TURMA_COD_SZ-1]=0;
        p = strtok(NULL, ";");
        if (!p) continue;
        strncpy(turmas[qtd].nome, p, TURMA_NOME_SZ-1); turmas[qtd].nome[TURMA_NOME_SZ-1]=0;
        qtd++; if (qtd>=MAX) break;
    }
    fclose(f);
    return qtd;
}

void salvarTurmas(Turma turmas[], int qtd) {
    FILE *f = fopen("turmas.csv", "wb");
    if (!f) {
        printf("Erro ao salvar turmas.\n");
        return;
    }
    write_bom_if_file_empty(f);
    for (int i=0;i<qtd;i++) {
        fprintf(f, "%s;%s\n", turmas[i].codigo, turmas[i].nome);
    }
    fclose(f);
}

int encontrarIndiceTurmaPorCodigo(Turma turmas[], int qtd, const char *codigo) {
    for (int i=0;i<qtd;i++) if (strcmp(turmas[i].codigo, codigo)==0) return i;
    return -1;
}

int carregarUsuarios(Usuario usuarios[]) {
    FILE *f = fopen("usuarios.csv", "rb");
    if (!f) return 0;
    skip_bom_if_present(f);
    int qtd = 0;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;
        char *p = strtok(line, ";");
        if (!p) continue;
        strncpy(usuarios[qtd].nome, p, NOME_SZ-1); usuarios[qtd].nome[NOME_SZ-1]=0;
        p = strtok(NULL, ";"); if (!p) continue;
        strncpy(usuarios[qtd].login, p, LOGIN_SZ-1); usuarios[qtd].login[LOGIN_SZ-1]=0;
        p = strtok(NULL, ";"); if (!p) continue;
        strncpy(usuarios[qtd].senha, p, SENHA_SZ-1); usuarios[qtd].senha[SENHA_SZ-1]=0;
        p = strtok(NULL, ";"); if (!p) continue;
        usuarios[qtd].tipo = atoi(p);
        p = strtok(NULL, ";"); if (!p) strcpy(usuarios[qtd].telefone, ""); else { strncpy(usuarios[qtd].telefone, p, TEL_SZ-1); usuarios[qtd].telefone[TEL_SZ-1]=0; }
        p = strtok(NULL, ";"); if (!p) strcpy(usuarios[qtd].email, ""); else { strncpy(usuarios[qtd].email, p, EMAIL_SZ-1); usuarios[qtd].email[EMAIL_SZ-1]=0; }
        p = strtok(NULL, ";"); if (!p) strcpy(usuarios[qtd].turma, ""); else { strncpy(usuarios[qtd].turma, p, TURMA_COD_SZ-1); usuarios[qtd].turma[TURMA_COD_SZ-1]=0; }
        qtd++; if (qtd>=MAX) break;
    }
    fclose(f);
    return qtd;
}

void salvarUsuarios(Usuario usuarios[], int qtd) {
    FILE *f = fopen("usuarios.csv", "wb");
    if (!f) { printf("Erro ao salvar usuários.\n"); return; }
    write_bom_if_file_empty(f);
    for (int i=0;i<qtd;i++) {
        fprintf(f, "%s;%s;%s;%d;%s;%s;%s\n",
                usuarios[i].nome,
                usuarios[i].login,
                usuarios[i].senha,
                usuarios[i].tipo,
                usuarios[i].telefone,
                usuarios[i].email,
                usuarios[i].turma);
    }
    fclose(f);
}

int existeLogin(Usuario usuarios[], int qtd, const char *login) {
    for (int i=0;i<qtd;i++) if (strcmp(usuarios[i].login, login)==0) return 1;
    return 0;
}

int carregarAtividades(Atividade atividades[]) {
    FILE *f = fopen("atividades.csv", "rb");
    if (!f) return 0;
    skip_bom_if_present(f);
    int qtd = 0;
    char line[1200];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line)==0) continue;
        char *p = strtok(line, ";");
        if (!p) continue;
        strncpy(atividades[qtd].titulo, p, TITULO_SZ-1); atividades[qtd].titulo[TITULO_SZ-1]=0;
        p = strtok(NULL, ";"); if (!p) continue;
        strncpy(atividades[qtd].tema, p, TEMA_SZ-1); atividades[qtd].tema[TEMA_SZ-1]=0;
        p = strtok(NULL, ";"); if (!p) continue;
        strncpy(atividades[qtd].descricao, p, DESC_SZ-1); atividades[qtd].descricao[DESC_SZ-1]=0;
        p = strtok(NULL, ";"); if (!p) continue;
        strncpy(atividades[qtd].turma_codigo, p, TURMA_COD_SZ-1); atividades[qtd].turma_codigo[TURMA_COD_SZ-1]=0;
        p = strtok(NULL, ";"); if (!p) continue;
        strncpy(atividades[qtd].professor, p, PROF_SZ-1); atividades[qtd].professor[PROF_SZ-1]=0;
        qtd++; if (qtd>=MAX) break;
    }
    fclose(f);
    return qtd;
}

void salvarAtividades(Atividade atividades[], int qtd) {
    FILE *f = fopen("atividades.csv", "wb");
    if (!f) { printf("Erro ao salvar atividades.\n"); return; }
    write_bom_if_file_empty(f);
    for (int i=0;i<qtd;i++) {
        fprintf(f, "%s;%s;%s;%s;%s\n",
                atividades[i].titulo,
                atividades[i].tema,
                atividades[i].descricao,
                atividades[i].turma_codigo,
                atividades[i].professor);
    }
    fclose(f);
}

int carregarFrequencias(Frequencia fqs[]) {
    FILE *f = fopen("frequencias.csv", "rb");
    if (!f) return 0;
    skip_bom_if_present(f);
    int qtd=0;
    char line[512];
    while (fgets(line,sizeof(line),f)) {
        line[strcspn(line,"\r\n")]=0;
        if(strlen(line)==0) continue;
        char *p=strtok(line,";");
        if(!p) continue;
        strncpy(fqs[qtd].data,p,19); fqs[qtd].data[19]=0;
        p=strtok(NULL,";"); if(!p) continue;
        strncpy(fqs[qtd].turma,p,TURMA_COD_SZ-1); fqs[qtd].turma[TURMA_COD_SZ-1]=0;
        p=strtok(NULL,";"); if(!p) continue;
        strncpy(fqs[qtd].aluno,p,NOME_SZ-1); fqs[qtd].aluno[NOME_SZ-1]=0;
        p=strtok(NULL,";"); if(!p) continue;
        strncpy(fqs[qtd].presenca,p,1); fqs[qtd].presenca[1]=0;
        p=strtok(NULL,";"); if(!p) continue;
        strncpy(fqs[qtd].professor,p,PROF_SZ-1); fqs[qtd].professor[PROF_SZ-1]=0;
        qtd++; if(qtd>=MAX) break;
    }
    fclose(f);
    return qtd;
}

void salvarFrequencias(Frequencia fqs[], int qtd) {
    FILE *f=fopen("frequencias.csv","wb");
    if(!f){ printf("Erro ao salvar frequências.\n"); return; }
    write_bom_if_file_empty(f);
    for(int i=0;i<qtd;i++)
        fprintf(f,"%s;%s;%s;%s;%s\n",
                fqs[i].data,fqs[i].turma,fqs[i].aluno,fqs[i].presenca,fqs[i].professor);
    fclose(f);
}

void obterDataAtual(char *buf, int max) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buf, max, "%Y-%m-%d", tm_info);
}

void listarTurmas(Turma turmas[], int qtdTurmas) {
    printf("\n--- Turmas cadastradas ---\n");
    if (qtdTurmas==0) { printf("Nenhuma turma cadastrada.\n"); return; }
    for (int i=0;i<qtdTurmas;i++)
        printf("%d) %s - %s\n", i+1, turmas[i].codigo, turmas[i].nome);
}

void listarUsuarios(Usuario usuarios[], int qtdUsuarios) {
    printf("\n--- Usuários ---\n");
    if (qtdUsuarios==0) { printf("Nenhum usuário.\n"); return; }
    for (int i=0;i<qtdUsuarios;i++) {
        printf("%d) %s (login: %s) - %s - Tel: %s - Email: %s - Turma: %s\n",
               i+1,
               usuarios[i].nome,
               usuarios[i].login,
               usuarios[i].tipo==1?"Admin":usuarios[i].tipo==2?"Professor":"Aluno",
               usuarios[i].telefone[0]?usuarios[i].telefone:"-",
               usuarios[i].email[0]?usuarios[i].email:"-",
               usuarios[i].turma[0]?usuarios[i].turma:"-");
    }
}

void diarioEletronico(const Usuario *prof, Usuario usuarios[], int qtdUsuarios, Turma turmas[], int qtdTurmas) {
    Frequencia fqs[MAX];
    int qtdF = carregarFrequencias(fqs);
    if (qtdTurmas==0) { printf("Nenhuma turma cadastrada.\n"); return; }
    listarTurmas(turmas, qtdTurmas);
    printf("Digite o código da turma para registrar presença: ");
    char cod[TURMA_COD_SZ];
    fgets(cod, TURMA_COD_SZ, stdin);
    cod[strcspn(cod,"\r\n")]=0;
    if (encontrarIndiceTurmaPorCodigo(turmas, qtdTurmas, cod)==-1) {
        printf("Turma inválida.\n"); return;
    }
    char data[20]; obterDataAtual(data, sizeof(data));
    printf("\nData do diário: %s\n", data);
    printf("\n--- Lista de alunos da turma %s ---\n", cod);
    int encontrou = 0;
    for (int i=0;i<qtdUsuarios;i++) {
        if (usuarios[i].tipo==3 && strcmp(usuarios[i].turma, cod)==0) {
            encontrou++;
            printf("Aluno: %s (P/A): ", usuarios[i].nome);
            char op[8];
            fgets(op, sizeof(op), stdin);
            op[strcspn(op,"\r\n")]=0;
            if (strlen(op)==0) strcpy(op,"A");
            Frequencia fq;
            strncpy(fq.data, data, sizeof(fq.data)-1); fq.data[sizeof(fq.data)-1]=0;
            strncpy(fq.turma, cod, TURMA_COD_SZ-1); fq.turma[TURMA_COD_SZ-1]=0;
            strncpy(fq.aluno, usuarios[i].nome, NOME_SZ-1); fq.aluno[NOME_SZ-1]=0;
            fq.presenca[0] = (op[0]=='P' || op[0]=='p') ? 'P' : 'A';
            fq.presenca[1] = 0;
            strncpy(fq.professor, prof->nome, PROF_SZ-1); fq.professor[PROF_SZ-1]=0;
            if (qtdF < MAX) fqs[qtdF++] = fq;
        }
    }
    if (!encontrou) {
        printf("Nenhum aluno nessa turma.\n");
        return;
    }
    salvarFrequencias(fqs, qtdF);
    printf("Presenças registradas com sucesso!\n");
}

void visualizarFrequencia(const Usuario *aluno) {
    if (aluno->turma[0]==0) {
        printf("Você não está em nenhuma turma.\n");
        return;
    }
    Frequencia fqs[MAX];
    int qtdF = carregarFrequencias(fqs);
    int total=0, pres=0;
    printf("\n--- Frequência (%s / Turma %s) ---\n", aluno->nome, aluno->turma);
    for (int i=0;i<qtdF;i++) {
        if (strcmp(fqs[i].aluno, aluno->nome)==0 && strcmp(fqs[i].turma, aluno->turma)==0) {
            printf("%s - %s (%s)\n", fqs[i].data, fqs[i].presenca[0]=='P'?"Presente":"Ausente", fqs[i].professor);
            total++;
            if (fqs[i].presenca[0]=='P') pres++;
        }
    }
    if (total==0) printf("Nenhum registro encontrado.\n");
    else printf("\nPresença: %.1f%% (%d/%d)\n", (pres*100.0)/total, pres, total);
}

void menuAdmin(Usuario usuarios[], int *qtdUsuarios, Turma turmas[], int *qtdTurmas, Atividade atividades[], int *qtdAtividades) {
    int opc;
    do {
        printf("\n--- MENU ADMIN ---\n");
        printf("1) Cadastrar turma\n");
        printf("2) Listar turmas\n");
        printf("3) Cadastrar usuário (nome/login/senha/tipo/telefone/email/turma)\n");
        printf("4) Listar usuários\n");
        printf("0) Sair\n");
        printf("Escolha: ");
        scanf("%d", &opc);
        getchar();
        if (opc==1) {
            if (*qtdTurmas >= MAX) { printf("Limite de turmas atingido.\n"); continue; }
            Turma t;
            printf("Código da turma (ex: A1): ");
            fgets(t.codigo, TURMA_COD_SZ, stdin); t.codigo[strcspn(t.codigo,"\r\n")]=0;
            if (encontrarIndiceTurmaPorCodigo(turmas, *qtdTurmas, t.codigo) != -1) {
                printf("Código já existe.\n"); continue;
            }
            printf("Nome da turma: ");
            fgets(t.nome, TURMA_NOME_SZ, stdin); t.nome[strcspn(t.nome,"\r\n")]=0;
            turmas[*qtdTurmas] = t; (*qtdTurmas)++;
            salvarTurmas(turmas, *qtdTurmas);
            printf("Turma cadastrada com sucesso.\n");
        } else if (opc==2) {
            listarTurmas(turmas, *qtdTurmas);
        } else if (opc==3) {
            if (*qtdUsuarios >= MAX) { printf("Limite de usuários atingido.\n"); continue; }
            Usuario u;
            printf("Nome completo: ");
            fgets(u.nome, NOME_SZ, stdin); u.nome[strcspn(u.nome,"\r\n")]=0;
            printf("Login (único): ");
            fgets(u.login, LOGIN_SZ, stdin); u.login[strcspn(u.login,"\r\n")]=0;
            if (existeLogin(usuarios, *qtdUsuarios, u.login)) { printf("Login já existe. Cancela.\n"); continue; }
            printf("Senha: ");
            fgets(u.senha, SENHA_SZ, stdin); u.senha[strcspn(u.senha,"\r\n")]=0;
            printf("Tipo (1=Admin,2=Professor,3=Aluno): ");
            char tmp[8]; fgets(tmp, sizeof(tmp), stdin); u.tipo = atoi(tmp);
            printf("Telefone (ou Enter para vazio): ");
            fgets(u.telefone, TEL_SZ, stdin); u.telefone[strcspn(u.telefone,"\r\n")]=0;
            printf("Email (ou Enter para vazio): ");
            fgets(u.email, EMAIL_SZ, stdin); u.email[strcspn(u.email,"\r\n")]=0;
            if (*qtdTurmas == 0) {
                u.turma[0]=0;
                printf("Nenhuma turma cadastrada. Usuario ficará sem turma.\n");
            } else {
                printf("Escolha turma pelo código (ou deixe em branco):\n");
                listarTurmas(turmas, *qtdTurmas);
                printf("Código (ex: A1) ou apenas Enter: ");
                char sel[TURMA_COD_SZ];
                fgets(sel, TURMA_COD_SZ, stdin); sel[strcspn(sel,"\r\n")]=0;
                if (strlen(sel)==0) u.turma[0]=0;
                else {
                    if (encontrarIndiceTurmaPorCodigo(turmas, *qtdTurmas, sel) == -1) {
                        printf("Turma não encontrada. Usuário ficará sem turma.\n");
                        u.turma[0]=0;
                    } else {
                        strncpy(u.turma, sel, TURMA_COD_SZ-1); u.turma[TURMA_COD_SZ-1]=0;
                    }
                }
            }
            usuarios[*qtdUsuarios] = u; (*qtdUsuarios)++;
            salvarUsuarios(usuarios, *qtdUsuarios);
            printf("Usuário cadastrado com sucesso.\n");
        } else if (opc==4) {
            listarUsuarios(usuarios, *qtdUsuarios);
        } else if (opc==0) {
            printf("Saindo do menu admin.\n");
        } else {
            printf("Opção inválida.\n");
        }
    } while (opc!=0);
}

int realizarLogin(Usuario usuarios[], int qtd) {
    char login[LOGIN_SZ], senha[SENHA_SZ];
    printf("\nLogin: ");
    scanf("%39s", login);
    printf("Senha: ");
    scanf("%39s", senha);
    for (int i=0;i<qtd;i++) {
        if (strcmp(usuarios[i].login, login)==0 && strcmp(usuarios[i].senha, senha)==0) return i;
    }
    return -1;
}

void menuProfessor(const Usuario *prof, Usuario usuarios[], int qtdUsuarios, Turma turmas[], int qtdTurmas) {
    Atividade atividades[MAX];
    int qtdAtividades = carregarAtividades(atividades);
    int opc;
    do {
        printf("\n--- MENU PROFESSOR (%s) ---\n", prof->nome);
        printf("1) Criar atividade\n");
        printf("2) Listar minhas atividades\n");
        printf("3) Diário eletrônico (registrar presenças)\n");
        printf("0) Sair\n");
        printf("Escolha: ");
        scanf("%d", &opc);
        getchar();
        if (opc==1) {
            if (qtdAtividades >= MAX) { printf("Limite de atividades atingido.\n"); continue; }
            Atividade a;
            printf("Título: ");
            fgets(a.titulo, TITULO_SZ, stdin); a.titulo[strcspn(a.titulo,"\r\n")]=0;
            printf("Tema: ");
            fgets(a.tema, TEMA_SZ, stdin); a.tema[strcspn(a.tema,"\r\n")]=0;
            printf("Descrição: ");
            fgets(a.descricao, DESC_SZ, stdin); a.descricao[strcspn(a.descricao,"\r\n")]=0;
            if (qtdTurmas==0) {
                printf("Nenhuma turma cadastrada. Atividade não criada.\n");
            } else {
                listarTurmas(turmas, qtdTurmas);
                printf("Digite o código da turma para a atividade: ");
                char cod[TURMA_COD_SZ];
                fgets(cod, TURMA_COD_SZ, stdin); cod[strcspn(cod,"\r\n")]=0;
                if (encontrarIndiceTurmaPorCodigo(turmas, qtdTurmas, cod)==-1) {
                    printf("Turma inválida. Cancelando.\n");
                } else {
                    strncpy(a.turma_codigo, cod, TURMA_COD_SZ-1); a.turma_codigo[TURMA_COD_SZ-1]=0;
                    strncpy(a.professor, prof->nome, PROF_SZ-1); a.professor[PROF_SZ-1]=0;
                    atividades[qtdAtividades++] = a;
                    salvarAtividades(atividades, qtdAtividades);
                    printf("Atividade criada e salva.\n");
                }
            }
        } else if (opc==2) {
            printf("\n--- Minhas atividades ---\n");
            int found = 0;
            for (int i=0;i<qtdAtividades;i++) {
                if (strcmp(atividades[i].professor, prof->nome)==0) {
                    found++;
                    printf("\n[%d] %s\nTema: %s\nTurma: %s\nDescrição: %s\n",
                           found, atividades[i].titulo, atividades[i].tema, atividades[i].turma_codigo, atividades[i].descricao);
                }
            }
            if (!found) printf("Você não tem atividades postadas.\n");
        } else if (opc==3) {
            diarioEletronico(prof, (Usuario *)usuarios, qtdUsuarios, turmas, qtdTurmas);
        } else if (opc==0) {
            printf("Saindo do menu professor.\n");
        } else {
            printf("Opção inválida.\n");
        }
    } while (opc!=0);
}

void menuAluno(const Usuario *aluno) {
    int opc;
    do {
        printf("\n--- MENU ALUNO (%s) ---\n", aluno->nome);
        printf("1) Ver atividades da turma\n");
        printf("2) Ver frequência\n");
        printf("0) Sair\n");
        printf("Escolha: ");
        scanf("%d",&opc); getchar();
        if(opc==1){
            Atividade atividades[MAX];
            int qtdA = carregarAtividades(atividades);
            printf("Atividades da turma %s:\n", aluno->turma);
            int found=0;
            for(int i=0;i<qtdA;i++)
                if(strcmp(atividades[i].turma_codigo,aluno->turma)==0) {
                    found++;
                    printf("\n[%d] %s\nTema:%s\nProf:%s\nDesc:%s\n",
                           found,atividades[i].titulo,atividades[i].tema,
                           atividades[i].professor,atividades[i].descricao);
                }
            if(!found) printf("Nenhuma atividade.\n");
        } else if(opc==2){
            visualizarFrequencia(aluno);
        }
    } while(opc!=0);
}

int main() {
    setlocale(LC_ALL, "");
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    Usuario usuarios[MAX];
    Turma turmas[MAX];
    Atividade atividades[MAX];

    int qtdUsuarios = carregarUsuarios(usuarios);
    int qtdTurmas   = carregarTurmas(turmas);
    int qtdAtividades = carregarAtividades(atividades);

    if (qtdUsuarios == 0) {
        strcpy(usuarios[0].nome, "Administrador");
        strcpy(usuarios[0].login, "admin");
        strcpy(usuarios[0].senha, "1234");
        usuarios[0].tipo = 1;
        usuarios[0].telefone[0]=0;
        usuarios[0].email[0]=0;
        usuarios[0].turma[0]=0;
        qtdUsuarios = 1;
        salvarUsuarios(usuarios, qtdUsuarios);
        printf("Admin padrão criado: login=admin senha=1234\n");
    }

    int opcaoSistema;
    do {
        printf("\n=== Sistema Acadêmico (UTF-8) ===\n");
        printf("1) Fazer login\n");
        printf("0) Encerrar o sistema\n");
        printf("Escolha: ");
        scanf("%d", &opcaoSistema);
        getchar();

        if (opcaoSistema == 1) {
            int idx = realizarLogin(usuarios, qtdUsuarios);
            if (idx == -1) {
                printf("Login ou senha inválidos.\n");
            } else {
                Usuario *u = &usuarios[idx];
                printf("Bem-vindo, %s!\n", u->nome);

                if (u->tipo == 1) {
                    menuAdmin(usuarios, &qtdUsuarios, turmas, &qtdTurmas, atividades, &qtdAtividades);
                } else if (u->tipo == 2) {
                    menuProfessor(u, usuarios, qtdUsuarios, turmas, qtdTurmas);
                } else {
                    menuAluno(u);
                }

                printf("\nVocê saiu da conta.\n");
            }
        } else if (opcaoSistema != 0) {
            printf("Opção inválida.\n");
        }

    } while (opcaoSistema != 0);

    printf("\nEncerrando o sistema...\n");
    return 0;
}
=======
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define MAX 300

#define NOME_SZ 100
#define LOGIN_SZ 40
#define SENHA_SZ 40
#define TEL_SZ 30
#define EMAIL_SZ 100
#define TURMA_COD_SZ 40
#define TURMA_NOME_SZ 100
#define TITULO_SZ 120
#define TEMA_SZ 100
#define DESC_SZ 512
#define PROF_SZ 100

const unsigned char UTF8_BOM[3] = {0xEF,0xBB,0xBF};

typedef struct {
    char nome[NOME_SZ];
    char login[LOGIN_SZ];
    char senha[SENHA_SZ];
    int tipo;
    char telefone[TEL_SZ];
    char email[EMAIL_SZ];
    char turma[TURMA_COD_SZ];
} Usuario;

typedef struct {
    char codigo[TURMA_COD_SZ];
    char nome[TURMA_NOME_SZ];
} Turma;

typedef struct {
    char titulo[TITULO_SZ];
    char tema[TEMA_SZ];
    char descricao[DESC_SZ];
    char turma_codigo[TURMA_COD_SZ];
    char professor[PROF_SZ];
} Atividade;

typedef struct {
    char data[20];
    char turma[TURMA_COD_SZ];
    char aluno[NOME_SZ];
    char presenca[2];
    char professor[PROF_SZ];
} Frequencia;

void write_bom_if_file_empty(FILE *f) {
    fwrite(UTF8_BOM, 1, 3, f);
}

void skip_bom_if_present(FILE *f) {
    unsigned char head[3];
    size_t n = fread(head, 1, 3, f);
    if (n < 3) { fseek(f, 0, SEEK_SET); return; }
    if (!(head[0]==UTF8_BOM[0] && head[1]==UTF8_BOM[1] && head[2]==UTF8_BOM[2])) {
        fseek(f, 0, SEEK_SET);
    }
}

int carregarTurmas(Turma turmas[]) {
    FILE *f = fopen("turmas.csv", "rb");
    if (!f) return 0;
    skip_bom_if_present(f);
    int qtd = 0;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;
        char *p = strtok(line, ";");
        if (!p) continue;
        strncpy(turmas[qtd].codigo, p, TURMA_COD_SZ-1); turmas[qtd].codigo[TURMA_COD_SZ-1]=0;
        p = strtok(NULL, ";");
        if (!p) continue;
        strncpy(turmas[qtd].nome, p, TURMA_NOME_SZ-1); turmas[qtd].nome[TURMA_NOME_SZ-1]=0;
        qtd++; if (qtd>=MAX) break;
    }
    fclose(f);
    return qtd;
}

void salvarTurmas(Turma turmas[], int qtd) {
    FILE *f = fopen("turmas.csv", "wb");
    if (!f) {
        printf("Erro ao salvar turmas.\n");
        return;
    }
    write_bom_if_file_empty(f);
    for (int i=0;i<qtd;i++) {
        fprintf(f, "%s;%s\n", turmas[i].codigo, turmas[i].nome);
    }
    fclose(f);
}

int encontrarIndiceTurmaPorCodigo(Turma turmas[], int qtd, const char *codigo) {
    for (int i=0;i<qtd;i++) if (strcmp(turmas[i].codigo, codigo)==0) return i;
    return -1;
}

int carregarUsuarios(Usuario usuarios[]) {
    FILE *f = fopen("usuarios.csv", "rb");
    if (!f) return 0;
    skip_bom_if_present(f);
    int qtd = 0;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;
        char *p = strtok(line, ";");
        if (!p) continue;
        strncpy(usuarios[qtd].nome, p, NOME_SZ-1); usuarios[qtd].nome[NOME_SZ-1]=0;
        p = strtok(NULL, ";"); if (!p) continue;
        strncpy(usuarios[qtd].login, p, LOGIN_SZ-1); usuarios[qtd].login[LOGIN_SZ-1]=0;
        p = strtok(NULL, ";"); if (!p) continue;
        strncpy(usuarios[qtd].senha, p, SENHA_SZ-1); usuarios[qtd].senha[SENHA_SZ-1]=0;
        p = strtok(NULL, ";"); if (!p) continue;
        usuarios[qtd].tipo = atoi(p);
        p = strtok(NULL, ";"); if (!p) strcpy(usuarios[qtd].telefone, ""); else { strncpy(usuarios[qtd].telefone, p, TEL_SZ-1); usuarios[qtd].telefone[TEL_SZ-1]=0; }
        p = strtok(NULL, ";"); if (!p) strcpy(usuarios[qtd].email, ""); else { strncpy(usuarios[qtd].email, p, EMAIL_SZ-1); usuarios[qtd].email[EMAIL_SZ-1]=0; }
        p = strtok(NULL, ";"); if (!p) strcpy(usuarios[qtd].turma, ""); else { strncpy(usuarios[qtd].turma, p, TURMA_COD_SZ-1); usuarios[qtd].turma[TURMA_COD_SZ-1]=0; }
        qtd++; if (qtd>=MAX) break;
    }
    fclose(f);
    return qtd;
}

void salvarUsuarios(Usuario usuarios[], int qtd) {
    FILE *f = fopen("usuarios.csv", "wb");
    if (!f) { printf("Erro ao salvar usuários.\n"); return; }
    write_bom_if_file_empty(f);
    for (int i=0;i<qtd;i++) {
        fprintf(f, "%s;%s;%s;%d;%s;%s;%s\n",
                usuarios[i].nome,
                usuarios[i].login,
                usuarios[i].senha,
                usuarios[i].tipo,
                usuarios[i].telefone,
                usuarios[i].email,
                usuarios[i].turma);
    }
    fclose(f);
}

int existeLogin(Usuario usuarios[], int qtd, const char *login) {
    for (int i=0;i<qtd;i++) if (strcmp(usuarios[i].login, login)==0) return 1;
    return 0;
}

int carregarAtividades(Atividade atividades[]) {
    FILE *f = fopen("atividades.csv", "rb");
    if (!f) return 0;
    skip_bom_if_present(f);
    int qtd = 0;
    char line[1200];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line)==0) continue;
        char *p = strtok(line, ";");
        if (!p) continue;
        strncpy(atividades[qtd].titulo, p, TITULO_SZ-1); atividades[qtd].titulo[TITULO_SZ-1]=0;
        p = strtok(NULL, ";"); if (!p) continue;
        strncpy(atividades[qtd].tema, p, TEMA_SZ-1); atividades[qtd].tema[TEMA_SZ-1]=0;
        p = strtok(NULL, ";"); if (!p) continue;
        strncpy(atividades[qtd].descricao, p, DESC_SZ-1); atividades[qtd].descricao[DESC_SZ-1]=0;
        p = strtok(NULL, ";"); if (!p) continue;
        strncpy(atividades[qtd].turma_codigo, p, TURMA_COD_SZ-1); atividades[qtd].turma_codigo[TURMA_COD_SZ-1]=0;
        p = strtok(NULL, ";"); if (!p) continue;
        strncpy(atividades[qtd].professor, p, PROF_SZ-1); atividades[qtd].professor[PROF_SZ-1]=0;
        qtd++; if (qtd>=MAX) break;
    }
    fclose(f);
    return qtd;
}

void salvarAtividades(Atividade atividades[], int qtd) {
    FILE *f = fopen("atividades.csv", "wb");
    if (!f) { printf("Erro ao salvar atividades.\n"); return; }
    write_bom_if_file_empty(f);
    for (int i=0;i<qtd;i++) {
        fprintf(f, "%s;%s;%s;%s;%s\n",
                atividades[i].titulo,
                atividades[i].tema,
                atividades[i].descricao,
                atividades[i].turma_codigo,
                atividades[i].professor);
    }
    fclose(f);
}

int carregarFrequencias(Frequencia fqs[]) {
    FILE *f = fopen("frequencias.csv", "rb");
    if (!f) return 0;
    skip_bom_if_present(f);
    int qtd=0;
    char line[512];
    while (fgets(line,sizeof(line),f)) {
        line[strcspn(line,"\r\n")]=0;
        if(strlen(line)==0) continue;
        char *p=strtok(line,";");
        if(!p) continue;
        strncpy(fqs[qtd].data,p,19); fqs[qtd].data[19]=0;
        p=strtok(NULL,";"); if(!p) continue;
        strncpy(fqs[qtd].turma,p,TURMA_COD_SZ-1); fqs[qtd].turma[TURMA_COD_SZ-1]=0;
        p=strtok(NULL,";"); if(!p) continue;
        strncpy(fqs[qtd].aluno,p,NOME_SZ-1); fqs[qtd].aluno[NOME_SZ-1]=0;
        p=strtok(NULL,";"); if(!p) continue;
        strncpy(fqs[qtd].presenca,p,1); fqs[qtd].presenca[1]=0;
        p=strtok(NULL,";"); if(!p) continue;
        strncpy(fqs[qtd].professor,p,PROF_SZ-1); fqs[qtd].professor[PROF_SZ-1]=0;
        qtd++; if(qtd>=MAX) break;
    }
    fclose(f);
    return qtd;
}

void salvarFrequencias(Frequencia fqs[], int qtd) {
    FILE *f=fopen("frequencias.csv","wb");
    if(!f){ printf("Erro ao salvar frequências.\n"); return; }
    write_bom_if_file_empty(f);
    for(int i=0;i<qtd;i++)
        fprintf(f,"%s;%s;%s;%s;%s\n",
                fqs[i].data,fqs[i].turma,fqs[i].aluno,fqs[i].presenca,fqs[i].professor);
    fclose(f);
}

void obterDataAtual(char *buf, int max) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buf, max, "%Y-%m-%d", tm_info);
}

void listarTurmas(Turma turmas[], int qtdTurmas) {
    printf("\n--- Turmas cadastradas ---\n");
    if (qtdTurmas==0) { printf("Nenhuma turma cadastrada.\n"); return; }
    for (int i=0;i<qtdTurmas;i++)
        printf("%d) %s - %s\n", i+1, turmas[i].codigo, turmas[i].nome);
}

void listarUsuarios(Usuario usuarios[], int qtdUsuarios) {
    printf("\n--- Usuários ---\n");
    if (qtdUsuarios==0) { printf("Nenhum usuário.\n"); return; }
    for (int i=0;i<qtdUsuarios;i++) {
        printf("%d) %s (login: %s) - %s - Tel: %s - Email: %s - Turma: %s\n",
               i+1,
               usuarios[i].nome,
               usuarios[i].login,
               usuarios[i].tipo==1?"Admin":usuarios[i].tipo==2?"Professor":"Aluno",
               usuarios[i].telefone[0]?usuarios[i].telefone:"-",
               usuarios[i].email[0]?usuarios[i].email:"-",
               usuarios[i].turma[0]?usuarios[i].turma:"-");
    }
}

void diarioEletronico(const Usuario *prof, Usuario usuarios[], int qtdUsuarios, Turma turmas[], int qtdTurmas) {
    Frequencia fqs[MAX];
    int qtdF = carregarFrequencias(fqs);
    if (qtdTurmas==0) { printf("Nenhuma turma cadastrada.\n"); return; }
    listarTurmas(turmas, qtdTurmas);
    printf("Digite o código da turma para registrar presença: ");
    char cod[TURMA_COD_SZ];
    fgets(cod, TURMA_COD_SZ, stdin);
    cod[strcspn(cod,"\r\n")]=0;
    if (encontrarIndiceTurmaPorCodigo(turmas, qtdTurmas, cod)==-1) {
        printf("Turma inválida.\n"); return;
    }
    char data[20]; obterDataAtual(data, sizeof(data));
    printf("\nData do diário: %s\n", data);
    printf("\n--- Lista de alunos da turma %s ---\n", cod);
    int encontrou = 0;
    for (int i=0;i<qtdUsuarios;i++) {
        if (usuarios[i].tipo==3 && strcmp(usuarios[i].turma, cod)==0) {
            encontrou++;
            printf("Aluno: %s (P/A): ", usuarios[i].nome);
            char op[8];
            fgets(op, sizeof(op), stdin);
            op[strcspn(op,"\r\n")]=0;
            if (strlen(op)==0) strcpy(op,"A");
            Frequencia fq;
            strncpy(fq.data, data, sizeof(fq.data)-1); fq.data[sizeof(fq.data)-1]=0;
            strncpy(fq.turma, cod, TURMA_COD_SZ-1); fq.turma[TURMA_COD_SZ-1]=0;
            strncpy(fq.aluno, usuarios[i].nome, NOME_SZ-1); fq.aluno[NOME_SZ-1]=0;
            fq.presenca[0] = (op[0]=='P' || op[0]=='p') ? 'P' : 'A';
            fq.presenca[1] = 0;
            strncpy(fq.professor, prof->nome, PROF_SZ-1); fq.professor[PROF_SZ-1]=0;
            if (qtdF < MAX) fqs[qtdF++] = fq;
        }
    }
    if (!encontrou) {
        printf("Nenhum aluno nessa turma.\n");
        return;
    }
    salvarFrequencias(fqs, qtdF);
    printf("Presenças registradas com sucesso!\n");
}

void visualizarFrequencia(const Usuario *aluno) {
    if (aluno->turma[0]==0) {
        printf("Você não está em nenhuma turma.\n");
        return;
    }
    Frequencia fqs[MAX];
    int qtdF = carregarFrequencias(fqs);
    int total=0, pres=0;
    printf("\n--- Frequência (%s / Turma %s) ---\n", aluno->nome, aluno->turma);
    for (int i=0;i<qtdF;i++) {
        if (strcmp(fqs[i].aluno, aluno->nome)==0 && strcmp(fqs[i].turma, aluno->turma)==0) {
            printf("%s - %s (%s)\n", fqs[i].data, fqs[i].presenca[0]=='P'?"Presente":"Ausente", fqs[i].professor);
            total++;
            if (fqs[i].presenca[0]=='P') pres++;
        }
    }
    if (total==0) printf("Nenhum registro encontrado.\n");
    else printf("\nPresença: %.1f%% (%d/%d)\n", (pres*100.0)/total, pres, total);
}

void menuAdmin(Usuario usuarios[], int *qtdUsuarios, Turma turmas[], int *qtdTurmas, Atividade atividades[], int *qtdAtividades) {
    int opc;
    do {
        printf("\n--- MENU ADMIN ---\n");
        printf("1) Cadastrar turma\n");
        printf("2) Listar turmas\n");
        printf("3) Cadastrar usuário (nome/login/senha/tipo/telefone/email/turma)\n");
        printf("4) Listar usuários\n");
        printf("0) Sair\n");
        printf("Escolha: ");
        scanf("%d", &opc);
        getchar();
        if (opc==1) {
            if (*qtdTurmas >= MAX) { printf("Limite de turmas atingido.\n"); continue; }
            Turma t;
            printf("Código da turma (ex: A1): ");
            fgets(t.codigo, TURMA_COD_SZ, stdin); t.codigo[strcspn(t.codigo,"\r\n")]=0;
            if (encontrarIndiceTurmaPorCodigo(turmas, *qtdTurmas, t.codigo) != -1) {
                printf("Código já existe.\n"); continue;
            }
            printf("Nome da turma: ");
            fgets(t.nome, TURMA_NOME_SZ, stdin); t.nome[strcspn(t.nome,"\r\n")]=0;
            turmas[*qtdTurmas] = t; (*qtdTurmas)++;
            salvarTurmas(turmas, *qtdTurmas);
            printf("Turma cadastrada com sucesso.\n");
        } else if (opc==2) {
            listarTurmas(turmas, *qtdTurmas);
        } else if (opc==3) {
            if (*qtdUsuarios >= MAX) { printf("Limite de usuários atingido.\n"); continue; }
            Usuario u;
            printf("Nome completo: ");
            fgets(u.nome, NOME_SZ, stdin); u.nome[strcspn(u.nome,"\r\n")]=0;
            printf("Login (único): ");
            fgets(u.login, LOGIN_SZ, stdin); u.login[strcspn(u.login,"\r\n")]=0;
            if (existeLogin(usuarios, *qtdUsuarios, u.login)) { printf("Login já existe. Cancela.\n"); continue; }
            printf("Senha: ");
            fgets(u.senha, SENHA_SZ, stdin); u.senha[strcspn(u.senha,"\r\n")]=0;
            printf("Tipo (1=Admin,2=Professor,3=Aluno): ");
            char tmp[8]; fgets(tmp, sizeof(tmp), stdin); u.tipo = atoi(tmp);
            printf("Telefone (ou Enter para vazio): ");
            fgets(u.telefone, TEL_SZ, stdin); u.telefone[strcspn(u.telefone,"\r\n")]=0;
            printf("Email (ou Enter para vazio): ");
            fgets(u.email, EMAIL_SZ, stdin); u.email[strcspn(u.email,"\r\n")]=0;
            if (*qtdTurmas == 0) {
                u.turma[0]=0;
                printf("Nenhuma turma cadastrada. Usuario ficará sem turma.\n");
            } else {
                printf("Escolha turma pelo código (ou deixe em branco):\n");
                listarTurmas(turmas, *qtdTurmas);
                printf("Código (ex: A1) ou apenas Enter: ");
                char sel[TURMA_COD_SZ];
                fgets(sel, TURMA_COD_SZ, stdin); sel[strcspn(sel,"\r\n")]=0;
                if (strlen(sel)==0) u.turma[0]=0;
                else {
                    if (encontrarIndiceTurmaPorCodigo(turmas, *qtdTurmas, sel) == -1) {
                        printf("Turma não encontrada. Usuário ficará sem turma.\n");
                        u.turma[0]=0;
                    } else {
                        strncpy(u.turma, sel, TURMA_COD_SZ-1); u.turma[TURMA_COD_SZ-1]=0;
                    }
                }
            }
            usuarios[*qtdUsuarios] = u; (*qtdUsuarios)++;
            salvarUsuarios(usuarios, *qtdUsuarios);
            printf("Usuário cadastrado com sucesso.\n");
        } else if (opc==4) {
            listarUsuarios(usuarios, *qtdUsuarios);
        } else if (opc==0) {
            printf("Saindo do menu admin.\n");
        } else {
            printf("Opção inválida.\n");
        }
    } while (opc!=0);
}

int realizarLogin(Usuario usuarios[], int qtd) {
    char login[LOGIN_SZ], senha[SENHA_SZ];
    printf("\nLogin: ");
    scanf("%39s", login);
    printf("Senha: ");
    scanf("%39s", senha);
    for (int i=0;i<qtd;i++) {
        if (strcmp(usuarios[i].login, login)==0 && strcmp(usuarios[i].senha, senha)==0) return i;
    }
    return -1;
}

void menuProfessor(const Usuario *prof, Usuario usuarios[], int qtdUsuarios, Turma turmas[], int qtdTurmas) {
    Atividade atividades[MAX];
    int qtdAtividades = carregarAtividades(atividades);
    int opc;
    do {
        printf("\n--- MENU PROFESSOR (%s) ---\n", prof->nome);
        printf("1) Criar atividade\n");
        printf("2) Listar minhas atividades\n");
        printf("3) Diário eletrônico (registrar presenças)\n");
        printf("0) Sair\n");
        printf("Escolha: ");
        scanf("%d", &opc);
        getchar();
        if (opc==1) {
            if (qtdAtividades >= MAX) { printf("Limite de atividades atingido.\n"); continue; }
            Atividade a;
            printf("Título: ");
            fgets(a.titulo, TITULO_SZ, stdin); a.titulo[strcspn(a.titulo,"\r\n")]=0;
            printf("Tema: ");
            fgets(a.tema, TEMA_SZ, stdin); a.tema[strcspn(a.tema,"\r\n")]=0;
            printf("Descrição: ");
            fgets(a.descricao, DESC_SZ, stdin); a.descricao[strcspn(a.descricao,"\r\n")]=0;
            if (qtdTurmas==0) {
                printf("Nenhuma turma cadastrada. Atividade não criada.\n");
            } else {
                listarTurmas(turmas, qtdTurmas);
                printf("Digite o código da turma para a atividade: ");
                char cod[TURMA_COD_SZ];
                fgets(cod, TURMA_COD_SZ, stdin); cod[strcspn(cod,"\r\n")]=0;
                if (encontrarIndiceTurmaPorCodigo(turmas, qtdTurmas, cod)==-1) {
                    printf("Turma inválida. Cancelando.\n");
                } else {
                    strncpy(a.turma_codigo, cod, TURMA_COD_SZ-1); a.turma_codigo[TURMA_COD_SZ-1]=0;
                    strncpy(a.professor, prof->nome, PROF_SZ-1); a.professor[PROF_SZ-1]=0;
                    atividades[qtdAtividades++] = a;
                    salvarAtividades(atividades, qtdAtividades);
                    printf("Atividade criada e salva.\n");
                }
            }
        } else if (opc==2) {
            printf("\n--- Minhas atividades ---\n");
            int found = 0;
            for (int i=0;i<qtdAtividades;i++) {
                if (strcmp(atividades[i].professor, prof->nome)==0) {
                    found++;
                    printf("\n[%d] %s\nTema: %s\nTurma: %s\nDescrição: %s\n",
                           found, atividades[i].titulo, atividades[i].tema, atividades[i].turma_codigo, atividades[i].descricao);
                }
            }
            if (!found) printf("Você não tem atividades postadas.\n");
        } else if (opc==3) {
            diarioEletronico(prof, (Usuario *)usuarios, qtdUsuarios, turmas, qtdTurmas);
        } else if (opc==0) {
            printf("Saindo do menu professor.\n");
        } else {
            printf("Opção inválida.\n");
        }
    } while (opc!=0);
}

void menuAluno(const Usuario *aluno) {
    int opc;
    do {
        printf("\n--- MENU ALUNO (%s) ---\n", aluno->nome);
        printf("1) Ver atividades da turma\n");
        printf("2) Ver frequência\n");
        printf("0) Sair\n");
        printf("Escolha: ");
        scanf("%d",&opc); getchar();
        if(opc==1){
            Atividade atividades[MAX];
            int qtdA = carregarAtividades(atividades);
            printf("Atividades da turma %s:\n", aluno->turma);
            int found=0;
            for(int i=0;i<qtdA;i++)
                if(strcmp(atividades[i].turma_codigo,aluno->turma)==0) {
                    found++;
                    printf("\n[%d] %s\nTema:%s\nProf:%s\nDesc:%s\n",
                           found,atividades[i].titulo,atividades[i].tema,
                           atividades[i].professor,atividades[i].descricao);
                }
            if(!found) printf("Nenhuma atividade.\n");
        } else if(opc==2){
            visualizarFrequencia(aluno);
        }
    } while(opc!=0);
}

int main() {
    setlocale(LC_ALL, "");
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    Usuario usuarios[MAX];
    Turma turmas[MAX];
    Atividade atividades[MAX];

    int qtdUsuarios = carregarUsuarios(usuarios);
    int qtdTurmas   = carregarTurmas(turmas);
    int qtdAtividades = carregarAtividades(atividades);

    if (qtdUsuarios == 0) {
        strcpy(usuarios[0].nome, "Administrador");
        strcpy(usuarios[0].login, "admin");
        strcpy(usuarios[0].senha, "1234");
        usuarios[0].tipo = 1;
        usuarios[0].telefone[0]=0;
        usuarios[0].email[0]=0;
        usuarios[0].turma[0]=0;
        qtdUsuarios = 1;
        salvarUsuarios(usuarios, qtdUsuarios);
        printf("Admin padrão criado: login=admin senha=1234\n");
    }

    printf("=== Sistema Acadêmico (UTF-8) ===\n");
    int idx = realizarLogin(usuarios, qtdUsuarios);
    if (idx == -1) { printf("Login ou senha inválidos.\n"); return 0; }

    Usuario *u = &usuarios[idx];
    printf("Bem-vindo, %s!\n", u->nome);

    if (u->tipo == 1) {
        menuAdmin(usuarios, &qtdUsuarios, turmas, &qtdTurmas, atividades, &qtdAtividades);
    } else if (u->tipo == 2) {
        menuProfessor(u, usuarios, qtdUsuarios, turmas, qtdTurmas);
    } else {
        menuAluno(u);
    }

    printf("Encerrando.\n");
    return 0;
}
>>>>>>> 163236704c2f14babb050f852702225219ccf144
