  # labwork.S
  # Written 2015 by F Lundevall
	# Modified 25/01/2016 by O Bystedt
  # Skeleton file for IS1200/IS1500 lab 1 2015-2016.
  # The macros PUSH and POP are in the public domain.


.macro		PUSH $reg
	addi	$sp, $sp, -4
	sw		\$reg, 0($sp)

.endm

.macro		POP $reg
	lw		\$reg, 0($sp)
	addi	$sp, $sp, 4
.endm

.data
.align 2
delayparameter: .word 9442		# delay parameter 4711 in hex
.text
main:

.global enable_interrupt

enable_interrupt:
ei
jr		$ra					# jump to
nop

.global hexasc
hexasc:

 	li				$t1, 9						# set $t1 to 9
 	ble				$t5, $t1, hexanum	# if $t5 is equal or less than 9, branch to hexanum
	jr				$ra								# jump out of branch
	nop

	hexanum:
	addi			$t5, $t5, 0x30			# add 0x30 to callee value $t5 (ASCII code for "0" is 0x30)
	move			$a3, $t5						# move value in $t5 to $a3
	jr 				$ra
	nop


.global delay
 delay:

 		PUSH    $ra										# push the registers onto the stack
 		lw			$t0, delayparameter		# load delayparameter to $t0
 		mul			$t0, $a0, $t0					# multiply the delayparameter by the value $a0

 	loop:
		ble 		$t0, $0, doneLoop			# if $t0 <= 0, end loop
 		addiu		$t0, $t0, -1					# decrease number of loops by one
 		slt			$t1, $0, $t0					# if 0 < n of loops $t1==1 else $t1==0
 		beq			$t1, $0, doneLoop			# if $t1==0 (n of loops <= 0) branch to doneLoop
 		j	loop												# repeat loop again
 	doneLoop:
 	# Loops completed, pop the registers from the stack
 		POP			$ra
 		jr 			$ra
 		nop

.global time2string
time2string:

# stack:
	PUSH $ra										# create stack
	PUSH $s0
	PUSH $s1
	PUSH $s2
	PUSH $s3
	PUSH $s4
	PUSH $s5
	PUSH $s6

andi			$s0, $a1, 0xf000		# load byte 2 into temporary variable t1
srl				$s0, $s0, 12				# create variable s0 that is the most significant nibble of t1
andi			$s1, $a1, 0x0f00		# operate and with f00 to remove the nibble already in s0
srl				$s1, $s1, 8					# shift to the right to create variable s1

li				$s2, 58							# load intermediate 0x3a, the colon in ASCII code

andi			$s3, $a1, 0x00f0		# load byte 1 into temporary variable t3
srl				$s3, $s3, 4					# create variable s3 which is the most significant nibble of t3
andi			$s4, $a1, 0x000f		# operate and with f to load the final 4 bits into s4

li				$s5, 0x00						# load intermediate 0x30, the 0 in ASCII code
li				$s6, 0							#s6= 0



											# the X value for xx:00X

	move			$t5, $s0						# set $s0 to $t5
	jal	hexasc										# call hexasc which returns $a3 in ASCII number
	nop
	sb				$a3, 0($a0)					# set $a3 to position 0 of the stack $a0

	move			$t5, $s1						# set $s1 to $t5
	jal hexasc										# call hexasc which returns $a3 in ASCII number
	nop
	sb				$a3, 1($a0)					# set $a3 to position 1 of the stack $a0

	sb				$s2, 2($a0)					# set $s2 to position 2 of the stack $a0

	move			$t5, $s3						# set $s3 to $t5
	jal hexasc										# call hexasc which returns $a3 in ASCII number
	nop
	sb				$a3, 3($a0)					# set $a3 to position 3 of the stack $a0


	move			$t5, $s4						# set $s4 to $t5
	jal hexasc										# call hexasc which returns $a3 in ASCII number
	nop
	sb				$a3, 4($a0)					# set $a3 to position 4 of the stack $a0

	sb				$s5, 5($a0)					# set $s5 to position 5 of the stack $a0

	sb				$s6, 6($a0)					# set $s6 to position 6 of the stack $a0

	bne				$s3, $0, skip				# if  $s3 != 0 then skip
	bne				$s4, $0, skip				# if  $s4 != 0 then skip

	li				$t9, 0x58						# set to 'X' if last 2 digits are 0
	sb				$t9, 6($a0)





	skip:



	POP $s6
	POP $s5												# popping stack
	POP $s4
	POP $s3
	POP $s2
	POP $s1
	POP $s0
	POP $ra

	jr				$ra									# return to main
	nop
