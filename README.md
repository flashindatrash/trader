# TraderBot

# устанавливаем зависимости
sudo apt update
sudo apt -y install cmake
sudo apt -y install build-essential

# создаем в юзере папку Developer и переходим
cd ~
mkdir Developer
cd Developer

# клонируем репозиторий и переходим
git clone https://gitlab.com/flashindatrash/traderbot.git
cd traderbot

# обновляем сабмодули репы
git submodule update --init --recursive

# создаем папку build и переходим
mkdir build
cd build


