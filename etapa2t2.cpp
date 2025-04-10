#include <iostream>
#include <fstream>
#include <bitset>
#include <string>
#include <algorithm>
#include <exception>
using namespace std;

// void saidaArquivo(string IR, int PC, bitset<32> A, bitset<32> B, bitset<32> saida, bitset<32> saidaD, bool N, bool Z, bool vaiUm) {
//     ofstream arquivo("saida_etapa2_tarefa2.txt", ios::app);
//     if (arquivo.is_open()) {
//         if (PC == 1) {
//             arquivo << "b = " << B << endl;
//             arquivo << "a = " << A << endl;
//             arquivo << "\nStart of Program\n";
//             arquivo << "============================================================" << endl;
//         }
        
//         arquivo << "Cycle " << PC << "\n\n";
//         arquivo << "PC = " << PC << endl;
//         arquivo << "IR = " << IR << endl;
//         arquivo << "b = " << B << endl;
//         arquivo << "a = " << A << endl;
//         arquivo << "s = " << saida << endl;
//         arquivo << "sd = " << saidaD << endl;
//         arquivo << "n = " << N << endl;
//         arquivo << "z = " << Z << endl;
//         arquivo << "co = " << vaiUm << endl;
//         arquivo << "============================================================" << endl;
        
//     } else {
//         cerr << "Erro ao abrir o arquivo para escrita.\n";
//     }
// }
    

string Ula(string IR, bitset<32> A, bitset<32> B) {
           
    bool SLL8 = IR[0] - '0';
    bool SRA1 = IR[1] - '0';
    bool F0 = IR[2] - '0';
    bool F1 = IR[3] - '0';
    bool ENA = IR[4] - '0';
    bool ENB = IR[5] - '0';
    bool INVA = IR[6] - '0';
    bool INC = IR[7] - '0';

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

    
    //saidaArquivo(IR, A_operando, B_operando, saida, saidaD, N, Z, vaiUm);
    return saidaD.to_string();

}

class Caminho_de_dados{
private:
    string ir;
    int PC;
    bitset<32> registrador_MAR; 
    bitset<32> registrador_MDR;
    bitset<32> registrador_PC; 
    bitset<8> registrador_MBR; 
    bitset<32> registrador_SP; 
    bitset<32> registrador_LV; 
    bitset<32> registrador_CPP; 
    bitset<32> registrador_TOS; 
    bitset<32> registrador_OPC; 
    bitset<32> registrador_H;
public:

Caminho_de_dados(){
    registrador_MAR = bitset<32>("00000000000000000000000000000000");
    registrador_MDR = bitset<32>("00000000000000000000000000000000");
    registrador_PC = bitset<32>("00000000000000000000000000000000");
    registrador_MBR = bitset<8>("10000001");
    registrador_SP = bitset<32>("00000000000000000000000000000000");
    registrador_LV = bitset<32>("00000000000000000000000000000000");
    registrador_CPP = bitset<32>("00000000000000000000000000000000");
    registrador_TOS = bitset<32>("00000000000000000000000000000010");
    registrador_OPC = bitset<32>("00000000000000000000000000000000");
    registrador_H = bitset<32>("00000000000000000000000000000001");
    PC = 1;
}

~Caminho_de_dados(){
}

void set_ir(string instrucao){
    ir = instrucao;
}

string get_b_bus(){
    string aux = ir.substr(17, 4);
    if(aux == "0000") {
        return "mbr";
    } else if (aux == "0001") {
        return "pc";
    } else if (aux == "0010") {
        return "mbr";
    } else if (aux == "0011") {
        return "mar";
    } else if (aux == "0100") {
        return "sp";
    } else if (aux == "0101") {
        return "lv";
    } else if (aux == "0110") {
        return "cpp";
    } else if (aux == "0111") {
        return "tos";
    } else if (aux == "1000") {
        return "opc";
    } else {
        throw runtime_error("Código inválido para barramento B.");
    }
}

string get_barramentoB(string codigo){
    if(codigo == "0000") {
        return registrador_MDR.to_string();
    } else if (codigo == "0001") {
        return registrador_PC.to_string();
    } else if (codigo == "0010") {
        string complemento = "000000000000000000000000" + registrador_MBR.to_string();
        return complemento;
    } else if (codigo == "0011") {
        return registrador_MAR.to_string();
    } else if (codigo == "0100") {
        return registrador_SP.to_string();
    } else if (codigo == "0101") {
        return registrador_LV.to_string();
    } else if (codigo == "0110") {
        return registrador_CPP.to_string();
    } else if (codigo == "0111") {
        return registrador_TOS.to_string();
    } else if (codigo == "1000") {
        return registrador_OPC.to_string();
    } else {
        throw runtime_error("Código inválido para barramento B.");
    }
}

string get_c_bus(){
    string aux = ir.substr(8, 9);

    string cbus = "";
    
    if(aux[0]== '1') {
        cbus += "h ";
    }
    if (aux[1]=='1') {
        cbus += "opc ";
    }
    if (aux[2]=='1') {
        cbus += "tos ";
    }
    if (aux[3]=='1') {
        cbus += "cpp ";
    }
    if (aux[4]=='1') {
        cbus += "lv ";
    }
    if (aux[5]=='1') {
        cbus += "sp ";
    }
    if (aux[6]=='1') {
        cbus += "pc ";
    }
    if (aux[7]=='1') {
        cbus += "mdr ";
    }
    if (aux[8]=='1') {
        cbus += "mar ";
    }
    cbus += ".";

    return cbus;
}

void set_barramentoC(string codigo, string saida_ula){
    
    if(codigo[0]== '1') {
        registrador_H = bitset<32>(saida_ula);
    }
    if (codigo[1]=='1') {
        registrador_OPC = bitset<32>(saida_ula);
    }
    if (codigo[2]=='1') {
        registrador_TOS = bitset<32>(saida_ula);
    }
    if (codigo[3]=='1') {
        registrador_CPP = bitset<32>(saida_ula);
    }
    if (codigo[4]=='1') {
    registrador_LV = bitset<32>(saida_ula);
    }
    if (codigo[5]=='1') {
        registrador_SP = bitset<32>(saida_ula);
    }
    if (codigo[6]=='1') {
        registrador_PC = bitset<32>(saida_ula);
    }
    if (codigo[7]=='1') {
        registrador_MDR = bitset<32>(saida_ula);
    }
    if (codigo[8]=='1') {
        registrador_MAR = bitset<32>(saida_ula);
    }
}

void operacaoCaminhoDados(){
    string controle_ULA = ir.substr(0, 8);
    string barramentoC = ir.substr(8, 9);
    string barramentoB = ir.substr(17, 4);

    bitset<32> A_operando = registrador_H;
    bitset<32> B_operando = bitset<32>(get_barramentoB(barramentoB));

    string saida_ula = Ula(controle_ULA, A_operando, B_operando);
    cout << saida_ula << endl;
   

    set_barramentoC(barramentoC, saida_ula);
    PC++;
}

void imprimirBarramentos(){
    cout << "MAR = " << registrador_MAR << endl;
    cout << "MDR = " << registrador_MDR << endl;
    cout << "PC = " << registrador_PC << endl;
    cout << "MBR = " << registrador_MBR << endl;
    cout << "SP = " << registrador_SP << endl;
    cout << "LV = " << registrador_LV << endl;
    cout << "CPP = " << registrador_CPP << endl;
    cout << "TOS = " << registrador_TOS << endl;
    cout << "OPC = " << registrador_OPC << endl;
    cout << "H = " << registrador_H << endl;
}

void regitrarArquivoInicial(){
    ofstream arquivo("saida_etapa2_tarefa2.txt", ios::app);
    if (arquivo.is_open()) {

        arquivo << "\n============================================================" << endl;
        arquivo << "> Initial register states" << endl;
        arquivo << "MAR = " << registrador_MAR << endl;
        arquivo << "MDR = " << registrador_MDR << endl;
        arquivo << "PC = " << registrador_PC << endl;
        arquivo << "MBR = " << registrador_MBR << endl;
        arquivo << "SP = " << registrador_SP << endl;
        arquivo << "LV = " << registrador_LV << endl;
        arquivo << "CPP = " << registrador_CPP << endl;
        arquivo << "TOS = " << registrador_TOS << endl;
        arquivo << "OPC = " << registrador_OPC << endl;
        arquivo << "H = " << registrador_H << endl;
        arquivo << "\n============================================================" << endl;
        arquivo << "Start of Program" << endl;
        //arquivo << "============================================================" << endl;

    } else {
        cerr << "Erro ao abrir o arquivo para escrita.\n";
    }
}


void registrarArquivoAntes(){
    ofstream arquivo("saida_etapa2_tarefa2.txt", ios::app);
    if (arquivo.is_open()) {

        // if(PC == 1) {
        // arquivo << "> Initial Registers" << endl;
        // arquivo << "MAR = " << registrador_MAR << endl;
        // arquivo << "MDR = " << registrador_MDR << endl;
        // arquivo << "PC = " << registrador_PC << endl;
        // arquivo << "MBR = " << registrador_MBR << endl;
        // arquivo << "SP = " << registrador_SP << endl;
        // arquivo << "LV = " << registrador_LV << endl;
        // arquivo << "CPP = " << registrador_CPP << endl;
        // arquivo << "TOS = " << registrador_TOS << endl;
        // arquivo << "OPC = " << registrador_OPC << endl;
        // arquivo << "H = " << registrador_H << endl;
        // arquivo << "\n============================================================" << endl;
        // arquivo << "Start of Program" << endl;
        // }

        arquivo << "============================================================" << endl;
        arquivo << "Cycle " << PC << "\n";
        arquivo << "ir = " << ir.substr(0, 8) << " " << ir.substr(8, 9) << " "<< ir.substr(17, 4) << endl;

        //arquivo << "\nb_bus = " << get_b_bus << endl;
        //arquivo << "c_bus = " << get_c_bus << "\n\n";

        arquivo << "> Registers before instruction" << endl;
        arquivo << "MAR = " << registrador_MAR << endl;
        arquivo << "MDR = " << registrador_MDR << endl;
        arquivo << "PC = " << registrador_PC << endl;
        arquivo << "MBR = " << registrador_MBR << endl;
        arquivo << "SP = " << registrador_SP << endl;
        arquivo << "LV = " << registrador_LV << endl;
        arquivo << "CPP = " << registrador_CPP << endl;
        arquivo << "TOS = " << registrador_TOS << endl;
        arquivo << "OPC = " << registrador_OPC << endl;
        arquivo << "H = " << registrador_H << endl;
    } else {
        cerr << "Erro ao abrir o arquivo para escrita.\n";
    }
}

void registrarArquivoDepois(){
    ofstream arquivo("saida_etapa2_tarefa2.txt", ios::app);
    if (arquivo.is_open()) {
        arquivo << "\n> Registers after instruction" << endl;
        arquivo << "MAR = " << registrador_MAR << endl;
        arquivo << "MDR = " << registrador_MDR << endl;
        arquivo << "PC = " << registrador_PC << endl;
        arquivo << "MBR = " << registrador_MBR << endl;
        arquivo << "SP = " << registrador_SP << endl;
        arquivo << "LV = " << registrador_LV << endl;
        arquivo << "CPP = " << registrador_CPP << endl;
        arquivo << "TOS = " << registrador_TOS << endl;
        arquivo << "OPC = " << registrador_OPC << endl;
        arquivo << "H = " << registrador_H << endl;
    } else {
        cerr << "Erro ao abrir o arquivo para escrita.\n";
    }
}
};


int main(){
    Caminho_de_dados caminho;

    ifstream arquivo("programa_etapa2_tarefa2.txt");
    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo.\n";
        return 1;
    }

    ofstream arquivo_saida("saida_etapa2_tarefa2.txt", ios::app);

    string linha;

    int pc = 0;

    while(getline(arquivo, linha)){
            arquivo_saida << linha << endl;
    }
    arquivo.close();
    
    
    string linha2;

    ifstream arquivo2("programa_etapa2_tarefa2.txt");
    if (!arquivo2.is_open()) {
        cerr << "Erro ao abrir o arquivo2.\n";
        return 1;
    }

    caminho.regitrarArquivoInicial();
    while(getline(arquivo2, linha2)){
        if(linha.size() >= 21){


            try{
                caminho.set_ir(linha2);
                caminho.registrarArquivoAntes();
                caminho.operacaoCaminhoDados();
                caminho.registrarArquivoDepois();

                }catch (const runtime_error& e) {
                cout << "Erro: " << e.what() << endl;
            }
        }
        pc++;
    }

    ofstream arquivo_saida_final("saida_etapa2_tarefa2.txt", ios::app);
    arquivo_saida_final << "\n============================================================" << endl;
    arquivo_saida_final << "Cycle " << pc;
    arquivo_saida_final << "\nNo more lines, EOP." << endl;
    arquivo_saida_final.close();
    arquivo_saida.close();
    
    return 0;
}

