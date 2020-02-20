;-------------------------------------------------------------------------


.CODE
DllEntry PROC hInstDLL:DWORD, reason:DWORD, reserved1:DWORD
mov rax, 1
ret
DllEntry ENDP

MyProc1 proc

;rcx, rdx, r9, r8
;rcx f(x)
;rdx f(x+step), gdzie step = |(dolncaGranicaCalkowania-gornaGranicaCalkowania)|/iloscTrapezow
;r8 tablica do ktorej beda zapisywane pola poszczegolnych trapezow
;r9 liczba iteracji 
	mov r10, rcx ;do r10 trafia zawartosc 1 tablicy w wartoscami funkcji, chcialem uzyc xchg, ale wolny bardzo, wyniki wychodza srednio 100 mikrosekund wieksze
	mov rcx,r9 ; jako, ze rcx to count register uzyjemy go do skokow i zaladujemy do niego ilosc iteracji 
	vmovaps xmm5, xmm1 ;w xxm5 tylko na najstarszym bicie jest h/step
	shufps xmm5,xmm5,00h; shuffle o offset, zeby w kazdej komorce znalazla sie ta sama liczba 
	vmovaps xmm4,xmm0 ; to samo co wyzej
	shufps xmm4,xmm4,00h ;/2
	vdivps xmm4, xmm5,xmm4 ;h/2 w xmm4, optymalizacja, skoro to jest stale, to po co to liczyc za kazdym razem 
	
								
; algorytm (f(x1) + f(x1+step))/2 *h
; gdzie step = |(dolnaGranicaCalkowania - gornaGranicaCalkowania)|/iloscTrapezow
loopAgain:		
	vmovaps  xmm0, [R10]			;do xmm0 trafia f(x)						
	vmovaps  xmm1, [RDX]			;do xmm1 trafia f(x+1) czyli wartosc funkcji w drugim punkcie oddalonym o rowna odleglosc 
	vaddps	xmm2, xmm1, xmm0		; xmm2=a+b
	vaddps xmm3,xmm3,xmm2
	add R10, 16						;przesuw o 4 pointery 
	add RDX, 16						; -||-
	loop loopAgain ; loop do loop gain, jesli rcx!=0, zamiast uzywac jakiegos countera i porownywac uzywam instrukcji loop, chyba jest szybciej
	vmulps xmm3,xmm3,xmm4			;  xmm3= (a+b)*(h/2)
	vmovaps  [R8], xmm3				; do r8, czyli tablicy wynikowej trafiaja pola poszczegolnych trapezow
	ret

MyProc1 endp
end


