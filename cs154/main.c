#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "functions.h"


typedef struct pipe_line {
    InstInfo * pipelineArray[5];
    int size;
} pipeline;


typedef struct deststruct {
    int location;
    int wire;
    int mem;
} destStruct;

typedef struct pertinentflags {
    int instnum;
    int cycle;
    //int afterMaxPC;
    int maxpc;
    int branchTaken;
    int fetch;
    int numberOfBranchesTaken;
    //because load word needs data forwarding for 2 away sucka
    destStruct destArray[2];
} pFlags;



//pipeline forward declarations
void fetchBody(pipeline * pline, pFlags * pf, InstInfo * noop);
void decodeBody(pipeline * pline, pFlags * pf, InstInfo * noop);
void executeBody(pipeline * pline, pFlags * pf);
void memoryBody(pipeline * pline, pFlags * pf);
void writebackBody(pipeline * pline, pFlags * pf);

//branching flag setter
int needsExecuteNoop(pipeline * pline, pFlags * pf);
void dataForwardInDecode(pipeline * pline, pFlags * pf);





int main(int argc, char *argv[]) {
    //InstInfo curInst;
    //InstInfo *instPtr = &curInst;

    //initializes the struct that holds all pertinent flags that we will
    //need to keep throughout this process
    pFlags * pf = (pFlags *)malloc(sizeof(pFlags));
    pf->instnum = 0;
    pf->cycle = 0;

    //initializes the beq to zero so that we
    //assume the branch was not taken. I did it
    //this way so we can easily replace it with a
    //two bit predictor later
    pf->branchTaken = 0;

    //This flag is set when we insert a noop so that we
    //do not fetch when the pipeline is already full
    //it is set to 1 to indicate we do NOT need to fetch
    pf->fetch = 1;

    //counts the number of noops inserted so that we can
    //determine the correct number of instructions run


    //this is a noop instruction.  we will use this if we need to stall
    InstInfo * noop = (InstInfo *)malloc(sizeof(InstInfo));
    noop->inst = -1;
    noop->string[0] = '\0';


    //allocates space for the pipeline
    pipeline * pline = (pipeline *)malloc(sizeof(pipeline));


    //intializes the pipeline to fill it with noops
    pline->size = 5;
    int i;
    for (i = 0; i < 5; i++){
        pline->pipelineArray[i] = noop;
    }

    //because load word needs to data forward in memory location
    //we need 2 members in our dest array
    pf->destArray[0].location = -1;
    pf->destArray[0].wire = 8008135;


    FILE *program;
    if (argc != 2)
    {
        printf("Usage: sim filename\n");
        exit(0);
    }

    pf->maxpc = load(argv[1]);
    //printLoad(maxpc);

    int numberOfBranchesTaken = 0;

    //while (pc <= maxpc-1)
    //while ((heap->size > 0) && (maxpc != 0))
    pf->destArray[0].location = -1;
    pf->destArray[0].wire = 8008135;

    while (pline->size > 1 ) {
        //printf("this is the size: %d", pline->size);
        //incase the memory stage will have a lw
        //we data forward the execute wire to the
        //memory wires
        pf->destArray[1].location = pf->destArray[0].location;
        pf->destArray[1].wire = pf->destArray[0].wire;
        pf->destArray[1].mem = pf->destArray[0].mem;

        //Initialize these to zero again for the execute stage
        pf->destArray[0].location = -1;
        pf->destArray[0].mem = 0;

        //sets the branchTaken parameter back to 0 so that we
        //won't delete the instruction moving from the fetch
        //to the decode stage
        pf->branchTaken = 0;

        //sets the fetch parameter back to 1 so that we
        //can fetch freely when there isn't a noop
        pf->fetch = 1;

        //first we call write back so that we can make space to move the other
        //instructions forwards.  Then we call the others in opposite order
        //for the same reason

        writebackBody(pline, pf);
        memoryBody(pline, pf);
        executeBody(pline, pf);
        decodeBody(pline, pf, noop);
        fetchBody(pline, pf, noop);

        //we call this so that our registers are updated before we print
        //otherwise, we can delay the pipeline by 1 cycle.
        printP2(pline->pipelineArray[0],
                pline->pipelineArray[1],
                pline->pipelineArray[2],
                pline->pipelineArray[3],
                pline->pipelineArray[4],
                pf->cycle);

        pf->cycle = pf->cycle + 1;
    }

    printf("Cycles: %d\n", pf->cycle);
    // FUCK YOU
    printf("Instructions Executed: %d\n", pf->maxpc - pf->numberOfBranchesTaken);

    exit(0);

}

/* print
 *
 * prints out the state of the simulator after each instruction
 */
void print(InstInfo *inst, int count)
{
    int i, j;
    printf("Instruction %d: %d\n",count,inst->inst);
    printf("%s\n",inst->string);
    printf("Fields: {rd: %d, rs: %d, rt: %d, imm: %d}\n",
        inst->fields.rd, inst->fields.rs, inst->fields.rt, inst->fields.imm);
    printf("Control Bits:\n{alu: %d, mw: %d, mr: %d, mtr: %d, asrc: %d, bt: %d, rdst: %d, rw: %d}\n",
        inst->signals.aluop, inst->signals.mw, inst->signals.mr, inst->signals.mtr, inst->signals.asrc,
        inst->signals.btype, inst->signals.rdst, inst->signals.rw);
    printf("ALU Result: %d\n",inst->aluout);
    if (inst->signals.mr)
        printf("Mem Result: %d\n",inst->memout);
    else
        printf("Mem Result: X\n");
    for(i=0;i<8;i++)
    {
        for(j=0;j<32;j+=8)
            printf("$%d: %4d ",i+j,regfile[i+j]);
        printf("\n");
    }
    printf("\n");
}
/*
 * print out the loaded instructions.  This is to verify your loader
 * works.
 */
void printLoad(int max)
{
    int i;
    for(i=0;i<max;i++)
        printf("%d\n",instmem[i]);
}


void fetchBody(pipeline * pline, pFlags * pf, InstInfo * noop) {

    if (pf->fetch == 1 && pc < pf->maxpc) {

        //moves the previously fetched instruction
        //into decode for next time.  If the branch was taken
        //then we delete it.
        if (pf->branchTaken == 1) {
            pline->pipelineArray[1] = noop;
            pf->numberOfBranchesTaken++;
        }
        else {
            pline->pipelineArray[1] = pline->pipelineArray[0];
        }
        //creates a new instruction to fetch
        InstInfo *instPtr = (InstInfo*)(malloc(sizeof(InstInfo)));

        fetch(instPtr);
        decode(instPtr);
        pline->pipelineArray[0] = instPtr;
    }
    else if (pf->fetch == 1 && pc >= pf->maxpc) {
        //moves the previously fetched instruction
        //into decode for next time.  If the branch was taken
        //then we delete it.
        if (pf->branchTaken == 1) {
            pline->pipelineArray[1] = noop;
            pf->numberOfBranchesTaken++;
        }
        else {
            pline->pipelineArray[1] = pline->pipelineArray[0];
        }
        //creates a new instruction to fetch
        InstInfo *instPtr = (InstInfo*)(malloc(sizeof(InstInfo)));

        pline->pipelineArray[0] = noop;
        pline->size--;
    }

}

void decodeBody(pipeline * pline, pFlags * pf, InstInfo * noop) {
    if ( pline->pipelineArray[1]->inst != -1){
        decode(pline->pipelineArray[1]);

        //calls a function that takes care of data forwarding
        dataForwardInDecode(pline, pf);
        //calls a function that checks if we need to add
        //a noop between decode and execute
        int needNoop = needsExecuteNoop(pline, pf);

        if (needNoop) {
            //adds noop to execute stage
            pline->pipelineArray[2] = noop;

            //sets the flag so that we do not fetch an
            //instruction as the pipeline is already full
            pf->fetch = 0;
        }
        else {
            //moves decode to execute
            pline->pipelineArray[2] = pline->pipelineArray[1];
        }
    }
    else {
        pline->pipelineArray[2] = noop;
    }
}

void executeBody(pipeline * pline, pFlags * pf) {
    if ( pline->pipelineArray[2]->inst != -1){
        execute(pline->pipelineArray[2]);

        //automatically forwards the data, even it if may not be needed
        pf->destArray[0].location = pline->pipelineArray[2]->destreg;
        pf->destArray[0].wire = pline->pipelineArray[2]->destdata;

        //if the instruction is load word, we set the memflag in the
        //data forwarding array
        if (pline->pipelineArray[2]->fields.op == 17){
            pf->destArray[0].mem = 1;
        }
    }
}

void memoryBody(pipeline * pline, pFlags * pf) {
    //moves execute to memory
    pline->pipelineArray[3] = pline->pipelineArray[2];

    if ( pline->pipelineArray[3]->inst != -1){
        memory(pline->pipelineArray[3]);

        //forwards the data only if the instruction is lw,
        //even if it may not be needed
        if (pline->pipelineArray[3]->fields.op == 17){
            pf->destArray[1].location = pline->pipelineArray[3]->destreg;
            pf->destArray[1].wire = pline->pipelineArray[3]->memout;
        }
    }
}

void writebackBody(pipeline * pline, pFlags * pf) {
    //moves memory to writeback
    pline->pipelineArray[4] = pline->pipelineArray[3];

    if ( pline->pipelineArray[4]->inst != -1){
        writeback(pline->pipelineArray[4]);
    }
}


int needsExecuteNoop(pipeline * pline, pFlags * pf){
    //this is if there is a load word in the execute stage so we
    //have to wait till it reaches the memory stage, thus requiring
    //a noop to be put in the execute stage
    if (pline->pipelineArray[1]->input1 == pf->destArray[0].location &&
        pline->pipelineArray[2]->fields.op == 17) {
        return 1;
    }
    if (pline->pipelineArray[1]->input2 == pf->destArray[0].location &&
        pline->pipelineArray[2]->fields.op == 17) {
        return 1;
    }
    
    // if jal, we always take the branch and, thus, always insert a noop
    if(pline->pipelineArray[1]->fields.op == 8){
        pf->branchTaken = 1;
    }
            

    if (pline->pipelineArray[1]->fields.op == 10 || pline->pipelineArray[1]->fields.op == 37) {
        if (pline->pipelineArray[1]->input1 == pf->destArray[0].location) {
            return 1;
        }
        if (pline->pipelineArray[1]->input2 == pf->destArray[0].location) {
            return 1;
        }
        if (pf->destArray[1].mem == 1) {
            if (pline->pipelineArray[1]->input1 == pf->destArray[1].location) {
                return 1;
            }
            if (pline->pipelineArray[1]->input2 == pf->destArray[1].location) {
                return 1;
            }
        }
        // if the instruction was jr or if the instruction was branch and s1data == s2data, the branch is taken 
        if (pline->pipelineArray[1]->fields.op == 37 || pline->pipelineArray[1]->s1data - pline->pipelineArray[1]->s2data == 0) {
            pf->branchTaken = 1;
        }
        
    }
    return 0;
}

void dataForwardInDecode(pipeline * pline, pFlags * pf) {
    if (pline->pipelineArray[1]->input1 == pf->destArray[0].location) {
        pline->pipelineArray[1]->s1data = pf->destArray[0].wire;
    }
    if (pline->pipelineArray[1]->input2 == pf->destArray[0].location) {
        pline->pipelineArray[1]->s2data = pf->destArray[0].wire;
    }
    if (pf->destArray[0].mem == 1) {
        if (pline->pipelineArray[1]->input1 == pf->destArray[1].location) {
            pline->pipelineArray[1]->s1data = pf->destArray[1].wire;
        }
        if (pline->pipelineArray[1]->input2 == pf->destArray[1].location) {
            pline->pipelineArray[1]->s2data = pf->destArray[1].wire;
        }
    }
}



