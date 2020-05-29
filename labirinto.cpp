#include "labirinto.h"

#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <list>
#include<bits/stdc++.h>

using namespace std;

point pnt(int x, int y){
    point ret;
    ret.x = x;
    ret.y = y;
    return ret;
}

void maze::print(path p){
    int i;
    printf("[");
    for(i=0;i<p.steps-1;i++)
        printf("(%d,%d),",p.P[i].x,p.P[i].y);
    printf("(%d,%d)]\n",p.P[i].x,p.P[i].y);
}
void maze::print(){//Imprime o labirinto 
    int i,j;
    for(j=0;j<this->rows;j++){
        for(i=0;i<this->columns;i++)
            printf("%c",this->M[j][i]);
        printf("\n");
    }
}

void maze::print(maze m, path p){
    char M[m.rows][m.columns];
    int i,j;
    for(i=0;i<m.rows;i++){
        for(j=0;j<m.columns;j++){
            M[i][j] = m.M[i][j];
        }
    }
    for(i=0;i<p.steps;i++)
        M[p.P[i].x][p.P[i].y]='@';
    
    for(i=0;i<m.rows;i++){
        for(j=0;j<m.columns;j++)
            printf("%c",M[i][j]);
    printf("\n");
    }
    printf("\n");

}
path maze::solve(int alg){
    return Search(alg);
}

void maze::mazeToFile(const char * FileName){
    FILE * fp;
    int x, y;
    
    //Abrir arquivo para gravar dimensões do labirinto
    fp = fopen(FileName,"w+");
    if(!fp){
        printf("Arquivo não encontrado!\n");
        exit(0);
    }

    fprintf(fp, "%d %d\n",this->rows, this->columns);
    
    //Gravar labirinto em disco
    for(x=0;x<this->rows;x++){
        for(y=0;y<this->columns;y++){
            fprintf(fp,"%c",this->M[x][y]);
            if(this->M[x][y]=='#') {
                this->start.x = x;
                this->start.y = y;
            }
            if(this->M[x][y]=='$') {
                this->goal.x = x;
                this->goal.y = y;
            }
        }
        fprintf(fp,"\n");
    }
    fclose(fp);
}
char ** DinArray(int rows, int columns){//Criar array dinâmico para conter o labirinto
    int x, y;
    char ** A;
    A = (char**)malloc(sizeof(char*)*rows);

    for(x=0;x<rows;x++){
        A[x] = (char*)malloc(sizeof(char)*columns);
    }
    return A;
}

void maze::renew(double WallDensity){
    int x, y;
    for(x=0;x<this->rows;x++){
        for(y=0;y<this->columns;y++){
            this->M[x][y] = ((double)rand()/RAND_MAX) < WallDensity? '-':'*';
        }
    }
    this->start.x = 0;
    this->start.y =rand()%columns;
    this->goal.x = rows-1;
    this->goal.y = rand()%rows;
    this->M[this->start.x ][this->start.y]='#';
    this->M[this->goal.x ][this->goal.y]='$';
    
}

maze::maze(int rows, int columns, double WallDensity){//Cria labirinto com densidade de paresdes específica
    int x, y;
    this->M = DinArray(rows,columns);
    this->rows = rows;
    this->columns = columns;
    PATH.P=NULL;
    PATH.steps=-1;
    //criar
    for(x=0;x<this->rows;x++){
        for(y=0;y<this->columns;y++){
            this->M[x][y] = ((double)rand()/RAND_MAX) < WallDensity? '-':'*'    ;
        }
    }
    this->start.x = 0;
    this->start.y = rand()%columns;
    this->goal.x = rows-1;
    this->goal.y = rand()%rows;
    this->M[this->start.x ][this->start.y]='#';
    this->M[this->goal.x ][this->goal.y]='$';
}

maze::maze(char * FileName){//Construtor a partir de arquivo
    FILE * fp;
    int x, y;
    PATH.P=NULL;
    PATH.steps=-1;
    //Abrir arquivo para ler dimensões do labirinto
    fp = fopen(FileName,"r+");
    if(!fp){
        printf("Arquivo inexistente!\n");
        exit(0);
    }

    fscanf(fp, "%d %d\n",&this->rows, &this->columns);
    

    //Criar array dinâmico para conter o labirinto
    this->M = DinArray(this->rows,this->columns);

    //Ler do arquivo o labirinto
    for(x=0;x<this->rows;x++){
        for(y=0;y<this->columns;y++){
            fscanf(fp,"%c",&this->M[x][y]);
            if(this->M[x][y]=='#') this->start.x = x;
            if(this->M[x][y]=='#') this->start.y = y;
            if(this->M[x][y]=='$') this->goal.x = x;
            if(this->M[x][y]=='$') this->goal.y = y;
        }
        fscanf(fp,"\n");
    }
    fclose(fp);
   
}

maze::maze(const maze* mz){//Construtor cópia
    int x, y;
    *this = *mz;
    PATH.P=NULL;
    PATH.steps=-1;
    this->M = DinArray(this->rows,this->columns);

    //copiar
    for(x=0;x<this->rows;x++){
        for(y=0;y<this->columns;y++){
            this->M[x][y] = mz->M[x][y];
        }
    }
}

maze::~maze(){              //Destrutor
    int x;//Libera heap usado para armazenar o labirinto
    
    for(x=0;x<rows;x++)
        free(M[x]);
    
    free(M);
}

typedef pair<double, pair<int, int> > pPair;

bool maze::isValid(point p){
    return (p.x >= 0) && (p.x < this->rows) && (p.y >= 0) && (p.y < this->columns);
}

bool maze::isUnBlocked(char ** grid, point p){
    return (grid[p.x][p.y]=='-'?false:true);
}

double maze::H(int i, int j, point goal){ //Calcula a distância entre o ponto {i,j} e a saida do labirinto
    return ((double)sqrt ((i - goal.x)*(i - goal.x) + (j - goal.y)*(j - goal.y))); 
}
bool maze::IsDestination(point p){
    return (this->goal.x == p.x && this->goal.y == p.y);
}
path maze::tracePath (node ** NodeInfo  ){    
    int row = this->goal.x;
    int col = this->goal.y; 
    int i=0,steps=0;
    point * n;
    stack<point*> Path; 
    steps = 0; 
    while (!(NodeInfo[row][col].parent_i == row  && NodeInfo[row][col].parent_j == col )){ 
        n = (point*)malloc(sizeof(point));
        n->x = row;
        n->y = col;
        Path.push (n);
        steps++;
        int temp_row = NodeInfo[row][col].parent_i; 
        int temp_col = NodeInfo[row][col].parent_j; 
        row = temp_row; 
        col = temp_col; 
    }
    n = (point*)malloc(sizeof(point));
    n->x = row;
    n->y = col;
    Path.push (n);
    steps++;
    
    if(PATH.steps < steps){
        PATH.steps = steps;
        PATH.P = (point*)realloc(PATH.P, sizeof(point)*PATH.steps);//reserva na heap espaço para o caminho
    }
    
    
    while (!Path.empty()){ 
        n = Path.top(); 
        Path.pop();
        //this->M[n->x][n->y]='@';//Criar função que, em vez de fazer isso, imprima sem precisar alterar o labirinto
        PATH.P[i++]=*n;//isso copia o valor, não o endereço
        //printf("(%d,%d)",n->x,n->y);//usa sprintf envez
        //if(!Path.empty())
        //    printf(",");
        free(n);
    }

    
    return PATH;
}

void maze::criteria(int alg, int i, int j, point neighbor, set<pPair> * open){
    double gNew, hNew, fNew=0.0; 
    node * naux = &NodeInfo[neighbor.x][neighbor.y];
    pPair p;
    //agora é por um switch bem localizado para selecionar apenas o critério 
    switch(alg){
        //Buscas informadas:
        case 0://AStar, o critério é fNew = gNew + hNew;

            gNew = NodeInfo[i][j].g + 1.0; 
            hNew = H(neighbor.x, neighbor.y, this->goal); 
            fNew = gNew + hNew;
            
            if (naux->f >= fNew){ 
                open->insert( make_pair(fNew, make_pair(neighbor.x, neighbor.y))); 
                // Atualiza as informações desta célula 
                naux->f = fNew; 
                naux->g = gNew; 
                naux->h = hNew; 
                naux->parent_i = i; 
                naux->parent_j = j; 
            } 
        break;
        case 1://hill climbing otimiza uma solução  já encontrada
            printf("algoritmo ainda não implementado!");
            exit(0);
        break;


        case 2://best-first quâo distante a solução está do nó atual?
            fNew = H(neighbor.x, neighbor.y,goal);
            naux->parent_i = i; 
            naux->parent_j = j; 
            open->insert(make_pair(fNew, make_pair(neighbor.x, neighbor.y)));
        break;

        //Buscas não informadas
        case 4://Busca em profundidade, basta usar a lista como pilha
            p = *open->begin();
            fNew = p.first - 1;
            naux->parent_i = i; 
            naux->parent_j = j; 
            open->insert(make_pair(fNew, make_pair(neighbor.x, neighbor.y)));
        break;
        
        case 3://Busca em largura, basta usar a lista como fila
            p = *open->end();
            fNew = p.first + 1;
            naux->parent_i = i; 
            naux->parent_j = j; 
            open->insert(make_pair(fNew, make_pair(neighbor.x, neighbor.y)));
        break;
        default:
            printf("O algoritmo %d não existe!\n",alg);
            exit(0);
    }

}

path maze::Search(int alg){//Resolve o labirinto baseado em algum algoritmo especifico
    int i,j,k;
    point neighborhood[4];
    path ret;
    ret.steps = -1;//indica que não tem caminho
    std::list <int> a;
    bool ** closed;
    
    closed = (bool**)malloc(sizeof(bool*)*this->rows);
    for(i=0;i<this->rows;i++)
        closed[i]=(bool*)malloc(sizeof(bool)*this->columns);
    
    node * naux;

    NodeInfo = (node**)malloc(sizeof(node*)*this->rows);
    for(i=0;i<this->rows;i++)
        NodeInfo[i]=(node*)malloc(sizeof(node)*this->columns);

    for (i=0; i<this->rows; i++) { //Inicializa NodeInfo
        for (j=0; j<this->columns; j++){ 
            NodeInfo[i][j].f = FLT_MAX; 
            NodeInfo[i][j].g = FLT_MAX; 
            NodeInfo[i][j].h = FLT_MAX; 
            NodeInfo[i][j].parent_i = -1; 
            NodeInfo[i][j].parent_j = -1; 
        } 
    } 

    i = this->start.x;
    j = this->start.y;

    NodeInfo[i][j].f = 0.0; 
    NodeInfo[i][j].g = 0.0; 
    NodeInfo[i][j].h = 0.0; 
    NodeInfo[i][j].parent_i = i; 
    NodeInfo[i][j].parent_j = j; 

    set<pPair> open; 
    open.insert(make_pair(0.0, make_pair (i, j))); 
  
 
    bool foundDest = false; 
  
    while (!open.empty() && !foundDest ){
        pPair p = *open.begin();//Aqui o nó de maior F é pego da lista
        
        open.erase(open.begin()); 
  
        // Add this vertex to the closed list 
        i = p.second.first; 
        j = p.second.second; 
        closed[i][j] = true;

        
        neighborhood[0] = pnt(i-1,j);
        neighborhood[1] = pnt(i+1,j);
        neighborhood[2] = pnt(i,j-1);
        neighborhood[3] = pnt(i,j+1);

        for(k=0;k<4 && !foundDest ;k++)
            if (isValid(neighborhood[k])){
                naux = &NodeInfo[neighborhood[k].x][neighborhood[k].y];
                if (IsDestination(neighborhood[k])){ //A saída foi alcançada
                    naux->parent_i = i; 
                    naux->parent_j = j; 
                    foundDest = true;
                    ret = tracePath (NodeInfo);
                }   //Caso o nó seja alcançável 
                else if (!closed[neighborhood[k].x][neighborhood[k].y] &&  isUnBlocked(this->M, neighborhood[k])){ 
                    //A ideia é inserir o nó na lista aplicando o critério
                    //referente à cada algoritmo
                    criteria(alg, i, j ,neighborhood[k], &open);
                } 
            }
        

       
    }

    //desalocar tudo o que foi usad

    //Liberar closed list
    for(i=0;i<this->rows;i++)
        free(closed[i]);
    free(closed);
    
    //Liberar NodeInfo
    for(i=0;i<this->rows;i++)
        free(NodeInfo[i]);
    free(NodeInfo);

    //if(!foundDest)printf(".");
    
    return ret;
}

