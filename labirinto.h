#pragma once

typedef struct{
    int x,y;
}point;

typedef struct{
    int steps;
    point * P;  //vetor contendo os pontos referentes aos passos
}path;

class maze{

    typedef struct{
        int parent_i, parent_j;
        double f,g,h;
    }node;

    private:
        path PATH;
        bool isValid(int i, int j);
        bool isUnBlocked(char ** grid, int i,int j);
        double H(int i, int j, point goal);
        bool IsDestination(int x, int y);
        path tracePath (node ** NodeInfo);
        int rows;           
        int columns;
        point start;
        point goal;       
        char ** M;                                  //Matriz dinâmica que guarda a forma do labirinto
        path AStar();
    public:
        maze(int rows, int lines, double WallDensity);  //Construtor aleatório
        maze(char*);                                    //Construtor a partir do arquivo
        maze(const maze*);                              //Construtor cópia
        ~maze();
        path solve(int alg);                            //Resolve o labirinto usando um algoritmo específico
        void mazeToFile(char * FileName);               //Salva o labirinto em arquivo
        void print();                                   //Imprime labirinto na tela
        void print(path p);                             //imprime o caminho na tela
};


