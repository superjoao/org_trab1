#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
    #include <conio.h> //"getch" Windows.
#else
    #include <termios.h>
    #include <unistd.h> //"getchar sem enter" Linux.
#endif



#define HARDDISKLEN 2048
#define NFILES HARDDISKLEN/8
#define NCOLS 20

typedef struct{
    int freeSpace;
    int usedSpace;

}hardDisk;

typedef struct {
    char name[NCOLS];
    int id;
    int size;
    int head;
    int tail;

}metadataFile;

typedef struct{
    int rows;
    int cols;
    char **matrix;
} smartMatrix;

FILE *b;

//Tools
//Tools-Função: limpar tela
void limparTela() {
    #ifdef _WIN32
        system("cls") ; //limpar a tela no Windows
    #else
        system("clear"); //limpar a tela no Linux
    #endif
}

//Tools-Função: getch para Linux
char nonBlockingGetchar() {

    char ch;
    #ifdef _WIN32
        ch = getch();
    #else
        struct termios oldt, newt;
        int oldf;

        // Salva as configurações atuais do terminal
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;

        // Desabilita a leitura de caracteres da entrada padrão
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        // Lê um caractere
        ch = getchar();

        // Restaura as configurações originais do terminal
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    #endif

    return ch;
}

//Tools-Função: Faz a tela do terminal esperar input
void blockGetchar(){
    printf("\nPara continuar precione qualquer tecla.\n");
    getchar();
}

//Tools-Função: Atualiza o arquivo txt com o novo array sem o elemento deletado
void updateFile(FILE *b, metadataFile *mf[], int size) {
    b = fopen("Alunos.txt", "w");
    b = fopen("Alunos.txt", "ab+");
    if (!b) {
        printf("Erro na abertura do arquivo!");
        return;
    }

    
    for (int i = 0; i < size; i++) {
        if (mf[i]->tail > 0) {
            fprintf(b, "\n%s\n", mf[i]->name);
            fprintf(b, "%d\n", mf[i]->id);
            fprintf(b, "%d\n", mf[i]->size);
            fprintf(b, "%d\n", mf[i]->head);
            fprintf(b, "%d\n", mf[i]->tail);
        }
    }

    fclose(b); 
}

//Tools-Função: carrega do arquivo.txt para as variaveis metadatas
void loadMetadataFromFile(metadataFile *mf[], FILE *b) {
    if (!b) {
        printf("Erro ao abrir o arquivo para leitura.\n");
        return;
    }

    for (int i = 0; i < NFILES; i++) {
        if (fscanf(b, "%s", mf[i]->name) == 1 &&
            fscanf(b, "%d", &mf[i]->size) == 1 &&
            fscanf(b, "%d", &mf[i]->id) == 1 &&
            fscanf(b, "%d", &mf[i]->head) == 1 &&
            fscanf(b, "%d", &mf[i]->tail) == 1) {
            // Dados lidos com sucesso do arquivo
        } else {
            printf("Erro ao ler os dados do arquivo.\n");
            return;
        }
    }

    fclose(b);
}

//Tools-Função: Armazena as variaveis do metadata no arquivo.txt
void saveMetadataToFile(metadataFile *mf[], FILE *b,  int id){
    
    
    if(!b){
        printf("Erro na abertura do arquivo!");
        return;
    }else{   

        if (mf)
        {
            //armazenar a string no arquivo
            if (mf[0])
            {
            
                fprintf(b, "\n%s\n", mf[id]->name);
                fprintf(b, "%d\n", mf[id]->id);
                fprintf(b, "%d\n", mf[id]->size);
                fprintf(b, "%d\n", mf[id]->head);
                fprintf(b, "%d\n", mf[id]->tail);
            }
        }
    }
    fclose(b);
    
}

//Libera espaço da matrix inteligente
void freeSmartMatrix(smartMatrix *m){
    for (int i = 0; i < m->rows; i++) {
        free(m->matrix[i]);
    }
    free(m->matrix);
}

//Tools-Função: Inicialização de Matriz inteligente para os nomes do Menu
smartMatrix* newSmartMatrix(int line, smartMatrix *m){
    
    
    // aloca um vetor de LIN ponteiros para linhas
    m = malloc (sizeof(smartMatrix));
    
    if(!m) { 
        printf("\nErro de alocacao de memoria (3)!\n");
        return 0;
    }
    else{
        
        m->rows = line;
        m->cols = NCOLS;

        // aloca um vetor com todos os elementos da matriz
        m->matrix = malloc (m->rows * sizeof (char*));

        if(!m->matrix) { 
            printf("\nErro de alocacao de memoria (2)!\n");
            return 0;
        }
        else{
            
            // ajusta os demais ponteiros de linhas (i > 0)
            for (int i=0; i < m->rows; i++){
                m->matrix[i]= malloc((m->cols + 1) * sizeof(char));
                if(!m->matrix[i]){ 
                    printf("\nErro de alocacao de memoria (3)!\n");
                    return 0;
                }
            }
            /* libera a memória da matriz
            free (newMatrix->matrix[0]);
            free (newMatrix->matrix);
            */
        }
        
    }

    return m;
}

//MENU Tools
//MENU Tools- Função: Diz a posição da flecha no menu
void arrow(int position, int arrowPosition){
    (position == arrowPosition) ?printf("->") : printf("  ");
}

//MENU Tools- Função: Retorna a posição em que se encontra a seta.
int arrowChoice (int arrowPress,int position, int menuMax, int menuMin){
    if (arrowPress == 80 && position != menuMax){
        return  position + 1;
    }else if (arrowPress == 72 && position != menuMin){
        return position-=1;
    }else{
        return position;
    }
    return position;
}

//MENU Tools- Função: Guarda os nomes do menu (Opções do menu)
smartMatrix* matrixNamesMenu(char menuNames[], smartMatrix *m, metadataFile *mf[]){
    
    if (strcmp(menuNames, "home") == 0) {
        m = newSmartMatrix(5, m);
        strcpy(m->matrix[0], "New File");
        strcpy(m->matrix[1], "Delete File");
        strcpy(m->matrix[2], "File Properties");
        strcpy(m->matrix[3], "Defragmentation");
        strcpy(m->matrix[4], "Exit");
    }else if(strcmp(menuNames, "arquivos") == 0){
        
        if (mf)
        {   
            int j = 0;
            for (int i = 0; i < NFILES; i++) {
                
                if (mf[i]->tail > 0) j++;
            }
            m = newSmartMatrix(j+1, m);
            for (int i = 0; i < NFILES; i++) {
                
                if (mf[i]->tail > 0) strcpy(m->matrix[i], mf[i]->name);
            }
            strcpy(m->matrix[j], "Back");
        }
    }else
    {
        return m;
    }
    

    return m;
}

//HARDDISK
//HARDDISK- Função: Inicializa o HD 
void initHardDisk (hardDisk *d){
    d->freeSpace = 0;
    d->usedSpace = 0;
}

//HARDDISK- Função: Retorna o calculo da equação da quantidade de Bytes
int logicBlockCalculete1(int fileSize){
    return (fileSize %8 == 0) ?(fileSize/8) :(fileSize/8)+1;
}

//HARDDISK- Função: Calcula a quantidade de espaço no HD
void hardDiskCalculate (hardDisk *d, int fileSize){

    int logicBlock = fileSize;

    d->freeSpace = HARDDISKLEN - d->usedSpace;

    if((d->usedSpace + logicBlock*8) >= HARDDISKLEN){
        printf ("There is not enough space avalable on disk \n");
        printf ("b size: %d bytes\n", fileSize);
        return;
    } else {
        d->usedSpace = d->usedSpace + logicBlock*8;
        d->freeSpace = HARDDISKLEN - d->usedSpace;
    }
}

//HARDDISK- Função: Imprime o estado do HD
void printHardDisk(hardDisk *d){
    printf ("FreeSpace: %d bytes\n", d->freeSpace);
    printf ("usedSpace: %d bytes", d->usedSpace);
}

//Opções do Menu

//Opções do Menu/NEW FILE- Função: Aloca metadataFile
metadataFile* newMetadataFile() {

    metadataFile *mf = malloc(sizeof(metadataFile));

    if (!mf) {
        printf("\nErro de alocacao de memoria para metadataFile!\n");
        return NULL;
    } else {
        mf->name[NCOLS] = '\0'; // Atribuir o caractere nulo ao primeiro elemento da string name
        mf->size = 0;
        mf->id = 0;
        mf->head = 0;
        mf->tail = 0;
    }
    return mf;
}

//Opções do Menu/NEW FILE- Função: Diz o tamanho do começo e fim do Arquivo           
metadataFile* headAndTailCalculation(metadataFile *mf[],  int size, char name[]){
    
    char line[20];
    int count = 5;
    if (b != NULL){
        while (fgets(line, sizeof(line), b)) count++;
    }
    int id = (count/5)-1;
    strcpy(mf[id]->name, name);
    mf[id]->size = size;
    mf[id]->id = id+1;
    if (id == 0)
    {
        mf[id]->head = 0;
        mf[id]->tail = logicBlockCalculete1(mf[id]->size);
        b = fopen("Alunos.txt", "ab+"); 
        saveMetadataToFile(mf, b, id);
        return mf[NFILES];
    }

    mf[id]->head = mf[id-1]->tail+1;
    mf[id]->tail = mf[id]->head + logicBlockCalculete1(mf[id]->size);

    b = fopen("Alunos.txt", "ab+"); 
    saveMetadataToFile(mf, b, id); 
    

    return mf[NFILES];
}

//Opções do Menu/NEW FILE- Função: Checa a unicidade dos nomes dos arquivos
char sameNameCheck(char name[], metadataFile *mf[]) {
    int foundSameName;

    do {
        foundSameName = 0;

        for (int j = 0; j < NFILES; j++) {
            if (strcmp(name, mf[j]->name) == 0) {
                printf("\nErro: Arquivo com mesmo nome nao pode!");
                blockGetchar();

                char ch, nome[NCOLS] = {'\0'};
                limparTela();
                printf("\nQual o nome do novo arquivo? ");

                int i = 0;
                while ((ch = getchar()) != '\n' && i < NCOLS - 1) {
                    nome[i] = ch;
                    i++;
                }
                nome[i] = '\0';

                strcpy(name, nome);
                foundSameName = 1;
                break;
            }
        }

    } while (foundSameName);
    

    return name[NCOLS];
}

//Opções do Menu/NEW FILE- Função: Novo Arquivo
metadataFile* newFile(metadataFile *mf[], hardDisk *d){

    int sizeFile, totalFileSize = 0;
    char ch, nome[NCOLS] = {'\n'};

    printf("Qual o nome do arquivo? ");
    int i = 0;
    while ((ch = getchar()) != '\n' && i < NCOLS - 1) {
        nome[i] = ch;
        i++;
    }
    nome[NCOLS] = sameNameCheck(nome, mf);
    
    printf("Qual o Tamanho do arquivo? ");
    scanf("%d", &sizeFile);
    mf[NFILES] = headAndTailCalculation(mf, sizeFile, nome);
        
    // da o numero de (8bits)bites total do HD
    totalFileSize = mf[0]->tail;

    for (int i = 0; i < NFILES; i++)
    {
        if (mf[i]->head > 0) totalFileSize += mf[i]->head;
    }

    totalFileSize += mf[NFILES-1]->tail;

    hardDiskCalculate(d, totalFileSize);
    printHardDisk(d);
    blockGetchar();
    
    return mf[NFILES];
}

//Opções do Menu/NEW FILE- Função: imprime arquivo
void printFile(metadataFile *mf[]){
    
    if (mf)
    {   
        for (int i = 0; i < NFILES; i++){

            if (mf[i]->tail > 0){
                  
                printf("\nNome: %s\n",mf[i]->name);
                printf("I.D: %d\n",mf[i]->id);
                printf("Tamanho: %d\n",mf[i]->size);
                printf("Inicio do Arquivo: %d bytes\n",mf[i]->head);
                printf("Final do Arquivo: %d bytes\n",mf[i]->tail); 
            }

        }

    }
}

//Opções do Menu/DELETE- Função:Deletar Arquivo
void deleteMetadataFile(metadataFile *mf[], int index) {
    
    
    
    int id; 
    if (index < 0 || index >= NFILES) {
        printf("Índice inválido\n");
        return;
    }

    // Liberar a memória alocada para o elemento no índice 'index'
    free(mf[index]);

    // Reorganizar o array para manter os outros elementos
    for (int i = index; i < NFILES - 1; i++) {
        mf[i] = mf[i + 1];
    }
    for (int i = index; i < NFILES - 1; i++) {
        if (mf[i]->size == 0)
        {
            id = i;
            break;
        }
        
    }

    // Definir o último elemento do array como NULL para evitar referências inválidas
    mf[NFILES - 1]->name[NCOLS] = '\0'; // Atribuir o caractere nulo ao primeiro elemento da string name
    mf[NFILES - 1]->size = 0;
    mf[NFILES - 1]->id = 0;
    mf[NFILES - 1]->head = 0;
    mf[NFILES - 1]->tail = 0;
    updateFile(b,mf, id);
    
}

//Opções do Menu/File Properties- Função que vé os atributos da variavel mf
//Opções do Menu/Desfragmentation-
//MENU

//MENU- Função: Menu para selecionar o arquivo desejado
int menuArquivos(char menuNames[], char description[],smartMatrix *m, metadataFile *mf[], hardDisk *d){   
    
    int position = 0,arrowPress;
    
    initHardDisk(d);
    m = matrixNamesMenu(menuNames, m, mf);

    int menu_max = m->rows;

    for (int i = 0; i < m->rows; i++) {
        printf("%s\n", m->matrix[i]);
    }
    int menu_min = 0;
    do{
        limparTela();
        printf("     %s\n", description);
        printf("     ----------------------\n");
        for (int i = 0; i < menu_max; i++){
            arrow(i,position); printf("   |%-20s|\n", m->matrix[i]);
        }
        printf("     ----------------------\n");
        printf("use as setas \x18 \x19 para navegar no menu e press enter para confirmar.\n");
        
        arrowPress = nonBlockingGetchar();
        fflush(stdin);
        
        position = arrowChoice(arrowPress, position, menu_max-1, menu_min);
        
    }while(arrowPress != 13);
    
    freeSmartMatrix(m);
    

    if (position == menu_max)
    {
        return position = -1;
    }
    
    return position;
}

//MENU- Função: Menu Principal
void menu(char menuNames[], char description[],smartMatrix *m, metadataFile *mf[], hardDisk *d){   
    
    int position = 0, arrowPress, index;
    
    initHardDisk(d);
    m = matrixNamesMenu(menuNames, m, mf);

    int menu_max = m->rows;
    b = fopen("Alunos.txt", "ab+");

    for (int i = 0; i < m->rows; i++) {
        printf("%s\n", m->matrix[i]);
    }
    int menu_min = 0;
    do{
        limparTela();
        printf("     %s\n", description);
        printf("     ----------------------\n");
        for (int i = 0; i < menu_max; i++){
            arrow(i,position); printf("   |%-20s|\n", m->matrix[i]);
        }
        printf("     ----------------------\n");
        printf("use as setas \x18 \x19 para navegar no menu e press enter para confirmar.\n");
        
        arrowPress = nonBlockingGetchar();
        fflush(stdin);
        
        position = arrowChoice(arrowPress, position, menu_max-1, menu_min);
        
    }while(arrowPress != 13);

    freeSmartMatrix(m);

    switch (position)
    {
    case 0:
        if(!mf) exit(-1);
        mf[NFILES] = newFile(mf, d);
        printFile(mf);
        blockGetchar();
        menu("home", "HOME", m, mf, d);
        break;

    case 1:
        index = menuArquivos("arquivos", "DELETAR ARQUIVO:", m, mf, d);
        if (index != -1)
        {
            deleteMetadataFile(mf, index);
        }
        printFile(mf);
        blockGetchar();
        menu("home", "HOME", m, mf, d);
        break;
    
    case 2:

    case 4: 

        fclose(b);
        for (int i = 0; i < NFILES; i++) {
            free(mf[i]);
        }
        

        printf("\nO sistema esta sendo desligado\n");
        printf("\nAte a proxima!\n");
        exit(-1);
        break;
    default:

        break;
    }
}


int main(int argc, char const *argv[])
{
    smartMatrix m1; metadataFile *mf1[NFILES]; hardDisk d1;

    for (int i = 0; i < NFILES; i++) {
        mf1[i] = newMetadataFile();
        if (!mf1[i]) {
            // Tratamento de erro, caso a alocação falhe
            printf("Erro ao criar metadataFile[%d]!\n", i+1);
            return 1;
        }
    }

    b = fopen("Alunos.txt", "ab+"); 
    loadMetadataFromFile(mf1, b);

	menu("home", "HOME", &m1, mf1, &d1);
    
    return 0;
}