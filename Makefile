.PHONY: all to_postfix_notation GUI

all: GUI_RUN

GUI_RUN: GUI
	./3D_VIEWER

GUI: obj_parcer.o affine_transformations.o
	gcc -g -o 3D_VIEWER $^ GUI.c `pkg-config --cflags --libs gtk4-macos`

obj_parcer.o:
	gcc -c -o obj_parcer.o obj_parcer.c `pkg-config --cflags --libs gtk4-macos`

affine_transformations.o:
	gcc -c -o affine_transformations.o affine_transformations.c `pkg-config --cflags --libs gtk4-macos`

install: GUI
	mkdir -p ../3D_VIEWER_APP
	mkdir -p ~/models
	mkdir -p ~/gui
	install ./3D_VIEWER ../3D_VIEWER_APP
	install ./setting.txt ~/
	install ./gui/GUI.xml ~/gui
	install ./gui/SETTING.xml ~/gui
	install ./gui/style.css ~/gui
	install ./models/*.obj ~/models

uninstall:
	rm -rf ../3D_VIEWER_APP
	rm -rf ~/models
	rm -rf ~/gui
	rm -rf ~/setting.txt

dvi:
	open -a "Safari" ./documentation/DOCUMENTATION_3D_VIEWER.html

dist:
	tar -czf 3D_VIEWER_COPY.tgz ./*

tests:
	gcc --coverage affine_transformations.c obj_parcer.c test.c  `pkg-config --cflags --libs gtk4-macos` -o test -lcheck 
	./test

clean:
	rm -rf ./*.o ./*.a ./a.out ./*_exe ./*.dSYM  ./test *.gc* *.info report *.cfg  *.tgz 3D_VIEWER_COPY
	rm -f 3D_VIEWER computing_module affine_transformations

lint:
	cp ../materials/linters/CPPLINT.cfg CPPLINT.cfg
	python3 ../materials/linters/cpplint.py --extension=c *.c *.h 

gcov:
	gcc --coverage affine_transformations.c obj_parcer.c test.c  `pkg-config --cflags --libs gtk4-macos` -o test -lcheck 
	./test
	lcov -t "test" -o test.info -c -d ./
	genhtml -o report test.info
	open -a "Safari" ./report/index.html

leaks:
	CK_FORK=no leaks --atExit -- ./test
