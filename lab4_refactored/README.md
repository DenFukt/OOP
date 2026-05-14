# Лабораторна робота №4 — Керування нерухомістю

## Тема
Система керування об'єктами нерухомості (квартири, комерція, будинки).

## Технології
- **Мова:** C++17
- **DI/IoC контейнер:** Hypodermic
- **База даних:** SQLite3
- **Тести:** Catch2

---

## Архітектура (3 шари)

```
lab4/
├── models/               ← Шар моделей (сутності)
│   ├── Property.h        — базовий клас + struct Passport
│   ├── Apartment.h       — квартира
│   ├── Commercial.h      — комерційна нерухомість
│   └── OwnHouse.h        — приватний будинок
│
├── repository/           ← Шар даних (доступ до БД)
│   ├── IRepository.h     — інтерфейс репозиторію
│   └── SQLiteRepository.h— реалізація через SQLite
│
├── service/              ← Шар бізнес-логіки
│   ├── IEstateService.h  — інтерфейс сервісу
│   └── EstateService.h   — реалізація сервісу
│
├── utils/
│   └── DoublyLinkedList.h— двозв'язний список (шаблон)
│
├── Hypodermic/           — DI/IoC контейнер
├── sqlite3.h / sqlite3.c / sqlite3.o
├── catch.hpp             — фреймворк тестування
│
├── lab4.cpp              ← Точка входу (main + UI)
├── tests.cpp             ← Юніт-тести
└── .vscode/tasks.json    — налаштування збірки VSCode
```

---

## Принципи IoC / DI

У `main()` використовується **Hypodermic** як IoC-контейнер:

```cpp
Hypodermic::ContainerBuilder builder;
builder.registerType<SQLiteRepository>().as<IRepository>().singleInstance();
builder.registerType<EstateService>().as<IEstateService>().singleInstance();
auto container = builder.build();
auto service = container->resolve<IEstateService>();
```

`EstateService` не знає, яка саме БД використовується — отримує `IRepository` через конструктор (Constructor Injection).

---

## Збірка

### Основна програма
```bash
g++ lab4.cpp sqlite3.o -o lab4.exe -I. -I./Hypodermic -pthread -std=c++17
```

### Тести
```bash
g++ tests.cpp sqlite3.o -o tests.exe -I. -I./Hypodermic -pthread -std=c++17
./tests.exe
```

У VSCode: `Ctrl+Shift+B` → обрати потрібний таск.

---

## Функціонал

| Пункт меню | Дія |
|---|---|
| 1 | Показати всі об'єкти |
| 2 | Змінити ціну |
| 3 | Видалити об'єкт |
| 4 | Розпочати ремонт |
| 5 | Додати новий об'єкт |
| 6 | Здати/повернути з оренди |
| 7 | Придбати об'єкт |
| 8 | Розрахувати прибуток від оренди |
| 9 | Продати об'єкт |
| 10 | Загальна кількість об'єктів |
| 11 | Фільтр за максимальною ціною |
