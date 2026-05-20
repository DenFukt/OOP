# 🏠 Estate Manager — Лабораторна робота №5

## Технології
C++17 · SQLite · cpp-httplib · Firebase REST API · Docker

---

## Архітектура

```
lab5/
├── models/          Property, Apartment, Commercial, OwnHouse
├── factory/         PropertyFactory       ← Creational (Factory Method)
├── observer/        IObserver, EventLogger← Behavioral (Observer)
├── facade/          EstateApiFacade       ← Structural (Facade)
├── repository/      IRepository, SQLiteRepository
├── cloud/           ICloudSync, FirebaseSync
├── service/         IEstateService, EstateService
├── monitoring/      HealthCheck, TransactionLog, ApiInsights
├── utils/           DoublyLinkedList, JsonParser
├── frontend/        index.html (SPA)
├── main.cpp         HTTP-сервер, DI wiring
├── tests.cpp        22 юніт-тести (Catch2)
├── Makefile
├── Dockerfile
└── docker-compose.yml
```

---

## Дизайн-патерни

| Категорія   | Патерн         | Файл                        | Опис |
|-------------|----------------|-----------------------------|------|
| Creational  | Factory Method | factory/PropertyFactory.h   | Централізує `new Apartment/Commercial/OwnHouse` |
| Structural  | Facade         | facade/EstateApiFacade.h    | Спрощує HTTP ↔ бізнес-логіка |
| Behavioral  | Observer       | observer/EventLogger.h      | EventLogger + TransactionLog підписуються на події |

---

## Запуск

### Локально (Linux/WSL)
```bash
make          # завантажує httplib.h, компілює
./lab5        # запуск на порту 8080
```

### Docker
```bash
docker-compose up --build
```
Відкрий: http://localhost:8080

### Тести
```bash
make tests
./tests
```

---

## API ендпоінти

| Метод  | Шлях                              | Опис |
|--------|-----------------------------------|------|
| GET    | /api/properties                   | Всі об'єкти |
| POST   | /api/properties                   | Додати об'єкт |
| DELETE | /api/properties/:name             | Видалити |
| PUT    | /api/properties/:name/rent        | Здати/повернути оренду |
| PUT    | /api/properties/:name/buy         | Купити |
| PUT    | /api/properties/:name/sell        | Продати |
| PUT    | /api/properties/:name/price       | Змінити ціну |
| GET    | /api/income                       | Дохід від оренди |
| GET    | **api/health**                    | Стан сервісу |
| GET    | **api/transactions**              | Лог транзакцій |
| GET    | **api/insights**                  | API-статистика |

---

## Firebase (cloud sync)

1. Створи проєкт на https://firebase.google.com
2. Увімкни Realtime Database
3. Встанови змінну середовища:
```bash
export FIREBASE_URL="https://<project>-default-rtdb.firebaseio.com"
./lab5
```
Або в docker-compose.yml:
```yaml
environment:
  FIREBASE_URL: "https://my-project-default-rtdb.firebaseio.com"
```
