#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define TAMANHO_SENHA 30
#define TAMANHO_APELIDO 30

const char *cesar(char senha[TAMANHO_SENHA], int func);
const char *askPwdName(int option, char apelido[TAMANHO_APELIDO]);
void newPwd(char senha[TAMANHO_SENHA], char apelido[TAMANHO_APELIDO]);
void listAll(void);
void clean(void);
int checkDB(void);
int getPwd(char apelido[TAMANHO_APELIDO], int func);
void menu(void);

int main()
{
    puts("\n----- Gerenciador de Senhas -----");

    int option = 0, func = 0;
    char senha[TAMANHO_SENHA], apelido[TAMANHO_APELIDO];

    while (1)
    {
        menu();

        fflush(stdin);
        scanf("%d", &option);
        fgetc(stdin);
        if (option == 0)
            break;
        else if (option == 1 || option == 2 || option == 3 || option == 4)
        {
            strcpy(apelido, askPwdName(option, apelido));

            if (option == 1)
            {

                int apelidoExists = getPwd(apelido, 3);
                if (apelidoExists == 1)
                {
                    puts("Este apelido ja foi usado para cadastrar uma senha! Se deseja alterar uma senha existente, favor utilizar a opção \"3\" do menu.");
                    continue;
                }
                printf("Digite a senha que será cadastrada: ");
                fflush(stdin);
                fgets(senha, 102, stdin);
                if (strchr(senha, '\n') != NULL)
                    senha[strlen(senha) - 1] = '\0';
                char senhaCesar[TAMANHO_SENHA] = " ";
                strcpy(senhaCesar, cesar(senha, 0));
                newPwd(senhaCesar, apelido);

                printf("\nNova senha cadastrada com sucesso para o apelido \"%s\"!\n", apelido);
            }
            else if (option == 2)
            {
                if (checkDB() == 1)
                    continue;
                if (getPwd(apelido, 3) == 2)
                {
                    puts("Não é possível exlcuir uma senha que não foi cadastrada. Verifique os apelidos existentes através da opção \"5\" do menu.");
                    continue;
                }
                getPwd(apelido, 0);
                clean();

                printf("\nSenha \"%s\" excluída com sucesso.\n", apelido);
            }
            else if (option == 3)
            {
                if (checkDB() == 1)
                    continue;
                getPwd(apelido, 1);

                printf("\nSenha \"%s\" alterada com sucesso!\n", apelido);
            }
            else if (option == 4)
            {
                if (checkDB() == 1)
                    continue;
                getPwd(apelido, 2);
            }
        }
        else if (option == 5)
        {
            if (checkDB() == 1)
                continue;
            listAll();
        }
        else
        {
            puts("Opção inválida, por favor digite o número referente à uma opção existente no menu.");
            continue;
        }
    }
    return 0;
}

void menu(void)
{
    puts("\n----- MENU -----");
    puts("Lista de Opções:");
    puts("1 - Armazenar nova senha.");
    puts("2 - Remover senha.");
    puts("3 - Atualizar senha.");
    puts("4 - Listar senha.");
    puts("5 - Listar todas as senhas (Apelidos).");
    puts("0 - Finalizar Programa.\n");
    printf("Escolha uma opção: ");
}

void newPwd(char senha[TAMANHO_SENHA], char apelido[TAMANHO_APELIDO])
{
    FILE *db_W = fopen("./db.txt", "a");
    fprintf(db_W, "\n%s %s", apelido, senha);
    fclose(db_W);
}

int getPwd(char apelido[TAMANHO_APELIDO], int func)
{ //func == 0 -> remover senha;
    FILE *db_R = fopen("./db.txt", "r");
    char cApelido[TAMANHO_APELIDO], cSenha[TAMANHO_SENHA], cLinha[TAMANHO_APELIDO + TAMANHO_SENHA + 1];
    int linhaApelido = -1, linha = 0;
    while (!feof(db_R))
    {
        fgets(cLinha, TAMANHO_APELIDO + TAMANHO_SENHA + 2, db_R);
        if (strchr(cLinha, '\n') != NULL)
            cLinha[strlen(cLinha) - 1] = '\0';
        if (strlen(cLinha) < 3)
            continue;
        sscanf(cLinha, "%s %s\n", cApelido, cSenha);
        linha++;
        if (strcmp(cApelido, apelido) == 0)
        {
            linhaApelido = linha;
            if (func == 3)
                return 1;
            if (func == 2)
            {
                char senhaDescr[TAMANHO_SENHA];
                printf("Descriptografando a senha \"%s\"...\n", cSenha);
                strcpy(senhaDescr, cesar(cSenha, 1));
                printf("A senha \"%s\" é: %s\n", apelido, senhaDescr);
                break;
            }
        }
    }
    fclose(db_R);

    if (linhaApelido != -1)
    {
        if (func == 0 || func == 1)
        {
            db_R = fopen("./db.txt", "r");
            FILE *ftmp = fopen("./aux.tmp", "a");
            int controleLinha = 0;

            while (!feof(db_R))
            {
                fgets(cLinha, TAMANHO_APELIDO + TAMANHO_SENHA + 2, db_R);
                if (strlen(cLinha) < 3)
                    continue;

                fputs(cLinha, ftmp);
            }
            fclose(ftmp);
            fclose(db_R);
            FILE *ftmpR = fopen("./aux.tmp", "r");
            FILE *db_W = fopen("./db.txt", "w");
            for (int i = 0; i < linha; i++)
            {
                fgets(cLinha, TAMANHO_APELIDO + TAMANHO_SENHA + 2, ftmpR);
                if (strchr(cLinha, '\n') != NULL)
                    cLinha[strlen(cLinha) - 1] = '\0';
                controleLinha++;
                if (controleLinha != linhaApelido)
                {
                    fprintf(db_W, "%s%s", i == 0 ? "" : "\n", cLinha);
                }
            }

            fclose(ftmpR);
            remove("./aux.tmp");

            fclose(db_W);

            if (func == 1)
            {
                printf("Digite a nova senha que substituirá \"%s\": ", apelido);
                fflush(stdin);
                char novaSenha[TAMANHO_SENHA];
                fgets(novaSenha, 102, stdin);
                if (strchr(novaSenha, '\n') != NULL)
                    novaSenha[strlen(novaSenha) - 1] = '\0';
                strcpy(novaSenha, cesar(novaSenha, 0));
                newPwd(novaSenha, apelido);
            }
        }
    }
    else if (func == 3)
    {
        return 2;
    }
    else if (func == 2)
    {
        printf("Nenhuma senha foi cadastrada com este apelido. Verifique os apelidos existentes através da opção \"5\" do menu.\n");
    }
}

void listAll(void)
{
    FILE *db_R = fopen("./db.txt", "r");
    char cLinha[TAMANHO_APELIDO + TAMANHO_SENHA + 2], cApelido[TAMANHO_APELIDO], cSenha[TAMANHO_SENHA];

    puts("\n------- Lista de todas as suas senhas -------");
    while (!feof(db_R))
    {
        fgets(cLinha, TAMANHO_APELIDO + TAMANHO_SENHA + 2, db_R);
        if (strlen(cLinha) < 3)
            continue;
        // puts(cLinha);
        sscanf(cLinha, "%s %s", cApelido, cSenha);
        printf("%s => %s\n", cApelido, cesar(cSenha, 1));
        strcpy(cLinha, " ");
    }
    fclose(db_R);
}

const char *cesar(char senha[TAMANHO_SENHA], int func)
{ //func == 0 ? "Criptografar" : "Descriptografar";
    for (int i = 0; i < strlen(senha); i++)
    {
        char chr = senha[i];
        if (strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890", chr) != NULL)
        {
            for (int j = 0; j < 14; j++) //14 == chave de deslocamento escolhida para o sistema de criptografia de cesar
            {
                if (func == 0)
                { //Criptografar
                    if (chr == 'Z')
                        chr = 'A';
                    else if (chr == 'z')
                        chr = 'a';
                    else if (chr == '9')
                        chr = '0';
                    else
                        chr++;
                }
                else if (func == 1)
                { //Descriptografar
                    if (chr == 'A')
                        chr = 'Z';
                    else if (chr == 'a')
                        chr = 'z';
                    else if (chr == '0')
                        chr = '9';
                    else
                        chr--;
                }
            }
        }
        senha[i] = chr;
    }
    return senha;
}

void clean(void)
{
    getPwd("\n", 0);
}

int checkDB(void)
{
    char db[9] = "./db.txt";
    if (!access(db, F_OK) == 0)
    {
        puts("Arquivo \"./db.txt\" não encontrado, talvez você não tenha registrado nenhuma senha ainda.");
        return 1;
    }
    else
        return 0;
}

const char *askPwdName(int option, char apelido[TAMANHO_APELIDO])
{
    switch (option)
    {
    case 1:
        puts("\n----- Cadastro de Senha -----");
        break;
    case 2:
        puts("\n----- Exclusão de Senha -----");
        break;
    case 3:
        puts("\n----- Atualização de Senha -----");
        break;
    case 4:
        puts("\n----- Busca de Senha -----");
        break;
    default:
        break;
    }
    printf("Digite o *APELIDO* da senha que será %s: ", option == 1 ? "cadastrada" : option == 2 ? "removida"
                                                                                 : option == 3   ? "alterada"
                                                                                                 : "listada");

    fflush(stdin);
    fgets(apelido, 102, stdin);
    apelido[strlen(apelido) - 1] = '\0';

    return apelido;
}