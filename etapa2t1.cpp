#include <iostream>
#include <fstream>
#include <bitset>
#include <string>
#include <algorithm>
#include <exception>
using namespace std;

void saidaArquivo(string IR, int PC, bitset<32> A, bitset<32> B, bitset<32> saida, bitset<32> saidaD, bool N, bool Z, bool vaiUm) {
    ofstream arquivo("saida_etapa2_tarefa1.txt", ios::app);
    if (arquivo.is_open()) {
        if (PC == 1) {
            arquivo << "b = " << B << endl;
            arquivo << "a = " << A << endl;
            arquivo << "\nStart of Program\n";
            arquivo << "============================================================" << endl;
        }
        
        arquivo << "Cycle " << PC << "\n\n";
        arquivo << "PC = " << PC << endl;
        arquivo << "IR = " << IR << endl;
        arquivo << "b = " << B << endl;
        arquivo << "a = " << A << endl;
        arquivo << "s = " << saida << endl;
        arquivo << "sd = " << saidaD << endl;
        arquivo << "n = " << N << endl;
        arquivo << "z = " << Z << endl;
        arquivo << "co = " << vaiUm << endl;
        arquivo << "============================================================" << endl;
        
    } else {
        cerr << "Erro ao abrir o arquivo para escrita.\n";
    }
}
    

void Ula(bool SLL8, bool SRA1, bool F0, bool F1, bool ENA, bool ENB, bool INVA, bool INC, string IR, int PC) {
    
    static bitset<32> A(0x00000001);     
    static bitset<32> B(0x80000000);          
    
    bitset<32> A_operando = A;
    bitset<32> B_operando = B; 

    if (!ENA) {
    A_operando.reset();  // Se ENA=0, A_operando=0
    } else if (ENA) {
    A_operando = A; // Se ENA=1, A_operando=A
    }
    
    if (!ENB){  // Se ENB=0, B_operando=0
    B_operando.reset();
    } else if (ENB){
    B_operando = B; // Se ENB=1, B_operando=B
    }

    if (INVA){
        A_operando.flip();
    }else if (!ENA && INVA){
        A_operando.reset(); // Se ENA=0 e INVA=1, A_operando=0
    }
    bitset<32> saida;
    bool vaiUm = 0;
    if (!F0 && !F1) {
        saida =  A_operando & B_operando;  // AND
        vaiUm = 0; // Não há carry out para AND
    } else if (!F0 && F1) {
        saida = A_operando | B_operando;      // OR
        vaiUm = 0; // Não há carry out para OR
    } else if (F0 && !F1) {
        saida = ~B_operando;                  // NOT B
        vaiUm = 0; // Não há carry out para NOT
    }  else if (F0 && F1) {
        unsigned long long soma = static_cast<unsigned long long>(A_operando.to_ulong()) +
                                  static_cast<unsigned long long>(B_operando.to_ulong()) +
                                  static_cast<unsigned long long>(INC);
        
        saida = bitset<32>(soma & 0xFFFFFFFF);
        vaiUm = soma >> 32;
    }

    bool N = 0, Z = 0;
    bitset<32> saidaD;

    if(!SLL8 && !SRA1) {
        saidaD = saida;
    }
    else if(!SLL8 && SRA1) {
        string aux;
        bool msb = saida[31];
        string msbAux = msb ? "1" : "0";
        string saidaStr = saida.to_string();
        saidaStr = msbAux + saidaStr.substr(0, 31);
        saidaD = bitset<32>(saidaStr);

    }
    else if(SLL8 && !SRA1) {
        saidaD = saida << 8;
    }
    else if(SLL8 && SRA1) {
        throw runtime_error("ERRO");
    }

    Z = (saidaD.none());
    N = saidaD[31];

    
    saidaArquivo(IR, PC, A_operando, B_operando, saida, saidaD, N, Z, vaiUm);

}

int main() {
    // Limpa o arquivo de saída antes de começar
    ofstream clear_file("saida_etapa2_tarefa1.txt");
    clear_file.close();

    ifstream arquivo("programa_etapa2_tarefa1.txt");

    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo.\n";
        return 1;
    }

    string linha;
    int PC = 0;

    ofstream arquivo_saida("saida_etapa2_tarefa1.txt", ios::app);

    while (getline(arquivo, linha)) {

        if (linha.size() >= 8) {
            string IR = linha.substr(0, 8);

            bool SLL8 = linha[0] - '0';
            bool SRA1 = linha[1] - '0';
            bool F0 = linha[2] - '0';
            bool F1 = linha[3] - '0';
            bool ENA = linha[4] - '0';
            bool ENB = linha[5] - '0';
            bool INVA = linha[6] - '0';
            bool INC = linha[7] - '0';

            PC++;
            try
            {
                Ula(SLL8, SRA1, F0, F1, ENA, ENB, INVA, INC, IR, PC);
            }
            catch(const exception& e)
            {
                arquivo_saida << "Cycle " << PC << "\n\n";
                arquivo_saida << "PC = " << PC << endl;
                arquivo_saida << "IR = " << IR << endl;
                arquivo_saida << "> Error, invalid control signals." << endl;
                arquivo_saida << "============================================================" << endl;
            }           
        } 
    }

    PC++;

    arquivo_saida << "Cycle " << PC << "\n\n";
    arquivo_saida << "PC = " << PC << endl;
    arquivo_saida << "> Line is empty, EOP." << endl;

    arquivo.close();
    arquivo_saida.close();

    return 0;
}