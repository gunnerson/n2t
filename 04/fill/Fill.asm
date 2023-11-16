// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen
// by writing 'black' in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen by writing
// 'white' in every pixel;
// the screen should remain fully clear as long as no key is pressed.

//  while (true) {
//      x = RAM[24576] == 0 ? 0 : -1
//      for (i=16384, i<24576, i++){
//      RAM[i] = x
//      }    
//  }

(LOOP)
    @16384
    D=A
    @i
    M=D         // i = 16384
    @24576
    D=A
    @max
    M=D         // max = 24576
    @x
    M=0         // x = 0
    @24576
    D=M         // D = RAM[24576]
    @DRAW
    D;JEQ       // if (key == 0 ) GOTO DRAW
    @x
    M=-1        // x = -1
(DRAW)
    @max
    D=M
    @i
    D=D-M       // if (i == 24576)
    @LOOP
    D;JEQ       // GOTO LOOP
    @x
    D=M
    @i
    A=M
    M=D         // RAM[i] = x
    @i
    M=M+1       // i++
    @DRAW
    0;JMP
