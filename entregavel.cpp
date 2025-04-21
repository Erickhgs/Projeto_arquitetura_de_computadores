#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>
#include <sstream>
#include <stdexcept>
#include "Mic.h" // reutiliza a classe Mic1 do seu projeto existente

using namespace std;

// Função auxiliar para adicionar uma microinstrução a uma lista
void adicionarInstrucao(vector<string>& lista, const string& instrucao) {
    lista.push_back(instrucao);
}

// Traduz uma instrução IJVM para microinstruções
vector<string> traduzInstrucaoIJVM(const string& tipo, const string& argumento) {
    vector<string> microinstr;

    if (tipo == "ILOAD") {
        adicionarInstrucao(microinstr, "00110100100000000000101"); // H = LV
        int x = stoi(argumento);
        for (int i = 0; i < x; i++) {
            adicionarInstrucao(microinstr, "00111001100000000000000"); // H = H + 1
        }
        adicionarInstrucao(microinstr, "00111000000000001010000"); // MAR = H; rd
        adicionarInstrucao(microinstr, "00110101000001001100100"); // MAR = SP = SP+1; wr
        adicionarInstrucao(microinstr, "00110100001000000000000"); // TOS = MDR
    }
    else if (tipo == "DUP") {
        adicionarInstrucao(microinstr, "00110101000001001000100"); // MAR = SP = SP+1
        adicionarInstrucao(microinstr, "00110100000000010100111"); // MDR = TOS; wr
    }
    else if (tipo == "BIPUSH") {
        adicionarInstrucao(microinstr, "00110101000001001000100"); // SP = MAR = SP+1

        // Parte especial: fetch para MBR, bits 8 primeiros são o byte a carregar
        string fetchInstrucao = argumento + "000000000110000";

        if (fetchInstrucao.size() != 23) {
            throw runtime_error("Erro: argumento BIPUSH deve ter 8 bits.");
        }
        adicionarInstrucao(microinstr, fetchInstrucao);

        adicionarInstrucao(microinstr, "00001000001000010101000"); // MDR = TOS = H; wr
    }
    else {
        throw runtime_error("Instrução IJVM desconhecida: " + tipo);
    }

    return microinstr;
}

int main() {
    ifstream arq("teste.txt");
    ofstream microout("microinstrucoes_etapa3_tarefa2.txt");

    if (!arq.is_open()) {
        cerr << "Erro ao abrir arquivos de entrada.\n";
        return 1;
    }
    if (!microout.is_open()) {
        cerr << "Erro ao abrir arquivos de saída.\n";
        return 1;
    }

    string linha;
    while (getline(arq, linha)) {
        if (linha.empty()) continue;

        stringstream ss(linha);
        string instrucao, argumento;
        ss >> instrucao >> argumento;

        vector<string> microprograma = traduzInstrucaoIJVM(instrucao, argumento);
        for (const string& mi : microprograma) {
            microout << mi << endl;
        }
    }

    arq.close();
    microout.close();

    // Executa a Mic-1 com as microinstruções geradas
    ifstream microin("microinstrucoes_etapa3_tarefa2.txt");
    ofstream log("saida_etapa_final.txt", ios::trunc);
    Mic1 mic;
    mic.carregarRegistradores("registradores_etapa3_tarefa1.txt");
    mic.imprimirEstadoInicial(log);

    int ciclo = 1;
    int instrucaoContador = 1;
    ifstream original("teste.txt");
    string linhaOriginal;
    vector<int> instrucoesTamanho;

    while (getline(original, linhaOriginal)) {
        if (linhaOriginal.empty()) continue;
        stringstream ss(linhaOriginal);
        string instrucao, argumento;
        ss >> instrucao >> argumento;
        vector<string> microinstr = traduzInstrucaoIJVM(instrucao, argumento);
        instrucoesTamanho.push_back(microinstr.size());
    }
    original.close();

    string instr;
    int microInstrRestantes = 0;
    size_t idxInstrucao = 0;

    while (getline(microin, instr)) {
        if (instr.size() != 23) continue;

        if (microInstrRestantes == 0 && idxInstrucao < instrucoesTamanho.size()) {
            log << "==================== Instrução " << instrucaoContador++ << " ====================\n";
            microInstrRestantes = instrucoesTamanho[idxInstrucao++];
        }

        mic.executarInstrucao(instr, ciclo++, log);
        microInstrRestantes--;
    }

    log << "=====================================================\n";
    log << "Cycle " << ciclo << "\nNo more lines, EOP.\n";

    microin.close();
    log.close();
    return 0;
}