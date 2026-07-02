# The FTC testing task

## Сборка

Для сборки достаточно иметь gcc, cmake, valgrind (для проверки на предмет утечек памяти и так далее):
```
# Ubuntu
sudo apt install -y gcc cmake valgrind

# Gnome
sudo dnf install -y gcc cmake valgrind
```

Для сборки с максимальным уровнем оптимизаций достаточно запустить скрипт `build.sh`
```bash
chmod +x build.sh
./build.sh
```
либо

```bash
bash ./build.sh
```
Для сборки с дебажным уровнем оптимизаций:
```
bash ./build.sh -b Debug
```

Для сборки с clang можно передавать переменную окружения:
```
CC=clang ./build.sh -b Debug
```

Для запуска утилиты-дампера (при условии, что входные данные `set1.dump` и `set2.dump` существуют):
```
./build/ftc_dump set1.dump set2.dump set3.dump
```

Для запуска тестов достаточно:
```bash
./build/ftc_test
```
С valgrind:

```bash
valgrind ./build/ftc_test
```

## Вывод тестов с valgrind

```log
potatocoder228@fedora:~/FCT_task$ valgrind ./build/ftc_test
==75501== Memcheck, a memory error detector
==75501== Copyright (C) 2002-2024, and GNU GPL'd, by Julian Seward et al.
==75501== Using Valgrind-3.26.0 and LibVEX; rerun with -h for copyright info
==75501== Command: ./build/ftc_test
==75501== 
testCreateDestroy: testCreateDestroy is success. Time: 0.000128s
testInsertSearch: testInsertSearch is success. Time: 0.000441s
testDelete: testDelete is success. Time: 0.000504s
testMinMaxSuccPred: testMinMaxSuccPred is success. Time: 0.000354s
testLargeInsertDelete: testLargeInsertDelete is success. Time: 0.005238s
testNullHandling: testNullHandling is success. Time: 0.000175s
testInvalidArgsAll is success. Time: 0.008389s
testInvalidArgs1 is success. Time: 0.007052s
testInvalidArgs2 is success. Time: 0.007130s
testInputNotExists is success. Time: 0.007057s
testOutputExists is success. Time: 0.016393s
testOutputPermissionDenied is success. Time: 0.017293s
testInputPermissionDenied is success. Time: 0.018020s
testSuccess is success. Time: 0.016254s
==75501== 
==75501== HEAP SUMMARY:
==75501==     in use at exit: 0 bytes in 0 blocks
==75501==   total heap usage: 2,652 allocs, 2,652 frees, 16,033,824 bytes allocated
==75501== 
==75501== All heap blocks were freed -- no leaks are possible
==75501== 
==75501== For lists of detected and suppressed errors, rerun with: -s
==75501== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

## Тесты

Ниже перечислены тесты, которые имело бы смысл добавить в принципе. Реализованы не все из них из-за ограничений по времени
и потому что некоторые вещи было бы удобнее генерировать более специализированными библиотеками (возможно).
1. Тест имён файлов (ничего не передали, уже существуют)
2. Тест, если прав на файлы не хватает (как на чтение, так и на запись в третий файл)
3. Тест, если данные в файлах записаны не до конца
4. Тест, если один из файлов пустой. Тест, если оба файла пустые
5. Тест с крайними значениями - в одном файле лишь 1 структура, второй пустой и наоборот, Тест с 100000 структур в одном файле и наоборот. Тест с обоими заполненными файлами с различным количеством структур - 1, 10, 50, 1000, 50000, 100000
6. Тест с 100000 одинаковых структур
7. Несколько тестов с случайно сгенерированными данными на 100000 структур
