#!/usr/bin/env python3
"""
Генератор случайных тестовых массивов для структуры StatData.
Выводит C-код с объявлениями массивов.

Сгенерирован с помощью DeepSeek
"""

import random
import argparse
import sys

def generate_random_record(rec_id):
    """Создаёт запись StatData со случайными значениями для заданного id."""
    return {
        'id': rec_id,
        'count': random.randint(1, 1000),
        'cost': round(random.uniform(0.001, 1000.0), 6),
        'primary': random.randint(0, 1),
        'mode': random.randint(0, 7)
    }

def format_record(rec, indent=4):
    """Форматирует одну запись в виде строки инициализатора C."""
    spaces = ' ' * indent
    return (f"{spaces}{{ .id = {rec['id']}L, .count = {rec['count']}, "
            f".cost = {rec['cost']:.6f}, .primary = {rec['primary']}, "
            f".mode = {rec['mode']} }}")

def generate_arrays(size_a, size_b, overlap, seed=None):
    """
    Генерирует три массива:
    - a: size_a записей
    - b: size_b записей, причём overlap id совпадают с id из a
    - out: объединение a и b без дубликатов (при совпадении id: count и cost суммируются,
           primary и mode берутся из b), отсортировано по cost возрастанию.
    """
    if seed is not None:
        random.seed(seed)

    # 1. Генерация уникальных id для a
    ids_a = set()
    while len(ids_a) < size_a:
        ids_a.add(random.randint(1, 10**9))

    # 2. Генерация id для b: берём overlap штук из a, остальные новые
    overlap_ids = set(random.sample(sorted(ids_a), min(overlap, len(ids_a))))
    ids_b = set(overlap_ids)
    while len(ids_b) < size_b:
        new_id = random.randint(1, 10**9)
        if new_id not in ids_a and new_id not in ids_b:
            ids_b.add(new_id)

    # 3. Создание записей для a и b (независимые значения)
    records_a = {id_: generate_random_record(id_) for id_ in ids_a}
    records_b = {id_: generate_random_record(id_) for id_ in ids_b}

    # 4. Построение выходного массива (объединение)
    out_dict = {}
    all_ids = set(ids_a) | set(ids_b)
    for id_ in all_ids:
        in_a = id_ in ids_a
        in_b = id_ in ids_b

        if in_a and in_b:
            # Объединяем: суммируем count и cost, primary и mode берём из b
            ra = records_a[id_]
            rb = records_b[id_]
            out_dict[id_] = {
                'id': id_,
                'count': ra['count'] + rb['count'],
                'cost': round(ra['cost'] + rb['cost'], 6),
                'primary': rb['primary'],
                'mode': rb['mode']
            }
        elif in_a:
            out_dict[id_] = records_a[id_].copy()
        else:  # только в b
            out_dict[id_] = records_b[id_].copy()

    # 5. Сортировка по cost (возрастание)
    out_records = sorted(out_dict.values(), key=lambda r: r['cost'])

    return records_a, records_b, out_records

def print_c_code(records_a, records_b, out_records, name_a='case_1_in_a',
                 name_b='case_1_in_b', name_out='case_1_out'):
    """Выводит C-код с объявлениями массивов."""
    def print_array(name, recs):
        size = len(recs)
        if size == 0:
            print(f"static StatData {name}[0] = {{}};")
            return
        print(f"static StatData {name}[{size}] = {{")
        for i, rec in enumerate(recs):
            line = format_record(rec, indent=4)
            if i == size - 1:
                line += " "
            else:
                line += ", "
            print(line)
        print("};")

    print_array(name_a, list(records_a.values()))
    print()
    print_array(name_b, list(records_b.values()))
    print()
    print_array(name_out, out_records)

def main():
    parser = argparse.ArgumentParser(
        description='Генерирует C-код с тестовыми массивами StatData.'
    )
    parser.add_argument('--size-a', type=int, default=5,
                        help='Количество элементов в массиве A (по умолчанию 5)')
    parser.add_argument('--size-b', type=int, default=5,
                        help='Количество элементов в массиве B (по умолчанию 5)')
    parser.add_argument('--overlap', type=int, default=None,
                        help='Количество общих id между A и B (не более min(size_a, size_b)). '
                             'Если не указано, берётся случайное число от 0 до min.')
    parser.add_argument('--seed', type=int, default=None,
                        help='Seed для генератора случайных чисел (для воспроизводимости)')
    parser.add_argument('--output', type=str, default=None,
                        help='Имя выходного файла. Если не указано, печатает в stdout.')
    args = parser.parse_args()

    size_a = args.size_a
    size_b = args.size_b
    overlap = args.overlap
    if overlap is None:
        overlap = random.randint(0, min(size_a, size_b))
    else:
        overlap = min(overlap, min(size_a, size_b))

    records_a, records_b, out_records = generate_arrays(
        size_a, size_b, overlap, seed=args.seed
    )

    # Вывод в файл или stdout
    if args.output:
        with open(args.output, 'w') as f:
            # Перенаправляем stdout в файл
            sys.stdout = f
            print_c_code(records_a, records_b, out_records)
            sys.stdout = sys.__stdout__
    else:
        print_c_code(records_a, records_b, out_records)

if __name__ == '__main__':
    main()