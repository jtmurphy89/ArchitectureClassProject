addi $8,$0,6
sw $8,12($0)
addi $9, $0, 3
lw   $10, 12($0)
jr $10
addi $16, $0, -1
addi $11, $0, 9
jr $11
addi $4, $3, 45
jal LABEL
addi $5, $6, 7
LABEL:   addi $16, $0, -1
