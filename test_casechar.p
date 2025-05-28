VAR 
    y : CHAR.
BEGIN
    y := 'C'; 
    CASE y OF
        'A': DISPLAY 'A'; 
        'B': DISPLAY 'C';
        'C': DISPLAY 'B'; 
        'D': DISPLAY 'D';
    END.
END.
