#include <iostream>
#include <fstream>
#include <bitset>
#include <string>
using namespace std;



void saidaArquivo(string IR, int PC, bool saida, bool vaiUm, bool A, bool B){
    ofstream arquivo("saida_etapa1.txt", ios::app);
    if (arquivo.is_open()) {
        arquivo << "IR: " << IR << ", PC: " << PC << ", A: " << A << ", B: " << B << ", saida: " << saida << ", vaiUm: " << vaiUm << endl;
        arquivo.close();
    } else {
        cerr << "Erro ao abrir o arquivo para escrita.\n";
    }
}

void Ula(bool F0, bool F1, bool ENA, bool ENB, bool INVA, bool INC, string IR, int PC){
    bool A = 1;
    bool B = 1;

    if (!ENA) A = 0;
    if (!ENB) B = 0;
    if (INVA) A = !A;


    bool AB = A & B;
    bool AorB = A | B;
    bool Bnegado = !B;

    bool saida = 0;
    if(!F0 && !F1){
        saida = AB;
    }else if (!F0 && F1){
        saida = AorB;
    }else if (F0 && !F1){
        saida = Bnegado;
    }else if (F0 && F1){
        saida = A ^ B;
    }


    bool vaiUm = (A & B) | (INC & (A ^ B));

    saidaArquivo(IR, PC, saida, vaiUm, A, B);
}


int main() {
    ifstream arquivo("programa_etapa1.txt");

    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo.\n";
        return 1;
    }

    string linha;
    int PC = 0;

    while (getline(arquivo, linha)) {
        if (linha.size() >= 6) {
            string IR = linha.substr(0, 6);

            bool F0 = linha[0] - '0';
            bool F1 = linha[1] - '0';
            bool ENA = linha[2] - '0';
            bool ENB = linha[3] - '0';
            bool INVA = linha[4] - '0';
            bool INC = linha[5] - '0';

            PC++;
            Ula(F0, F1, ENA, ENB, INVA, INC, IR, PC);
        }
    }

    arquivo.close();

    return 0;
}