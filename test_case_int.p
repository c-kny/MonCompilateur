VAR
    i: INTEGER.
BEGIN
    i := 2;
    CASE i OF
        1: DISPLAY 'A';
        2,3: DISPLAY 'B';
        4: DISPLAY 'C';
    END.
END. 