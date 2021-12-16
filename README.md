# TraderBot  

##### устанавливаем зависимости  
	sudo apt update  
	sudo apt -y install cmake  
	sudo apt -y install build-essential  
	sudo apt -y install libssl-dev  
	sudo apt -y install libcurl4-openssl-dev  
	sudo apt -y install libhiredis-dev  
	sudo apt -y install libjsoncpp-dev  
	sudo apt -y install redis  

##### запускаем сервис редиса  
	sudo /etc/load.d/redis-server start  

##### создаем в юзере папку Developer и переходим  
	cd ~
	mkdir Developer  
  

##### build libwebsockets  
	cd ~/Developer
	git clone https://libwebsockets.org/repo/libwebsockets  
	cd libwebsockets  
	mkdir build  
	cd build  
	cmake ..
	make && sudo make install  
	sudo ldconfig  

##### build tgbot
	cd ~/Developer
	git clone https://github.com/reo7sp/tgbot-cpp 
	cd tgbot-cpp  
	mkdir build  
	cd build
	cmake ..
	sudo make install

##### клонируем репозиторий и переходим  
	cd ~/Developer  
	git clone https://gitlab.com/flashindatrash/traderbot.git  
	cd traderbot  

##### обновляем сабмодули репы  
	git submodule update --load --recursive  

##### создаем свой конфиг  
	cp ./config/default.cfg ./config/test.cfg  
	nano ./config/test.cfg  
##### редактируем конфиг  

##### создаем папку build и переходим  
	mkdir build  
	cd build  

##### устанавливаем бота 
	cmake -DCMAKE_INSTALL_PREFIX=. -DCMAKE_BUILD_TYPE=Debug ..  
	sudo make insall  

#### запускаем бота с аргументами  
	~/Developer/traderbot/build/bin/trader -c ~/Developer/traderbot/config/test.cfg -s btc usdt  
