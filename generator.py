#!/usr/bin/env python3
"""
Генератор случайных тестовых массивов для структуры StatData.
Выводит C-код с объявлениями массивов.

Правила объединения записей с одинаковым id:
- count и cost суммируются,
- primary = логическое И (1 только если оба равны 1),
- mode = максимум из двух.
"""

import random
import argparse
import sys

# Максимальные значения для типов (для контроля переполнения)
MAX_COUNT = 1000       # чтобы сумма двух не превысила INT_MAX
MAX_COST = 1000.0      # чтобы сумма оставалась в диапазоне float

def generate_random_record(rec_id):
    """Создаёт запись StatData со случайными значениями для заданного id."""
    return {
        "id": rec_id,
        "count": random.randint(1, MAX_COUNT),
        "cost": round(random.uniform(0.001, MAX_COST), 6),
        "primary": random.randint(0, 1),
        "mode": random.randint(0, 7),
    }

def format_record(rec, indent=4):
    """Форматирует одну запись в виде строки инициализатора C."""
    spaces = " " * indent
    return (
        f"{spaces}{{ .id = {rec['id']}L, .count = {rec['count']}, "
        f".cost = {rec['cost']:.6f}, .primary = {rec['primary']}, "
        f".mode = {rec['mode']} }}"
    )

def merge_records(a_records, b_records):
    """
    Объединяет два списка записей по id.
    Возвращает список записей с уникальными id и объединёнными полями.
    """
    merged = {}
    for rec in a_records + b_records:
        id_ = rec['id']
        if id_ not in merged:
            merged[id_] = rec.copy()
        else:
            # Суммируем count и cost
            merged[id_]['count'] += rec['count']
            merged[id_]['cost'] += rec['cost']
            # primary = AND
            merged[id_]['primary'] = 1 if (merged[id_]['primary'] and rec['primary']) else 0
            # mode = max
            if rec['mode'] > merged[id_]['mode']:
                merged[id_]['mode'] = rec['mode']
    # Сортировка по cost возрастанию
    result = sorted(merged.values(), key=lambda r: r['cost'])
    return result

def generate_arrays(size_a, size_b, overlap, seed=None):
    """
    Генерирует три массива:
    - a: size_a записей
    - b: size_b записей, причём overlap id совпадают с id из a
    - out: объединение a и b по правилам merge_records, отсортировано по cost.
    """
    if seed is not None:
        random.seed(seed)

    # Генерация уникальных id для a
    ids_a = set()
    while len(ids_a) < size_a:
        ids_a.add(random.randint(1, 10**9))

    # Генерация id для b: берём overlap штук из a, остальные новые
    overlap_ids = set(random.sample(sorted(ids_a), min(overlap, len(ids_a))))
    ids_b = set(overlap_ids)
    while len(ids_b) < size_b:
        new_id = random.randint(1, 10**9)
        if new_id not in ids_a and new_id not in ids_b:
            ids_b.add(new_id)

    # Создание записей для a и b (независимые значения)
    records_a = {id_: generate_random_record(id_) for id_ in ids_a}
    records_b = {id_: generate_random_record(id_) for id_ in ids_b}

    # Объединение через merge_records
    out_records = merge_records(list(records_a.values()), list(records_b.values()))

    return records_a, records_b, out_records

def print_c_code(records_a, records_b, out_records,
                 name_a="case_1_in_a", name_b="case_1_in_b", name_out="case_1_out"):
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

    print("""
#pragma once
#include "dumper.h"
#include <stdlib.h>
#include <string.h>
        """)
    print_array(name_a, list(records_a.values()))
    print()
    print_array(name_b, list(records_b.values()))
    print()
    print_array(name_out, out_records)

def main():
    parser = argparse.ArgumentParser(
        description="Генерирует C-код с тестовыми массивами StatData."
    )
    parser.add_argument(
        "--size-a", type=int, default=5,
        help="Количество элементов в массиве A (по умолчанию 5)"
    )
    parser.add_argument(
        "--size-b", type=int, default=5,
        help="Количество элементов в массиве B (по умолчанию 5)"
    )
    parser.add_argument(
        "--overlap", type=int, default=None,
        help="Количество общих id между A и B (не более min(size_a, size_b)). "
             "Если не указано, берётся случайное число от 0 до min."
    )
    parser.add_argument(
        "--seed", type=int, default=None,
        help="Seed для генератора случайных чисел (для воспроизводимости)"
    )
    parser.add_argument(
        "--output", type=str, default=None,
        help="Имя выходного файла. Если не указано, печатает в stdout."
    )
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

    if args.output:
        with open(args.output, "w") as f:
            sys.stdout = f
            print_c_code(records_a, records_b, out_records)
            sys.stdout = sys.__stdout__
    else:
        print_c_code(records_a, records_b, out_records)

if __name__ == "__main__":
    main()
