#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "functions.h"

// these are the structures used in this simulator


// global variables
// register file
int regfile[32];
// instruction memory
int instmem[100];  // only support 100 static instructions
// data memory
int datamem[1024];
// program counter
int pc;



// these are the different functions you need to write
int load(char *filename);
void fetch(InstInfo *);
void decode(InstInfo *);
void execute(InstInfo *);
void memory(InstInfo *);
void writeback(InstInfo *);

/* load
 *
 * Given the filename, which is a text file that
 * contains the instructions stored as integers
 *
 * You will need to load it into your global structure that
 * stores all of the instructions.
 *
 * The return value is the maxpc - the number of instructions
 * in the file
 */
int load(char *filename)
{
    FILE* fp = fopen(filename, "r");
    char* line = NULL;
    size_t len = 0;
    int maxpc = 0;
    pc = 0;
    int temp = 0;
    while(getline(&line, &len, fp) != -1){
        temp = atoi(line);
        instmem[maxpc] =  temp;
        maxpc++;
    }
    free(line);
    return maxpc;
}

/* fetch
 *
 * This fetches the next instruction and updates the program counter.
 * "fetching" means filling in the inst field of the instruction.
 */
void fetch(InstInfo *instruction)
{
    instruction->inst = instmem[pc];
    instruction->pc = pc;
    pc++;
}

/* decode
 *
 * This decodes an instruction.  It looks at the inst field of the
 * instruction.  Then it decodes the fields into the fields data
 * member.  The first one is given to you.
 *
 * Then it checks the op code.  Depending on what the opcode is, it
 * fills in all of the signals for that instruction.
 */
void decode(InstInfo *instruction)
{
    // fill in the signals and fields
    int val = instruction->inst;
    int op, func;
    instruction->fields.op = (val >> 26) & 0x03f;
    instruction->fields.func = val & 0x03f;
    instruction->fields.rd = (val >> 11) & 0x1f;
    instruction->fields.rt = (val >> 16) & 0x1f;
    instruction->fields.rs = (val >> 21) & 0x1f;
    instruction->fields.imm = val & 0xffff;
    if (instruction->fields.imm > 32767) { //2^15 - 1 because of 2's compliment
        instruction->fields.imm = instruction->fields.imm - 65536; //subtracting by 2^16 flips bits
    }

    switch(instruction->fields.op){
        case 49: // addi
            //instruction->fields.func = -1;
            instruction->signals.aluop = 2;
            instruction->signals.mw = 0;
            instruction->signals.mr = 0;
            instruction->signals.mtr = 0; // UNSURE
            instruction->signals.asrc = 1;
            instruction->signals.btype = 0;
            instruction->signals.rdst = 0;
            instruction->signals.rw = 1;
            sprintf(instruction->string,"addi $%d, $%d, %d",
                instruction->fields.rt, instruction->fields.rs,
                instruction->fields.imm);
            instruction->destreg = instruction->fields.rt;
            instruction->input1 = instruction->fields.rs;
            instruction->input2 = instruction->fields.imm;
            instruction->s1data = regfile[instruction->fields.rs];
            break;
        case 32: // R-type
            instruction->signals.mw = 0;
            instruction->signals.mr = 0;
            instruction->signals.mtr = 0;
            instruction->signals.asrc = 0;
            instruction->signals.btype = 0;
            instruction->signals.rdst = 1;
            instruction->signals.rw = 1;
            instruction->destreg = instruction->fields.rd;
            instruction->input1 = instruction->fields.rs;
            instruction->input2 = instruction->fields.rt;
            instruction->s1data = regfile[instruction->fields.rs];
            instruction->s2data = regfile[instruction->fields.rt];
            switch(instruction->fields.func){
                case 32: // add
                    instruction->signals.aluop = 2;
                    sprintf(instruction->string,"add $%d, $%d, $%d",
                        instruction->fields.rd, instruction->fields.rs,
                        instruction->fields.rt);
                    break;
                case 40: // sub
                    instruction->signals.aluop = 3;
                    sprintf(instruction->string,"sub $%d, $%d, $%d",
                        instruction->fields.rd, instruction->fields.rs,
                        instruction->fields.rt);
                    break;
                case 36: //and
                    instruction->signals.aluop = 0;
                    sprintf(instruction->string,"and $%d, $%d, $%d",
                        instruction->fields.rd, instruction->fields.rs,
                        instruction->fields.rt);
                    break;
                case 48: // sgt
                    instruction->signals.aluop = 7;
                    sprintf(instruction->string,"sgt $%d, $%d, $%d",
                        instruction->fields.rd, instruction->fields.rs,
                        instruction->fields.rt);
                    break;
            }
            break;
        case 17: // lw
            //instruction->fields.func = -1;
            instruction->signals.aluop = 2;
            instruction->signals.mw = 0;
            instruction->signals.mr = 1;
            instruction->signals.mtr = 1;
            instruction->signals.asrc = 1;
            instruction->signals.btype = 0;
            instruction->signals.rdst = 0;
            instruction->signals.rw = 1;
            sprintf(instruction->string,"lw $%d, %d($%d)",
                instruction->fields.rt, instruction->fields.imm,
                instruction->fields.rs);
            instruction->destreg = instruction->fields.rt;
            instruction->input1 = instruction->fields.rs;
            instruction->input2 = instruction->fields.imm;
            instruction->s1data = regfile[instruction->fields.rs];
            break;
        case 18: // sw
            //instruction->fields.func = -1;
            instruction->signals.aluop = 2;
            instruction->signals.mw = 1;
            instruction->signals.mr = 0;
            instruction->signals.mtr = -1;
            instruction->signals.asrc = 1;
            instruction->signals.btype = 0;
            instruction->signals.rdst = -1;
            instruction->signals.rw = 0;
            sprintf(instruction->string,"sw $%d, %d($%d)",
                instruction->fields.rt, instruction->fields.imm,
                instruction->fields.rs);
            instruction->destreg = -1;
            instruction->input1 = instruction->fields.rs;
            instruction->input2 = instruction->fields.imm;
            instruction->s1data = regfile[instruction->fields.rs];
            break;
        case 10: // beq
            //instruction->fields.func = -1;
            instruction->signals.aluop = 3;
            instruction->signals.mw = 0;
            instruction->signals.mr = 0;
            instruction->signals.mtr = -1;
            instruction->signals.asrc = 0;
            instruction->signals.btype = 3;
            instruction->signals.rdst = -1;
            instruction->signals.rw = 0;
            sprintf(instruction->string,"beq $%d, $%d, %d",
                instruction->fields.rs, instruction->fields.rt,
                instruction->fields.imm);
            instruction->destreg = -1;
            instruction->input1 = instruction->fields.rs;
            instruction->input2 = instruction->fields.rt;
            instruction->s1data = regfile[instruction->fields.rs];
            instruction->s2data = regfile[instruction->fields.rt];
            break;
        case 37: // jr
            //instruction->fields.func = -1;
            //instruction->fields.rt = -1;
            //instruction->fields.imm = -1;
            instruction->signals.aluop = -1;
            instruction->signals.mw = 0;
            instruction->signals.mr = 0;
            instruction->signals.mtr = -1;
            instruction->signals.asrc = -1;
            instruction->signals.btype = 2;
            instruction->signals.rdst = -1;
            instruction->signals.rw = 0;
            sprintf(instruction->string,"jr $%d",
                instruction->fields.rs);
            instruction->destreg = -1;
            instruction->input1 = instruction->fields.rs;
            instruction->s1data = regfile[instruction->fields.rs];
            break;
        case 8: // jal
            //instruction->fields.func = -1;
            //instruction->fields.rt = -1;
            //instruction->fields.rs = -1;
            instruction->fields.imm = val & 0x3ffffff;
            if (instruction->fields.imm > 33554431) { //2^ - 1 because of 2's compliment
                instruction->fields.imm = instruction->fields.imm - 67108863; //subtracting by 2^16 flips bits
            }
            instruction->signals.aluop = -1;
            instruction->signals.mw = 0;
            instruction->signals.mr = 0;
            instruction->signals.mtr = 2;
            instruction->signals.asrc = -1;
            instruction->signals.btype = 1;
            instruction->signals.rdst = 2;
            instruction->signals.rw = 1;
            sprintf(instruction->string,"jal %d",
                instruction->fields.imm);
            instruction->destreg = 2;
            break;
    }




}

/* execute
 *
 * This fills in the aluout value into the instruction and destdata
 */

void execute(InstInfo *instruction)
{
    switch(instruction->signals.aluop) {
        case 0:
            if (instruction->signals.asrc){
                instruction->aluout = instruction->s1data & instruction->fields.imm;
            }
            else {
                instruction->aluout = instruction->s1data & instruction->s2data;
            }
            instruction->destdata = instruction->aluout;
            break;
        case 2:
            if (instruction->signals.asrc){
                instruction->aluout = instruction->s1data + instruction->fields.imm;
            }
            else {
                instruction->aluout = instruction->s1data + instruction->s2data;
            }
            instruction->destdata = instruction->aluout;
            break;
        case 3:
             if (instruction->signals.asrc){
                instruction->aluout = instruction->s1data - instruction->fields.imm;
            }
            else {
                instruction->aluout = instruction->s1data - instruction->s2data;
            }
            instruction->destdata = instruction->aluout;
            break;
        case 7:
             if (instruction->signals.asrc){
                instruction->aluout = instruction->s1data > instruction->fields.imm;
            }
            else {
                instruction->aluout = instruction->s1data > instruction->s2data;
            }
            instruction->destdata = instruction->aluout;
            break;
    }

}

/* memory
 *
 * If this is a load or a store, perform the memory operation
 */

void memory(InstInfo *instruction)
{
    int rs, imm;
    if (instruction->signals.mr) {
        rs = instruction->s1data;
        imm = instruction->fields.imm;

        instruction->memout = datamem[rs + imm];
    }
    if (instruction->signals.mw) {
        rs = instruction->s1data;
        imm = instruction->fields.imm;

        datamem[rs + imm] = regfile[instruction->fields.rt];

    }
}

/* writeback
 *
 * If a register file is supposed to be written, write to it now
 */
void writeback(InstInfo *instruction)
{
    if (instruction->signals.rw == 1) {
        if (instruction->signals.mtr == 1){
            switch(instruction->signals.rdst){
                case 0:
                    regfile[instruction->fields.rt] = instruction->memout;
                    break;
                case 1:
                    regfile[instruction->fields.rd] = instruction->memout;
                    break;
                case 2:
                    regfile[31] = instruction->memout;
                    break;
            }
        }
        else if (instruction->signals.mtr == 0){
            switch(instruction->signals.rdst){
                case 0:
                    regfile[instruction->fields.rt] = instruction->destdata;
                    break;
                case 1:
                    regfile[instruction->fields.rd] = instruction->destdata;
                    break;
                case 2:
                    regfile[31] = instruction->destdata;
                    break;
            }
        }
        else {
            switch(instruction->signals.rdst){
                case 0:
                    regfile[instruction->fields.rt] = pc;
                    break;
                case 1:
                    regfile[instruction->fields.rd] = pc;
                    break;
                case 2:
                    regfile[31] = pc;
                    break;

            }
        }
    }
}

/* setPCWithInfo
 *
 * branch instruction will overwrite PC
*/
void setPCWithInfo( InstInfo *instruction)
{
    switch(instruction->signals.btype){
        case 0:
            break;
        case 1:
            pc = instruction->fields.imm & 0x3fffff;
            break;
        case 2:
            pc = instruction->s1data;
            break;
        case 3:
            if (instruction->destdata == 0)
                pc = instruction->fields.imm;
            break;
    }
}


