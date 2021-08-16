
rm -rf ./out
#rm -rf build-win build-wsl
mkdir -p out

(
	mkdir -p build-win
	cd build-win
	cmd.exe /C cmake -DCMAKE_INSTALL_PREFIX="..\out" -G "Visual Studio 16 2019" ..
	cmd.exe /C cmake --build . --target ALL_BUILD --config Release -- /nologo /verbosity:minimal /maxcpucount
)


(
	mkdir -p build-wsl
	cd build-wsl
	cmake -DCMAKE_INSTALL_PREFIX="../out" -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ..
	cmake --build .
)


(
	cd out
	mkdir bin
	cp ../build-win/Release/t65sim.exe bin
	cp ../build-wsl/t65sim bin
	cp ../t65 ../t65.bat bin
	chmod +x bin/*
	cp ../README.txt .
	zip ../out.zip -r ./*
)

