#ifndef MIC_H
#define MIC_H

#pragma once

#include <iostream>
#include <bitset>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

class Mic1 {
    private:
        bitset<32> H, OPC, TOS, CPP, LV, SP, PC, MDR, MAR;
        bitset<8> MBR;
        vector<bitset<32>> memoria;
        string IR;
    public:
        Mic1() {
            memoria.resize(16);
            memoria[4] = bitset<32>("00000000000000000000000000000010"); // mem[4] = 2
        }
        void carregarRegistradores(const string& caminho) {
            ifstream arq(caminho);
            if (!arq.is_open()) {
                cerr << "Erro ao abrir o arquivo de registradores.\n";
                return;
            }
            string nome, igual, valor;
            while (arq >> nome >> igual >> valor) {
                if (nome == "mar") MAR = bitset<32>(valor);
                else if (nome == "mdr") MDR = bitset<32>(valor);
                else if (nome == "pc") PC = bitset<32>(valor);
                else if (nome == "mbr") MBR = bitset<8>(valor);
                else if (nome == "sp") SP = bitset<32>(valor);
                else if (nome == "lv") LV = bitset<32>(valor);
                else if (nome == "cpp") CPP = bitset<32>(valor);
                else if (nome == "tos") TOS = bitset<32>(valor);
                else if (nome == "opc") OPC = bitset<32>(valor);
                else if (nome == "h") H = bitset<32>(valor);
            }
            arq.close();
        }
        void imprimirEstadoInicial(ofstream& log) {
            log << "============================================================\n";
            log << "Initial memory state\n*******************************\n";
            for (auto& m : memoria)
            log << m << "\n";
            log << "*******************************\n";
            log << "Initial register state\n*******************************\n";
            imprimirRegistradores(log);
            log << "============================================================\n";
            log << "Start of Program\n";
            log << "============================================================\n";
        }
        void imprimirRegistradores(ofstream& log) {
            log << "mar = " << MAR << "\n";
            log << "mdr = " << MDR << "\n";
            log << "pc = " << PC << "\n";
            log << "mbr = " << MBR << "\n";
            log << "sp = " << SP << "\n";
            log << "lv = " << LV << "\n";
            log << "cpp = " << CPP << "\n";
            log << "tos = " << TOS << "\n";
            log << "opc = " << OPC << "\n";
            log << "h = " << H << "\n";
        }
        bitset<32> executarULA(string sinais, bitset<32> A, bitset<32> B) {
            bool SLL8 = sinais[0] - '0';
            bool SRA1 = sinais[1] - '0';
            bool F0 = sinais[2] - '0';
            bool F1 = sinais[3] - '0';
            bool ENA = sinais[4] - '0';
            bool ENB = sinais[5] - '0';
            bool INVA = sinais[6] - '0';
            bool INC = sinais[7] - '0';
            if (!ENA) A.reset();
            if (!ENB) B.reset();
            if (INVA) A.flip();
            bitset<32> resultado;
            if (!F0 && !F1) resultado = A & B;
            else if (!F0 && F1) resultado = A | B;
            else if (F0 && !F1) resultado = ~B;
            else if (F0 && F1) resultado = A.to_ulong() + B.to_ulong() + INC;
            if (SLL8 && SRA1) throw runtime_error("Sinais SLL8 e SRA1 ativados ao mesmo tempo!");
            if (SLL8) resultado <<= 8;
            if (SRA1) resultado >>= 1;
            return resultado;
        }
        string nomeBarramentoB(string codigo) {
            if (codigo == "0000") return "mdr";
            if (codigo == "0001") return "pc";
            if (codigo == "0010") return "mbr";
            if (codigo == "0011") return "mbru";
            if (codigo == "0100") return "sp";
            if (codigo == "0101") return "lv";
            if (codigo == "0110") return "cpp";
            if (codigo == "0111") return "tos";
            if (codigo == "1000") return "opc";
            return "???";
        }
        bitset<32> valorBarramentoB(string codigo) {
            if (codigo == "0000") return MDR;
            if (codigo == "0001") return PC;
            if (codigo == "0010") return bitset<32>(string(24, '0') + MBR.to_string()); 
            if (codigo == "0011") return bitset<32>(string(24, '1') + MBR.to_string()); // MBRU
            if (codigo == "0100") return SP;
            if (codigo == "0101") return LV;
            if (codigo == "0110") return CPP;
            if (codigo == "0111") return TOS;
            if (codigo == "1000") return OPC;
            return 0;
        }
        string nomeBarramentoC(string codigo) {
            string s = "";
            if (codigo[0] == '1') s += "h ";
            if (codigo[1] == '1') s += "opc ";
            if (codigo[2] == '1') s += "tos ";
            if (codigo[3] == '1') s += "cpp ";
            if (codigo[4] == '1') s += "lv ";
            if (codigo[5] == '1') s += "sp ";
            if (codigo[6] == '1') s += "pc ";
            if (codigo[7] == '1') s += "mdr ";
            if (codigo[8] == '1') s += "mar ";
            return s;
        }
        void escreverBarramentoC(string codigo, bitset<32> valor) {
            if (codigo[0] == '1') H = valor;
            if (codigo[1] == '1') OPC = valor;
            if (codigo[2] == '1') TOS = valor;
            if (codigo[3] == '1') CPP = valor;
            if (codigo[4] == '1') LV = valor;
            if (codigo[5] == '1') SP = valor;
            if (codigo[6] == '1') PC = valor;
            if (codigo[7] == '1') MDR = valor;
            if (codigo[8] == '1') MAR = valor;
        }
        void executarInstrucao(string instrucao, int ciclo, ofstream& log) {
            IR = instrucao;
            string sinaisULA = IR.substr(0, 8);
            string cBus = IR.substr(8, 9);
            string memBits = IR.substr(17, 2);
            string bBus = IR.substr(19, 4);
            bitset<32> A = H;
            bitset<32> B = valorBarramentoB(bBus);
            bitset<32> Sd = executarULA(sinaisULA, A, B);
            log << "============================================================\n";
            log << "Cycle " << ciclo << "\n";
            log << "ir = " << sinaisULA << " " << cBus << " " << memBits << " " << bBus << "\n";
            log << "b = " << nomeBarramentoB(bBus) << "\n";
            log << "c = " << nomeBarramentoC(cBus) << "\n\n";
            log << "> Registers before instruction\n*******************************\n";
            imprimirRegistradores(log);
            log << "\n";
            escreverBarramentoC(cBus, Sd);
            if (memBits == "10") { //WRITE
                memoria[MAR.to_ulong()] = MDR;
            } else if (memBits == "01") { //READ
                MDR = memoria[MAR.to_ulong()];
            }
            log << "> Registers after instruction\n*******************************\n";
            imprimirRegistradores(log);
            log << "\n";
            log << "> Memory after instruction\n*******************************\n";
            for (auto& m : memoria) log << m << "\n";
        }
};

#endif