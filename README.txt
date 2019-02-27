=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=

	TEACA BOGDAN

	8 Ball Pool Game

=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=

Fisierele cod sursa se afla in directorul de la calea "Source\Laboratoare\Tema2".

Aceste fisiere sunt:

- Tema2.cpp (aici se afla codul principal)
- Tema2.h (header-ul fisierului Tema2.cpp, aici am definit variabile, structuri
si constante. Fiecare variabila din header are cate un comentariu atasat)
- CameraPool.h (aici am scris codul pentru camera third-person din joc)
- Shaders/VertexShader.glsl (vertex shader-ul principal)
- Shaders/FragmentShader.glsl (fragment shader-ul - am implementat iluminarea
Phong cu o sursa de lumina de tip Spot care se roteste sinusoidal dand impresia
unei lustre care se misca)
- Shaders/CueShader.glsl (vertex shader in care am implementat animatia tacului)

=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=

Game controls:

- La inceputul jocului, primul jucator poate muta bila cu tastele WASD in prima
treime din masa, dupa ce este multumit cu pozitia bilei, apasa Space, dupa care
se poate roti in jurul bilei miscand mouse-ul in timp ce tine apasat pe butonul
din dreapta al mouse-ului. Cand da drumul la butonul din stanga al mouse-ului,
se executa lovitura bilei de tac. Viteza imprimata bilei este direct proportionala
cu distanta la care se afla tacul fata de bila in momentul in care s-a dat drumul
la butonul din stanga al mouse-ului.
- Regulile jocului sunt implementate conform celor din enuntul temei.

=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=

Pentru bonus:

- am implementat o linie de ghidare indreptata catre directia unde se
va deplasa bila dupa ce este lovita de tac pentru a ajuta jucatorul sa joace mai
precis.

=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=<=>=