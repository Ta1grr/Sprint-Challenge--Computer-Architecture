#include "cpu.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// #define DATA_LEN 12

/**
 *  CPU_RAM_READ
 */
  unsigned char cpu_ram_read(struct cpu *cpu, unsigned char index) {
    return cpu->ram[index];
  }

/**
 *  CPU_RAM_WRITE
 */
  void cpu_ram_write (struct cpu *cpu, unsigned char index, unsigned char value) {
    cpu->ram[index] = value;
  }

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu, char *filename) {
  FILE *fp;
  char line[32];
  int address = 0;
  fp = fopen(filename, "r");

  if (fp == NULL) {
    printf("Error open file: %s\n", filename);
    exit(1);
  }

  while(fgets(line, sizeof(line), fp) != NULL) {
    char *endptr;

    unsigned char value = strtoul(line, &endptr, 2);

    if (endptr == line){
      continue;
    }

    cpu->ram[address++] = value;

  }

  fclose(fp);

  // TODO: Replace this with something less hard-coded
}

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op) {
    case ALU_MUL:
      // TODO
      cpu->registers[regA] = cpu->registers[regA] * cpu->registers[regB];
      break;
    case ALU_ADD:
      // TODO
      cpu->registers[regA] = (cpu->registers[regA] + cpu->registers[regB]);
      break;
    case ALU_CMP:
      if(cpu->registers[regA] > cpu->registers[regB]){
        cpu->FL = 0b00000010;
      }
      else if(cpu->registers[regA] < cpu->registers[regB]){
        cpu->FL = 0b00000100;
      }
      else{
        cpu->FL = 0b00000001;
      }
      break;
    default:
      break;

    // TODO: implement more ALU ops
  }
}


/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction

  unsigned char IR, operandA, operandB, add_to_PC;
  
  while (running) {

    IR = cpu_ram_read(cpu, cpu->PC);
    // OperandA - PC + 1
    operandA = cpu_ram_read(cpu, cpu->PC + 1); // MDR_A
    // OperandB - PC + 2
    operandB = cpu_ram_read(cpu, cpu->PC + 2); // MDR_B
    // adding to program counter
    add_to_PC = (IR >> 6) + 1;


      switch(IR) {
        case LDI:
          cpu->registers[operandA] = operandB;
          break;
        case PRN:
          printf("%d\n", cpu->registers[operandA]);
          break;
        case HLT:
          running = 0;
          break;
        case MUL:
          alu(cpu, ALU_MUL, operandA, operandB);
          break;
        case ADD:
          alu(cpu, ALU_ADD, operandA, operandB);
          break;
        case CMP:
          alu(cpu, ALU_CMP, operandA, operandB);
          break;
        case JMP:
          cpu->PC = cpu->registers[operandA]; // Setting PC to the value in the given register
          break;
        case JEQ:
          if(cpu->FL == 0b00000001 /* 0b00000LGE */) {
          cpu->PC = cpu->registers[operandA];
          }
          break;
        case JNE:
          if(cpu->FL == 0b00000000) {
          cpu->PC = cpu->registers[operandA];
          }
          break;
        case PUSH:
          cpu->registers[7]--;
          cpu->ram[cpu->registers[7]] = cpu->registers[operandA];
          break;
        case POP:
          cpu->registers[operandA] = cpu->ram[cpu->registers[7]];
          cpu->registers[7]++;
          break;
        case CALL:
          cpu->registers[7]--;
          cpu->ram[cpu->registers[7]] = cpu->PC += add_to_PC;
          cpu->PC = cpu->registers[operandA];
          cpu->PC -= 2; // To offset the add_to_PC;
          break;
        case RET:
          cpu->PC = cpu->ram[cpu->registers[7]];
          cpu->PC--; // To offset the add_to_PC;
          cpu->registers[7]++;
          break;
        default:
          break;
      }

        cpu->PC += add_to_PC;

    // TODO
    // 1. Get the value of the current instruction (in address PC).
    // 2. Figure out how many operands this next instruction requires
    // 3. Get the appropriate value(s) of the operands following this instruction
    // 4. switch() over it to decide on a course of action.
    // 5. Do whatever the instruction should do according to the spec.
    // 6. Move the PC to the next instruction.
  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers
  cpu->PC = 0;
  cpu->FL = 0; // 0 = 0b00000000 or 0x00
  memset(cpu->ram, 0, sizeof(cpu->ram)); // array
  memset(cpu->registers, 0, sizeof(cpu->registers)); // array
  cpu->registers[7] = 0xF4;
}

