#include <iostream>
#include "parse.h"


int main(void){
	
	Parse *prs = new Parse();
	string str;
	while (true){
		/*get a row input of console*/
		getline(cin, str);
		/*if the row input is not after-space, parsing the commands*/
		if (str.size()>0)
			prs->CmdParse(str);
	}

	return 0;
}

/* A part of test case*/

/**
define (5f_67 x1_ y2 z3) = (+ (* x1_ y2) (- z3 3))
test (5f_67 3 4.2 5) 14.6
*/

/**
define (f x y z) = (+ (* x y) (- sqrt(+ y z) 3))
test (f 1 2 2) 1
*/


/**
define (f a b) = (pow(a b))
test (f 2 3)  8
*/


/**
define (g a b) = (* a b)
define (h x y) = (+ x (* x y))
define (f u v) = (- u v)
test (f (g 6 2) (h 2.5 8)) -10.5
test (f (g 6 2) (h 2.5 8)) -7
test (f (g 6 2) (h 2.5 8)) (f 6 2)
*/

/**
define (f x) = (* 2 x)
numint f 0.001 -1 1
*/

/**
define (f x y) = (+ x y)
numint f 0.01 0 1 0 1
*/

/**
define (f x y z) = (+ z (+ x y))
numint f 0.01 1 2 1 2 1 2
*/

/**
define (f x) = (* x x)
min f 0.01 0.01 5 1000

define (g x y) = (+ (* x x) (* y y))
min g 0.1 0.0001 10 4 1000
*/


/**
define (g a b) = (+ a b)
define (c x) = (g 5 6)
test (c 100) 11
*/


/**
define (k x y z) = (+ x (+ y z))
define (a u) = (k (+ 1 u) (+ 2 u) (+ 3 u))
test (a 1) 9
*/

/**
define (f a b) = (* a b)
define (k x y z) = (+ x (+ y z))
define (g u) = (k (+ 1 (f u (k 1 2 u))) (+ 2 u) (* 3 u))
test (g 3) 33
*/


