# Лабораторная работа 3

**Название:** "Разработка драйверов сетевых устройств"

**Цель работы:** получить знания и навыки разработки драйверов сетевых интерфейсов для операционной системы Linux


## Описание функциональности драйвера

Драйвер выводит пакеты длиной меньше 70 байт.
Состояние разбора пакетов выводится в файл `/proc/var5`

## Инструкция по сборке

- ``make`` -- собрать модуль ядра
- ``sudo insmod lab3.ko`` -- загрузить модуль в ядро
- ``sudo rmmod lab3.ko`` -- выгрузить модуль из ядра
- `make clean` -- удалить объектные файлы


## Инструкция пользователя
1. Собрать модуль
2. Загрузить модуль в ядро (см. инструкцию по сборке)
3. ``dmesg`` -- вывести кольцевой буфер ядра
4. `car /proc/var5` -- прочитать `/proc/var5`
5. Выгрузить модуль из ядра (см. инструкцию по сборке)
6. Удалить объектные файлы (см. инструкцию по сборке)

## Примеры использования
````
root@alkarized-IdeaPad-3-15ARE05:/home/alkarized/cio/lab3# ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host 
       valid_lft forever preferred_lft forever
2: wlp1s0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue state UP group default qlen 1000
    link/ether 8c:c8:4b:3f:f5:f1 brd ff:ff:ff:ff:ff:ff
    inet 172.30.85.174/18 brd 172.30.127.255 scope global dynamic noprefixroute wlp1s0
       valid_lft 82838sec preferred_lft 82838sec
    inet6 fe80::dad4:7981:ca86:9905/64 scope link noprefixroute 
       valid_lft forever preferred_lft forever
3: docker0: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc noqueue state DOWN group default 
    link/ether 02:42:ff:66:6b:b6 brd ff:ff:ff:ff:ff:ff
    inet 172.17.0.1/16 brd 172.17.255.255 scope global docker0
       valid_lft forever preferred_lft forever
11: vni0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UNKNOWN group default qlen 1000
    link/ether 8c:c8:4b:3f:f5:f1 brd ff:ff:ff:ff:ff:ff
    inet 172.30.85.174/18 brd 172.30.127.255 scope global dynamic noprefixroute vni0
       valid_lft 86067sec preferred_lft 86067sec
    inet6 fe80::5213:2311:a71a:ba1b/64 scope link noprefixroute 
       valid_lft forever preferred_lft forever
````
`cat /proc/var5`
````
----------------------SADDR: 172.30.85.174
DADDR: 224.0.0.22PROTOCOL: 2
SIZE: 40 ----------------------
SADDR: 0.0.0.0DADDR: 0.0.0.0
PROTOCOL: 0SIZE: 0
 ----------------------SADDR: 172.30.85.174
DADDR: 185.125.190.49PROTOCOL: 6
SIZE: 60 ----------------------
SADDR: 0.0.0.0DADDR: 0.0.0.0
PROTOCOL: 0SIZE: 0
 ----------------------SADDR: 185.125.190.49
DADDR: 172.30.85.174PROTOCOL: 6
SIZE: 60 ----------------------
SADDR: 172.30.85.174DADDR: 185.125.190.49
PROTOCOL: 6SIZE: 52
 ----------------------SADDR: 172.30.85.174
DADDR: 185.125.190.49PROTOCOL: 6
SIZE: 52 ----------------------
SADDR: 185.125.190.49DADDR: 172.30.85.174
PROTOCOL: 6SIZE: 52
 ----------------------SADDR: 172.30.85.174
DADDR: 185.125.190.49
PROTOCOL: 6SIZE: 52
 ----------------------SADDR: 0.0.0.0
DADDR: 0.0.0.0PROTOCOL: 0
SIZE: 0 ----------------------
SADDR: 185.125.190.49DADDR: 172.30.85.174
PROTOCOL: 6SIZE: 52
 ----------------------

----------------------SADDR: 172.30.85.174
DADDR: 224.0.0.22PROTOCOL: 2
SIZE: 40 ----------------------
SADDR: 0.0.0.0DADDR: 0.0.0.0
PROTOCOL: 0SIZE: 0
 ----------------------SADDR: 172.30.85.174
DADDR: 185.125.190.49PROTOCOL: 6
SIZE: 60 ----------------------
SADDR: 0.0.0.0DADDR: 0.0.0.0
PROTOCOL: 0SIZE: 0
 ----------------------SADDR: 185.125.190.49
DADDR: 172.30.85.174PROTOCOL: 6
SIZE: 60 ----------------------
SADDR: 172.30.85.174DADDR: 185.125.190.49
PROTOCOL: 6SIZE: 52
 ----------------------SADDR: 172.30.85.174
DADDR: 185.125.190.49PROTOCOL: 6
SIZE: 52 ----------------------
SADDR: 185.125.190.49DADDR: 172.30.85.174
PROTOCOL: 6SIZE: 52
 ----------------------SADDR: 172.30.85.174
DADDR: 185.125.190.49
PROTOCOL: 6SIZE: 52
 ----------------------SADDR: 0.0.0.0
DADDR: 0.0.0.0PROTOCOL: 0
SIZE: 0 ----------------------
SADDR: 185.125.190.49DADDR: 172.30.85.174
PROTOCOL: 6SIZE: 52
 ----------------------
````
`dmesg`
````
[ 3619.187008] -----------------------------
[ 3619.257377] Captured saddr: 172.30.85.174
[ 3619.257386] daddr: 216.58.210.131
[ 3619.257388] Package full length: 61
[ 3619.257390] Data: \x8as\x01\xbb
[ 3619.257392] Protocol_type: 17
[ 3619.257395] -----------------------------
[ 3619.264696] Captured saddr: 216.58.210.131
[ 3619.264703] daddr: 172.30.85.174
[ 3619.264704] Package full length: 53
[ 3619.264705] Data: \x01\xbb\x8as
[ 3619.264706] Protocol_type: 17
[ 3619.264709] -----------------------------
[ 3619.268345] Captured saddr: 216.58.210.131
[ 3619.268347] daddr: 172.30.85.174
[ 3619.268349] Package full length: 54
[ 3619.268350] Data: \x01\xbb\x8as
[ 3619.268351] Protocol_type: 17
[ 3619.268353] -----------------------------
[ 3619.572199] Captured saddr: 0.0.0.0
[ 3619.572207] daddr: 112.241.204.255
[ 3619.572209] Package full length: 0
[ 3619.572212] Data: 
[ 3619.572213] Protocol_type: 128
[ 3619.572216] -----------------------------
[ 3620.893178] Captured saddr: 172.30.85.174
[ 3620.893186] daddr: 149.154.167.99
[ 3620.893187] Package full length: 52
[ 3620.893188] Data: \xb3\xd6\x01\xbb\x03\xca@7lYE\xa7\x80\x10\x1a5_\x1c
[ 3620.893190] Protocol_type: 6
[ 3620.893192] -----------------------------
[ 3621.864894] Captured saddr: 172.30.85.174
[ 3621.864902] daddr: 209.85.233.194
[ 3621.864905] Package full length: 61
[ 3621.864907] Data: \xb8n\x01\xbb
[ 3621.864909] Protocol_type: 17
[ 3621.864912] -----------------------------
[ 3621.873447] Captured saddr: 209.85.233.194
[ 3621.873453] daddr: 172.30.85.174
[ 3621.873455] Package full length: 54
[ 3621.873456] Data: \x01\xbb\xb8n
[ 3621.873457] Protocol_type: 17
[ 3621.873460] -----------------------------
[ 3622.848205] Captured saddr: 172.30.85.174
[ 3622.848214] daddr: 149.154.167.99
[ 3622.848217] Package full length: 52
[ 3622.848219] Data: \xb3\xd6\x01\xbb\x03\xca@\xdelYF\x18\x80\x10\x1a5Tk
[ 3622.848221] Protocol_type: 6
[ 3622.848225] -----------------------------
[ 3623.110678] Captured saddr: 216.58.210.131
[ 3623.110687] daddr: 172.30.85.174
[ 3623.110689] Package full length: 61
[ 3623.110691] Data: \x01\xbb\x8as
[ 3623.110693] Protocol_type: 17
[ 3623.110696] -----------------------------
[ 3623.113941] Captured saddr: 216.58.210.131
[ 3623.113944] daddr: 172.30.85.174
[ 3623.113946] Package full length: 51
[ 3623.113947] Data: \x01\xbb\x8as
[ 3623.113947] Protocol_type: 17
[ 3623.113949] -----------------------------
[ 3623.114347] Captured saddr: 172.30.85.174
[ 3623.114357] daddr: 216.58.210.131
[ 3623.114359] Package full length: 67
[ 3623.114361] Data: \x8as\x01\xbb
[ 3623.114363] Protocol_type: 17
[ 3623.114366] -----------------------------
[ 3623.117045] Captured saddr: 172.30.85.174
[ 3623.117052] daddr: 216.58.210.131
[ 3623.117054] Package full length: 61
[ 3623.117056] Data: \x8as\x01\xbb
[ 3623.117057] Protocol_type: 17
[ 3623.117060] -----------------------------
[ 3623.156383] Captured saddr: 216.58.210.131
[ 3623.156392] daddr: 172.30.85.174
[ 3623.156394] Package full length: 52
[ 3623.156396] Data: \x01\xbb\x8as
[ 3623.156398] Protocol_type: 17
[ 3623.156401] -----------------------------
````
