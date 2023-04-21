## Описание задачи
---
Задача описана в oldReadme.md
Используются с++17/boost/STL/Cmake

## Запуск
---

Собирается проект как обычно с помощью cmake, например так:
/usr/bin/cmake --build /home/arslan/snap/stockMarket/build --config Debug --target all -j 10 --

Тестировалось на компиляторе GCC 11.3.0 x86_64-linux-gnu, 

Итого создается 3 бинарника в директории build (если вводилась вышеописанная команда):

ClientRun - запуск клиента, работающего в командной строке. При запуске выводит help, где можно увидеть все описания доступных команд и их использование. Тк добавленая авторизация, сначала нужно зарегистрировать клиента (help подскажет) и затем залогиниться.

ServerRun - запуск сервера. Чтобы клиент подключился, нужно запустить сначала сервер.

allTests - запуск всех юнит-тестов. Написаны только для сервера (и лежат в директории server/tests), тк в целом тесты имитируют подключение с клиента. Иначе получилось бы дублирование логики тестов и на клиенте и на сервере (ну и экономия времени).

## Описание логики
---

Однопоточный синхронный клиент подключается к асинхронному однопоточному серверу. 

Основная логика написана в классе Core. Внутри него скрыты 2 кучи, каждый элемент которой соответствует стоимости валюты: одна max-куча для заявок на покупку, другая - min-куча для заявок на продажу. В процессе Core::match верхушки этих куч совершают сделки, тк эти стоимости являются наиболее выгодными для соответствующих клиентов. Тк кучи содержат только стоимости (своеобразный индекс) то добавлены 2 мапы (для покупки/продажи), которые содержат актуальные заявки. Элементы этих мап: стоимость -> [ссылки на все актуальные заявки с этой стоимостью].

### ℹ️ Выполненные условия:
---

**Сервер**

- [x] Поддерживает подключения нескольких клиентов одновременно. [По сути это поддерживает каркас из коробки]
- [x] Даёт возможность просмотра клиентом всех своих активных торговых заявок, всех своих совершенных сделок и своего баланса. [команды active, history и balance (изначально у всех 0)]

**Клиент**

- [x] Подключается к серверу и реализует все его возможности.

🎈 **Бонусы:**

- [x] Возможность просмотра клиентом котировок 📈 [команда quote]
- [ ] Получение отчёта о совершённой сделке в момент её заключения всеми сторонами-участниками. 🤼
- [x] Возможность отмены активных заявок клиентами. 🚫 [команда cancel, узнать id можно с помощью active]
- [ ] Сохранение истории всех заявок и сделок в базу данных (например, PostgreSQL). 💽
- [x] Авторизацию клиента с паролем. 🔑 [Сделал простую вариацию с самописным токеном]
- [ ] GUI к бирже на QT. 🤯