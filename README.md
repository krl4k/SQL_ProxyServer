# SQL_ProxyServer

### How to Start
    Необходимо создать конфигурационный фаил по примеру default.conf
    Отключить SSL
    Заполнить все необходимые поля. Запустить программу, подав вторым аргументом конфигурационный фаил
    В каждой директории присутствует Cmakelist для сборки проекта.

    

### /srcBoostProxyServer - реализация на boost
   * Boost::asio


### /proxyServer - реализация на сокетах Беркли

 * BSD_Sockets
 * Программа написана с помощью событийно-упрвляемого программирования
 * Неблокирующиеся вызовы(recv/send)
 * Select
   1. Вызов select блокирует выполнение программы до момента наступления событий на фаиловых дескрипторах.
   2. Такой подход позволяет не тратить зря процессорное время.


![Пример парсера](https://github.com/krl4k/SQL_ProxyServer/img/example.jpg)
