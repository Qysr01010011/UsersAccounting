# UsersAccounting
Simple client-server application for users accounting. Example of multithread server with SQLite database and GUI client with QtWidgets.

Для работы приложения необходымы библиотеки:
gcc
cmake
make
drogon
jsoncpp
uuid
sqlite3
zlib
libqt5concurrent5t64
libqt5core5t64
libqt5dbus5t64
libqt5gui5t64
libqt5help
libqt5network5t64
libqt5sql5t64
libqt5sql5-sqlite
libqt5websockets5
libqt5widgets5t64

или для Windows/MacOS
libqt6concurrent6t64
libqt6core6t64
libqt6dbus6t64
libqt6gui6t64
libqt6help6
libqt6network6t64
libqt6sql6t64
libqt6sql6-sqlite
libqt6websockets6
libqt6widgets6t64

Сборка приложения:
Перейти в папку с приложением и создать директорию build
    mkdir build

перейти в папку build
    cd build

выполнить последовательность команд для сборки приложения
Linux:
Генерация проекта CMake
    cmake ..

Сборка
    make

Windows:
Генерация проекта через CMake
    cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.6.2\msvc2019_64"
    
    -G "Visual Studio 16 2019" — генератор VS.
    -A x64 — 64-бит сборка.
    -DCMAKE_PREFIX_PATH — путь к Qt, чтобы CMake нашёл

Сборка 
    cmake --build . --config Release

MacOS:
Генерация проекта через CMake:
    cmake .. -DCMAKE_PREFIX_PATH=/Users/username/Qt/6.6.2/clang_64

    /Users/username/Qt/6.6.2/clang_64 — путь к Qt.

Сборка:
    cmake --build . --config Release

Запуск приложения:
Переходим в папку со сгенерированными файлами программы:
Linux:
    (папка с проектом)/build/UsersAccounting/client - папка с файлом запуска клиента
    (папка с проектом)/build/UsersAccounting/server - папка с файлом запуска server

Windows/MacOS:
    (папка с проектом)\Release\build\UsersAccounting\client - папка с файлом запуска клиента
    (папка с проектом)\Release\build\UsersAccounting\server - папка с файлом запуска server

Запускаем сервер из папки server, затем клиента из папки client:
Linux/MacOS:
    ./users-accounting-server
    ./users-accounting-client

Windows:
    users-accounting-server.exe
    users-accounting-client.exe

В открывшемся окне клиента находится выпадающий список со списком серверов, в нём пока один сервер Default, который подключается к локальному серверу,
при желании можно добавить другой путём нажатия кнопки "Добавить сервер" и затем выбрать его в выпадающем списке.

Ниже есть таблица, в которой будут содержаться добавленные записи.
Для добавления записи необходимо нажать кнопку "Добавить пользователя", в появившемся окне ввести имя и почту пользователя
и нажать ОК, после чего запись появится в таблице.
Примечание: если сервер запущен не будет, запись в таблице не отобразится, так как не будет сохранена на сервере.

Под таблицей 2 кнопки: Удалить пользователя и добавить пользователя.
Кнопка "Удалить пользователя" доступна только в том случае, если выбрана запись в таблице.

Внутренняя работа приложения:
При добавлении новой записи, формируется Json-структура вида:
"action" : "insert",
"data" :
    {
        "userName" : "ivan"
        "email" : "ivan@dsf.co",
    },
"status" : "successful"

и отправляется на сервер через websocket в виде текстового сообщения.

На сервере эта сообщение парсится обратно в Json-структуру, определяется действие (action), относительно которого 
готовится и выполняется и выполняется соответствующий запрос в БД.
Примечание: при первом запуске сервера БД автоматически создаётся и в ней создаётся таблица пользователей users.

Если запрос прошёл без ошибок, то в Json-структуру добавляется поле status со значением successful и структура принимает
следующий вид:

"action" : "insert",
"data" :
    {
        "id" : 1,
        "userName" : "ivan"
        "email" : "ivan@dsf.co",
    },
"status" : "successful"

и отправляется обратно так же в виде текстового сообщения. Если запрос был на изменение данных(добавление, удаление), 
то эта структура рассылается и другим клиентам, если они подключены к данному серверу.

Клиент принимает данное сообщение, парсит его в Json-структуру и проверяет status и action. Если status == successful,
то выполняется операция в соответствии с действием, указанным в структуре.

Если при выполнении запроса произошла ошибка, то только клиенту-отправителю данные возвращается со статусом error. 

Описание действий:
    Действие            Операция на сервере         Операция на клиенте
    select              выгрузка данных             Отображение полученных данных
    insert              сохранение записи в БД      Добавление сохранённой записи в таблицу
    delete              удаление записи из БД       Удаление соответствующей записи из таблицы

Вид Json-структур для каждой операции:

SELECT:

запрос:
{ "action" : "select" }

ответ:
{
    "action" : "select",
    "data" :
        [
            {
                "email" : "ivan@dsf.co",
                "id" : 1,
                "userName" : "ivan"
            }
        ],
    "status" : "successful"
}


INSERT:

запрос:
{
    "action" : "insert",
    "data" :
        {
            "email" : "ivan@dsf.co",
            "userName" : "ivan"
        }
}


ответ:
{
    "action" : "insert",
    "data" :
        {
            "email" : "ivan@dsf.co",
            "id" : 1,
            "userName" : "ivan"
        },
    "status" : "successful"
}


DELETE:

запрос:
{
    "action" : "insert",
    "data" :
        {
            "id" : 1
        }
}


ответ:
{
    "action" : "delete",
    "data" :
        {
            "id" : 1
        },
    "status" : "successful"
}