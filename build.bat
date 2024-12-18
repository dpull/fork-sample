if not exist "buildwin" (
	mkdir buildwin
)

cd buildwin
cmake ../ -G"Visual Studio 17 2022"

cd ..
pause