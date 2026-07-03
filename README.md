# The FTC testing task

## Дисклеймер

Весь код утилиты был написан самостоятельно - AI притягивался для CI, поиска ошибок, ревью уже написанного кода.
Поэтому мелкие недочёты могут быть (например, русскоязычные комментарии, которые я пропустил).


## Сборка

Для сборки достаточно иметь gcc, cmake, valgrind (для проверки на предмет утечек памяти и так далее):
```
# Ubuntu
sudo apt install -y gcc cmake valgrind

# Gnome
sudo dnf install gcc cmake valgrind
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

## Алгоритм работы JoinDump

Для реализации алгоритма (при условии, что порядок данных нам важен и мы не можем их произвольно сортировать), необходима была структура, которая могла бы выполнять быстрый поиск за  $O(log_2 N)$ или $O(1)$.

Выбор не велик - либо хэшмап, либо самобалансирующееся дерево.
У хэшмапы - быстрее поиск в лучшем случае ($O(1)$), но штрафы на пересчитывание хэшэй при изменении количества "корзин".
У красно-чёрного дерева таких недостатков нет, но лучшее время $O(log_2 N)$.

Так как обе структуры относительно сложны в реализации без ошибок, то я взял свою реализацию rbtree, которую когда-то делал.

https://github.com/PotatoCoder228/ImaqliqTestTask/blob/master/include/common/rb_tree_s.h

В ней было множество ошибок, поэтому интерфейс был покрыт дополнительными тестами, код отправлен на ревью через AI, и исправлены утечки памяти.

В любом случае, реализация с rbtree очень быстрая, даже на миллионе элементов.


## Вывод тестов с valgrind

```log
==88203== Memcheck, a memory error detector
==88203== Copyright (C) 2002-2024, and GNU GPL'd, by Julian Seward et al.
==88203== Using Valgrind-3.26.0 and LibVEX; rerun with -h for copyright info
==88203== Command: ./build/ftc_test
==88203== 
testCreateDestroy: testCreateDestroy is success. Time: 0.000129s
testInsertSearch: testInsertSearch is success. Time: 0.000449s
testDelete: testDelete is success. Time: 0.000488s
testMinMaxSuccPred: testMinMaxSuccPred is success. Time: 0.000388s
testLargeInsertDelete: testLargeInsertDelete is success. Time: 0.005720s
testNullHandling: testNullHandling is success. Time: 0.000174s
testDataVectorCreateDestroy: testDataVectorCreateDestroy is success. Time: 0.000087s
testDataVectorPushPop: testDataVectorPushPop is success. Time: 0.000459s
testDataVectorGetSet: testDataVectorGetSet is success. Time: 0.000252s
testDataVectorLarge: testDataVectorLarge is success. Time: 0.001040s
testDataVectorNullHandling: testDataVectorNullHandling is success. Time: 0.000111s
testInvalidArgsAll is success. Time: 0.008520s
testInvalidArgs1 is success. Time: 0.007282s
testInvalidArgs2 is success. Time: 0.007128s
testInputNotExists is success. Time: 0.007194s
testOutputExists is success. Time: 0.024946s
testOutputPermissionDenied is success. Time: 0.027952s
testInputPermissionDenied is success. Time: 0.031592s
testSuccess is success. Time: 0.020240s
==88203== 
==88203== HEAP SUMMARY:
==88203==     in use at exit: 0 bytes in 0 blocks
==88203==   total heap usage: 2,892 allocs, 2,892 frees, 16,817,184 bytes allocated
==88203== 
==88203== All heap blocks were freed -- no leaks are possible
==88203== 
==88203== For lists of detected and suppressed errors, rerun with: -s
==88203== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

## Документация

Документация была сгенерирована с помощью AI, проверена мной для основных интерфейсов и задеплоена на github.pages:

https://potatocoder228.github.io/ftc_task/index.html

## Тесты

Ниже перечислены тесты, которые имело бы смысл добавить в принципе. Реализованы не все из них из-за ограничений по времени и потому что некоторые вещи было бы удобнее генерировать более специализированными библиотеками (возможно).
1. Тест имён файлов (ничего не передали, уже существуют)
2. Тест, если прав на файлы не хватает (как на чтение, так и на запись в третий файл)
3. Тест, если данные в файлах записаны не до конца
4. Тест, если один из файлов пустой. Тест, если оба файла пустые
5. Тест с крайними значениями - в одном файле лишь 1 структура, второй пустой и наоборот, Тест с 100000 структур в одном файле и наоборот. Тест с обоими заполненными файлами с различным количеством структур - 1, 10, 50, 1000, 50000, 100000
6. Тест с 100000 одинаковых структур
7. Несколько тестов с случайно сгенерированными данными на 100000 структур

Также я постарался сделать покрытие основных используемых мной контейнеров - красно-чёрного дерева и вектора с `StatData*`

Плюс в результате сложений происходит накопительная ошибка, поэтому в тестах сравнение ведётся в некоторой окружности +- 1 десятая. Всё зависит от количества данных.
Не уверен, нужна ли обработка от переполнений, мало что можно сделать.

## Генератор тестовых данных

Для задач тестирования на больших объёмах данных, был сделан генератор: 
```bash
python3 generator.py --size-a 5000 --size-b 5000 --output test_data.h

# Заменить данные
cat test_data.h > include/test_data.h
```
Безусловно, лучше было бы это автоматизировать или сделать более гибко, но как есть

## Замечания

Надо причесать codestyle.