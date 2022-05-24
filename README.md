# TUN3R
Arduino based tuner for synths

Tuner zaprojektowany dla syntyzatora modularnego zainspirowany "Performance VCO" od LMNC:
https://www.lookmumnocomputer.com/1222-performance-vco  
  
Częstotliwość (nuta) mierzona jest na pinie A0, wymagany jest przebieg prostokątny. 
Odczyt wyśwetlany jest na wyświetlaczu 8-segmentowym, pinout można zadeklarować w kodzie. Dodatkowo
dwie diody mogą sygnalizować czy nuta jest dobrze nastrojona.  
  
Odczyt częstotliwości opiera się na zliczaniu cyfrowych przerwań sprzętowych. Reszta programu pochodzi z kodu
napisanego przez Josa Boutena:
https://github.com/josbouten/Tune-O-Matic
