; CHOS
; naskfunc
; TAB=4

[FORMAT "WCOFF"]			; オブジェクトファイルを作成
[INSTRSET "i486p"]			; 486の命令まで使いたいという記述
[BITS 32]					; 32ビットモード用の機械語を作らせる

; オブジェクトファイルのための情報

[FILE "naskfunc.nas"]		; ソースファイル名情報

		GLOBAL	_io_hlt		; このプログラムに含まれる関数名
		GLOBAL	_io_cli
		GLOBAL	_io_sti
		GLOBAL	_io_stihlt
		GLOBAL	_io_in8
		GLOBAL	_io_in16
		GLOBAL	_io_in32
		GLOBAL	_io_out8
		GLOBAL	_io_out16
		GLOBAL	_io_out32
		GLOBAL	_io_load_eflags
		GLOBAL	_io_store_eflags
		GLOBAL	_load_gdtr
		GLOBAL	_load_idtr
		GLOBAL	_asm_inthandler20
		GLOBAL	_asm_inthandler21
		GLOBAL	_asm_inthandler2c
		GLOBAL	_load_cr0
		GLOBAL	_store_cr0
		GLOBAL  _load_tr
		GLOBAL  _taskswitch4
		GLOBAL	_memtest_sub
		EXTERN	_inthandler20
		EXTERN	_inthandler21
		EXTERN	_inthandler2c


; 実際の関数

[SECTION .text]				; オブジェクトファイルではコレを書いてからプログラムを書く
							; .textセクションにプログラムを置いておくということかと．

_io_hlt:					; void io_hlt(void);
		HLT
		RET

_io_cli:					; void io_cli(void);
		CLI
		RET

_io_sti:					; void io_sti(void);
		STI
		RET

_io_stihlt:					; void io_stihlt(void);
		STI
		HLT
		RET

_io_in8:					; int io_in8(int port);
		MOV		EDX, [ESP+4]	; port
		MOV		EAX, 0
		IN		AL, DX
		RET

_io_in16:					; int io_in16(int port);
		MOV		EDX, [ESP+4]	; port
		MOV		EAX, 0
		IN		AX, DX
		RET

_io_in32:					; int io_in32(int port);
		MOV		EDX, [ESP+4]	; port
		IN		EAX, DX
		RET

_io_out8:					; void io_out8(int port, int data);
		MOV		EDX, [ESP+4]	; port
		MOV		AL, [ESP+8]		; data
		OUT		DX, AL
		RET

_io_out16:					; void io_out16(int port, int data);
		MOV		EDX, [ESP+4]	; port
		MOV		EAX, [ESP+8]	; data
		OUT		DX, AX
		RET

_io_out32:					; void io_out32(int port, int data);
		MOV		EDX, [ESP+4]	; port
		MOV		EAX, [ESP+8]	; data
		OUT		DX, EAX
		RET

_io_load_eflags:			; int io_load_eflags(void);
		PUSHFD				; PUSH EFLAGS
		POP		EAX
		RET

_io_store_eflags:			; void io_store_eflags(int eflags);
		MOV		EAX, [ESP+4]
		PUSH	EAX
		POPFD				; POP EFLAGS
		RET

_load_gdtr:					; void load_gdtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LGDT	[ESP+6]
		RET

_load_idtr:					; void load_idtr(int limit, int addr);
		MOV		AX,[ESP+4]		; limit
		MOV		[ESP+6],AX
		LIDT	[ESP+6]
		RET

_asm_inthandler20:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX, ESP
		PUSH	EAX
		MOV		AX, SS
		MOV		DS, AX
		MOV		ES, AX
		CALL	_inthandler20	; void inthandler20( int *esp );
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD				; 割り込み終了

_asm_inthandler21:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX, ESP
		PUSH	EAX
		MOV		AX, SS
		MOV		DS, AX
		MOV		ES, AX
		CALL	_inthandler21	; void inthandler21( int *esp );
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD				; 割り込み終了

_asm_inthandler2c:
		PUSH	ES
		PUSH	DS
		PUSHAD
		MOV		EAX, ESP
		PUSH	EAX
		MOV		AX, SS
		MOV		DS, AX
		MOV		ES, AX
		CALL	_inthandler2c
		POP		EAX
		POPAD
		POP		DS
		POP		ES
		IRETD				; 割り込み終了

_load_cr0:					; int load_cr0(void);
		MOV		EAX, CR0
		RET

_store_cr0:					; void store_cr0(int cr0);
		MOV		EAX, [ESP+4]
		MOV		CR0, EAX
		RET

_memtest_sub:		; unsigned int memtest_sub( unsigned int start, unsigned int end );
		PUSH	EDI
		PUSH	ESI
		PUSH	EBX
		MOV		ESI, 0xaa55aa55
		MOV		EDI, 0x55aa55aa
		MOV		EAX, [ESP+12+4]

mts_loop:
		MOV		EBX, EAX
		ADD		EBX, 0x0ffc
		MOV		EDX, [EBX]
		MOV		[EBX], ESI
		XOR		DWORD[EBX], 0xffffffff
		CMP		EDI, [EBX]
		JNE		mts_fin
		XOR		DWORD[EBX], 0xffffffff
		CMP		ESI, [EBX]
		JNE		mts_fin
		MOV		[EBX],EDX
		ADD		EAX, 0x1000
		CMP		EAX, [ESP+12+8]
		JBE		mts_loop
		POP		EBX
		POP		ESI
		POP		EDI
		RET

mts_fin:
		MOV		[EBX], EDX
		POP		EBX
		POP		ESI
		POP		EDI
		RET

_load_tr:				; void load_tr(int tr);
		LTR		[ESP+4]		; tr
		RET

_taskswitch4:			; void taskswitch4(void);
		JMP		4*8:0
		RET


