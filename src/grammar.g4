grammar elsix;

program : line ('\n' line)* ;

line :    LABEL? (ifexpr | thenexpr);

gotoexpr : END | DONE | LABEL;

ifexpr : ifkeyword compoundcondition gotoexpr '\n'
        | ifkeyword compoundcondition THEN operationblock gotoexpr? '\n';

thenexpr : THEN? operationblock gotoexpr? '\n'
        | THEN gotoexpr '\n';

compoundcondition : condition+;
condition
    : '(' contents ',' ('E' | 'N' | 'G' | 'L' ) ',' (contents | DECIMAL) ')'
    | '(' contents ',' ('EO' | 'NO' | 'GO' | 'LO' ) ',' OCTAL ')'
    | '(' contents ',' ('EH' | 'NH' | 'GH' | 'LH' ) ',' HOLLERITH ')'
    | '(' contents ',' 'P' ',' contents ')'
    | '(' contents ',' 'O' ',' (contents | OCTAL) ')'
    | '(' contents ',' 'OD' ',' DECIMAL ')'
    | '(' contents ',' 'OH' ',' HOLLERITH ')'
    | '(' contents ',' 'Z' ',' (contents | OCTAL) ')'
    | '(' contents ',' 'ZD' ',' DECIMAL ')'
    | '(' contents ',' 'ZH' ',' HOLLERITH ')';

operationblock : operation+;
operation
    : '(' ; // ToDo: Etc., etc.

fragment ifkeyword : IFANY | IF | IFALL | IFNALL | IFNONE | NOT | THEN | DONE;

IFANY     : 'IFANY'     | 'IF' ;
IFALL     : 'IFALL' ;
IFNALL    : 'IFNALL';
IFNONE    : 'IFNONE'    | 'NOT';
THEN      : 'THEN'  ;
DO        : 'DO'    ;
DONE      : 'DONE'  ;   // Ends a DO block.
FAIL      : 'FAIL'  ;   // Special GOTO upon failure.
STATE     : 'STATE' ;   // System debugging routine.
DUMP      : 'DUMP'  ;   // System debugging routine.
END       : 'END'   ;

LABEL : [A-Z]+[A-Z0-9]+;
DEFINABLE : [A-Z0-9];
BUG : [A-Z];
