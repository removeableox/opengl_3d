FLAGS = -lglfw -I/opt/homebrew/Cellar/glfw/3.4/include -L/opt/homebrew/Cellar/glfw/3.4/lib/ -I/usr/include/ -framework OpenGL -Iinclude/

main:
	rm -rf ./bin
	mkdir ./bin
	gcc ./src/* -o ./bin/main $(FLAGS)
